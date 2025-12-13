#pragma once
#include "Payoff.h"
#include <vector>

// Structure pour renvoyer le Prix ET les Sensibilités d'un coup
struct PricingResults {
    double price;  // V (Prix)
    double delta;  // dV/dS
    double gamma;  // d2V/dS2
    double theta;  // dV/dt (laissé à 0 ici)
};

class PDESolver {
private:
    // Paramètres du modèle
    double T, r, sigma;
    
    // Paramètres de la grille
    double S_max;
    size_t N; // Nombre de points d'espace
    size_t M; // Nombre de points de temps

    // Pas de discrétisation
    double dt;
    double dx;

    // Vecteurs pour les coefficients des matrices (constants grâce au log-transform)
    std::vector<double> B_lower, B_diag, B_upper; // Partie Explicite (droite)
    std::vector<double> A_lower, A_diag, A_upper; // Partie Implicite (gauche - à inverser)

public:
    // Constructeur
    PDESolver(double T, double r, double sigma, double S_max, size_t N, size_t M);

    // Pré-calcul des matrices (Optimisation)
    void precomputeMatrices();

    // Méthode principale de pricing qui renvoie la structure
    PricingResults solve(const Payoff& payoff, double S0);
};
