#ifndef EDP_PDESOLVER_H
#define EDP_PDESOLVER_H

#include "edp/Payoff.h" 
#include <vector>
#include <cstddef>      

namespace edp {

    // Structure pour les résultats financiers
    struct PricingResults {
        double price;  // V (Prix)
        double delta;  // dV/dS
        double gamma;  // d2V/dS2
        double theta;  // dV/dt (Grecque temporelle)
    };

    class PDESolver {
    private:
        // Paramètres financiers
        double T, r, sigma;
        
        // Paramètres numériques
        double S_max;
        double theta_scheme; // Le theta de Crank-Nicolson (0.5 = CN, 1.0 = Implicite)
        size_t N;            // Points d'espace
        size_t M;            // Points de temps

        // Pas de discrétisation
        double dt;
        double dx;

        // Vecteurs coefficients (stockés pour éviter la réallocation)
        // A = Matrice Implicite (Future), B = Matrice Explicite (Passée)
        std::vector<double> B_lower, B_diag, B_upper; 
        std::vector<double> A_lower, A_diag, A_upper; 

    public:
        PDESolver(double T, double r, double sigma, 
                  double S_max, double theta_scheme, 
                  size_t N, size_t M);

        // Interdiction de la copie 
        PDESolver(const PDESolver&) = delete;
        PDESolver& operator=(const PDESolver&) = delete;

        // Pré-calcul des matrices (indépendant du Payoff)
        void precomputeMatrices();

        // Résolution : Prend S0 pour interpoler le résultat final
        [[nodiscard]] PricingResults solve(const Payoff& payoff, double S0);
    };

} // namespace edp

#endif // EDP_PDESOLVER_H