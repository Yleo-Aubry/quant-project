#include "PDESolver.h"
#include "LinearSolver.h"
#include <cmath>
#include <vector>
#include <algorithm>
#include <iostream>

// Constructeur : Initialisation des paramètres et de la grille
PDESolver::PDESolver(double T_, double r_, double sigma_, double S_max_, size_t N_, size_t M_)
    : T(T_), r(r_), sigma(sigma_), S_max(S_max_), N(N_), M(M_) {
    
    dt = T / M;
    
    // Grille logarithmique : x = ln(S)
    // On choisit x_min correspondant à un S très petit mais > 0
    double x_min = std::log(S_max / 3000.0); 
    double x_max = std::log(S_max);
    
    // Pas d'espace constant en log
    dx = (x_max - x_min) / (N - 1);
}

// Pré-calcul des matrices A et B (fait une seule fois pour gagner du temps)
void PDESolver::precomputeMatrices() {
    double sigma2 = sigma * sigma;
    double nu = r - 0.5 * sigma2; // Drift sous la mesure risque-neutre log
    
    // --- COEFFICIENTS CRANK-NICOLSON ---
    // lambda : facteur lié à la diffusion
    // Formule : 0.5 (Crank-Nicolson) * 0.5 * sigma^2 (Black-Scholes) * dt/dx^2
    // Donc : 0.25 * dt * sigma^2 / dx^2
    double lambda = 0.25 * dt / (dx * dx);
    
    // gamma : facteur lié à la convection
    // Formule : 0.5 (Crank-Nicolson) * nu * dt/(2*dx) -> non, attention à la dérivée centrale
    // Dérivée convection centrale : (V_{i+1} - V_{i-1}) / 2dx
    // Donc coeff : 0.5 (CN) * nu * dt / (2*dx) = 0.25 * dt * nu / dx
    
    // Pour plus de clarté, calculons les coefficients alpha et beta finaux
    double alpha = lambda * sigma2;         // Partie diffusion pure
    double beta  = (0.25 * dt * nu) / dx;   // Partie convection pure

    size_t systemSize = N - 2;
    
    A_lower.resize(systemSize);
    A_diag.resize(systemSize);
    A_upper.resize(systemSize);
    
    B_lower.resize(systemSize);
    B_diag.resize(systemSize);
    B_upper.resize(systemSize);

    for (size_t i = 0; i < systemSize; ++i) {
        // Matrice A (Gauche - Implicite - Temps n) : (I - dt/2 * L)
        A_lower[i] = -alpha + beta;
        A_diag[i]  = 1.0 + 2.0 * alpha + 0.5 * r * dt;
        A_upper[i] = -alpha - beta;

        // Matrice B (Droite - Explicite - Temps n+1) : (I + dt/2 * L)
        B_lower[i] = alpha - beta;
        B_diag[i]  = 1.0 - 2.0 * alpha - 0.5 * r * dt;
        B_upper[i] = alpha + beta;
    }
}

PricingResults PDESolver::solve(const Payoff& payoff, double S0) {
    precomputeMatrices();

    // Reconstruction locale de la grille x pour être sûr des indices
    double x_min = std::log(S_max / 3000.0);
    double dx_computed = (std::log(S_max) - x_min) / (N - 1);
    
    std::vector<double> x(N);
    std::vector<double> V(N); // Prix de l'option

    // 1. Condition Terminale (Payoff à t=T)
    for (size_t i = 0; i < N; ++i) {
        x[i] = x_min + i * dx_computed;
        V[i] = payoff(std::exp(x[i])); // V = Payoff(S)
    }

    std::vector<double> d(N - 2);     // Second membre
    std::vector<double> V_next(N - 2); // Solution du système

    // 2. Boucle Temporelle (Backward : Maturité -> Aujourd'hui)
    for (size_t t = 0; t < M; ++t) {
        // Temps restant jusqu'à maturité
        // t=0 correspond au premier pas en arrière partant de T
        double time_to_maturity = (t + 1) * dt;

        // --- CONDITIONS AUX LIMITES EXACTES (Dirichlet) ---
        
        // Limite Gauche (S -> 0) : Pour un Call, vaut 0
        double V_boundary_left = 0.0; 

        // Limite Droite (S -> Infinity)
        // Comportement asymptotique Call : S - K * exp(-r * (T-t))
        double S_max_val = std::exp(x[N-1]);
        // Astuce pour récupérer K implicite : K = S_max - Payoff(S_max)
        double K_approx = S_max_val - payoff(S_max_val);
        double V_boundary_right = S_max_val - K_approx * std::exp(-r * time_to_maturity);

        // --- Construction du second membre d (Partie Explicite) ---
        for (size_t i = 0; i < N - 2; ++i) {
            // B * V_ancien
            d[i] = B_lower[i] * V[i] + B_diag[i] * V[i+1] + B_upper[i] * V[i+2];
        }

        // Injection des conditions aux limites (Passage au second membre)
        d[0]   -= A_lower[0] * V_boundary_left;
        d[N-3] -= A_upper[N-3] * V_boundary_right;

        // Résolution du système Ax = d via Thomas
        Solver::thomasAlgorithm(A_lower, A_diag, A_upper, d, V_next);

        // Mise à jour de V pour l'étape suivante
        for (size_t i = 0; i < N - 2; ++i) {
            V[i+1] = V_next[i];
        }
        // Ré-application des bords
        V[0] = V_boundary_left;
        V[N-1] = V_boundary_right;
    }

    // 3. Calcul des Résultats (Prix + Grecques) à S0
    double target_x = std::log(S0);
    
    // Recherche de l'index i tel que x[i] <= target_x < x[i+1]
    size_t i = 0;
    while (i < N-1 && x[i+1] < target_x) {
        i++;
    }

    // A. Interpolation du PRIX
    double ratio = (target_x - x[i]) / dx_computed;
    double price = V[i] * (1.0 - ratio) + V[i+1] * ratio;

    // B. Calcul du DELTA (dV/dS)
    // Chain rule : dV/dS = (dV/dx) * (1/S)
    // Approx dV/dx par différences finies centrées sur la grille log si possible
    // Ici on fait simple : forward difference sur l'intervalle d'interpolation
    double dV_dx = (V[i+1] - V[i]) / dx_computed;
    double delta = dV_dx / S0;

    // C. Calcul du GAMMA (d2V/dS2)
    // Formule : Gamma = (d2V/dx2 - dV/dx) / S^2
    double d2V_dx2 = 0.0;
    // On a besoin de 3 points. Si on est au bord, on simplifie.
    if (i > 0 && i < N - 1) {
        d2V_dx2 = (V[i+1] - 2.0 * V[i] + V[i-1]) / (dx_computed * dx_computed);
    }
    double gamma = (d2V_dx2 - dV_dx) / (S0 * S0);

    return {price, delta, gamma, 0.0};
}
