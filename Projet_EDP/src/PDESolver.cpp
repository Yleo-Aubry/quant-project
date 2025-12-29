#include "edp/PDESolver.h"
#include "edp/LinearSolver.h" 
#include <cmath>
#include <vector>
#include <algorithm>
#include <stdexcept>

namespace edp {

// Constructeur : Initialisation des paramètres
PDESolver::PDESolver(double T_, double r_, double sigma_, 
                     double S_max_, double theta_scheme_, 
                     size_t N_, size_t M_)
    : T(T_), r(r_), sigma(sigma_), 
      S_max(S_max_), theta_scheme(theta_scheme_), 
      N(N_), M(M_) {
    
    // Calcul des pas de discrétisation
    dt = T / static_cast<double>(M);
    
    // Grille logarithmique : x = ln(S)
    // On évite log(0) en prenant une borne basse petite mais strictement positive
    double S_min = S_max / 3000.0; 
    double x_min = std::log(S_min); 
    double x_max = std::log(S_max);
    
    dx = (x_max - x_min) / static_cast<double>(N - 1);
}

// Pré-calcul des matrices A (Implicite) et B (Explicite)
// Basé sur le Theta-Schéma généralisé
void PDESolver::precomputeMatrices() {
    // Paramètres de l'équation transformée (Log-space)
    // dV/dt + (r - sigma^2/2) dV/dx + 1/2 sigma^2 d2V/dx2 - rV = 0
    double sigma2 = sigma * sigma;
    double nu = r - 0.5 * sigma2; // Drift sous log-measure

    // Coefficients de discrétisation de base (sans Theta)
    // Diffusion : coeff devant d2V/dx2 * dt/dx^2
    double lambda = (sigma2 * dt) / (dx * dx); 
    // Convection : coeff devant dV/dx * dt/(2dx) (Différences finies centrées)
    double gamma  = (nu * dt) / (2.0 * dx); 
    // Réaction : r * dt
    double rho    = r * dt;

    size_t systemSize = N - 2;
    
    A_lower.resize(systemSize);
    A_diag.resize(systemSize);
    A_upper.resize(systemSize);
    
    B_lower.resize(systemSize);
    B_diag.resize(systemSize);
    B_upper.resize(systemSize);

    // Construction des matrices selon le Theta-scheme
    // LHS (A) : Partie Future (Implicite) -> Poids theta
    // RHS (B) : Partie Passée (Explicite) -> Poids (1 - theta)
    
    //  On remonte le temps (Backward), donc :
    // V_old = V_new + dt * L(V)
    // V_old = (I - (1-theta) dt L) V_n + (theta dt L) V_{n+1} ... 
    // La convention standard pour A * V_{new} = B * V_{old} donne :

    for (size_t i = 0; i < systemSize; ++i) {
        // --- Matrice A (Gauche - Implicite) ---
        // Diagonale : 1 + theta * (termes sortants)
        A_lower[i] = theta_scheme * (-0.5 * lambda + gamma); 
        A_diag[i]  = 1.0 + theta_scheme * (lambda + rho);
        A_upper[i] = theta_scheme * (-0.5 * lambda - gamma);

        // --- Matrice B (Droite - Explicite) ---
        // Diagonale : 1 - (1-theta) * (termes sortants)
        B_lower[i] = (1.0 - theta_scheme) * (0.5 * lambda - gamma);
        B_diag[i]  = 1.0 - (1.0 - theta_scheme) * (lambda + rho);
        B_upper[i] = (1.0 - theta_scheme) * (0.5 * lambda + gamma);
    }
}

PricingResults PDESolver::solve(const Payoff& payoff, double S0) {
    // 1. Préparation
    precomputeMatrices();

    // Reconstruction de la grille pour les calculs de conditions aux limites
    double x_min = std::log(S_max / 3000.0);
    
    
    std::vector<double> x(N);
    std::vector<double> S(N); // On stocke aussi S pour éviter exp() répétés
    std::vector<double> V(N); 

    // 2. Condition Terminale (Payoff à t=T)
    for (size_t i = 0; i < N; ++i) {
        x[i] = x_min + i * dx;
        S[i] = std::exp(x[i]);
        V[i] = payoff(S[i]); 
    }

    std::vector<double> d(N - 2);     // Second membre du système linéaire
    std::vector<double> V_solve(N - 2); // Vecteur résultat du solveur linéaire

    // 3. Boucle Temporelle (Backward)
    for (size_t t = 0; t < M; ++t) {
        // Temps restant jusqu'à maturité pour la prochaine étape (t+1)
        double time_next = (t + 1) * dt;

        // --- CONDITIONS AUX LIMITES (Dirichlet Dynamique) ---
        // On utilise l'approximation : V(Boundary) approx Payoff(Boundary) * Discount
        // Cela fonctionne pour Call et Put sans "if" explicite.
        // Pour être plus précis sur un Call (S -> infini), V ~ S (pas d'escompte sur S).
        
        
        // Limite Gauche (S -> 0)
        double S_low = S[0];
        double V_boundary_left = payoff(S_low) * std::exp(-r * time_next);

        // Limite Droite (S -> S_max)
        // Pour un Call, V ~ S - K*exp(-rt). Pour un Put, V ~ 0.
        // On utilise le Payoff pour détecter la valeur intrinsèque.
        // Si c'est un Call, payoff(S_max) = S_max - K.
        // La valeur actuelle est S_max - K * exp(-rt).
        // On reconstitue K implicite : K_approx = S_max - payoff(S_max).
        double S_high = S[N-1];
        double val_intrinsic = payoff(S_high);
        // Si val_intrinsic est proche de 0 (Put OTM), c'est 0.
        // Si val_intrinsic est grand (Call ITM), on ajuste le strike.
        double V_boundary_right = 0.0;
        
        if (val_intrinsic > S_high * 0.1) { 
             double K_implied = S_high - val_intrinsic;
             V_boundary_right = S_high - K_implied * std::exp(-r * time_next);
        } else {
             
             V_boundary_right = val_intrinsic * std::exp(-r * time_next);
        }

        // --- Construction du second membre d (Partie Explicite) ---
        for (size_t i = 0; i < N - 2; ++i) {
            d[i] = B_lower[i] * V[i] + B_diag[i] * V[i+1] + B_upper[i] * V[i+2];
        }

        // Injection des conditions aux limites
        d[0]     -= A_lower[0] * V_boundary_left;
        d[N-3]   -= A_upper[N-3] * V_boundary_right;

        // Résolution
        thomasAlgorithm(A_lower, A_diag, A_upper, d, V_solve);

        // Mise à jour de la solution globale
        for (size_t i = 0; i < N - 2; ++i) {
            V[i+1] = V_solve[i];
        }
        V[0]   = V_boundary_left;
        V[N-1] = V_boundary_right;
    }

    // 4. Interpolation et calcul des Grecques
    double target_x = std::log(S0);
    
    // Recherche dichotomique ou linéaire de l'indice
    // On s'assure de ne pas sortir des bornes pour le calcul de Gamma (i >= 1)
    size_t i = 1; 
    while (i < N - 2 && x[i+1] < target_x) {
        i++;
    }

    // A. Interpolation du PRIX
    double ratio = (target_x - x[i]) / dx;
    double price = V[i] * (1.0 - ratio) + V[i+1] * ratio;

    // B. Calcul du DELTA et GAMMA (Différences finies sur la grille log)
    // Chain rule : dV/dS = (dV/dx) * (1/S)
    double dV_dx = (V[i+1] - V[i-1]) / (2.0 * dx); // Différence centrée meilleure
    double delta = dV_dx / S[i];

    // Gamma = (d2V/dS2) = (d2V/dx2 - dV/dx) / S^2
    double d2V_dx2 = (V[i+1] - 2.0 * V[i] + V[i-1]) / (dx * dx);
    double gamma = (d2V_dx2 - dV_dx) / (S[i] * S[i]);

    // Theta (Temporel) : On pourrait le calculer en stockant V_old, 
    // mais ici on renvoie 0.0 ou une approx simple
    return {price, delta, gamma, 0.0};
}

} // namespace edp