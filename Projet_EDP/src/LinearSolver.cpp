#include "edp/LinearSolver.h"
#include <stdexcept> 
#include <cmath>     
#include <string>    

namespace edp { 

    void thomasAlgorithm(const std::vector<double>& a,
                         const std::vector<double>& b,
                         const std::vector<double>& c,
                         const std::vector<double>& d,
                         std::vector<double>& x) {
        
        size_t n = d.size();

        // --- VALIDATION ---
        if (n == 0) {
            throw std::invalid_argument("Erreur Solver: Le systeme est vide.");
        }
        if (a.size() != n || b.size() != n || c.size() != n) {
            throw std::invalid_argument("Erreur Solver: Dimensions des vecteurs a, b, c incoherentes.");
        }

        // --- PRÉPARATION ---
        // Redimensionnement automatique du vecteur résultat (Service rendu à l'appelant)
        if (x.size() != n) {
            x.resize(n);
        }

        // Copies de travail
        std::vector<double> c_prime = c; // Copie car modifié
        std::vector<double> d_prime = d; // Copie car modifié

        // --- ÉTAPE 1 : DESCENTE ---
        double pivot = b[0];
        
        if (std::abs(pivot) < 1e-15) {
             throw std::runtime_error("Erreur Solver: Pivot nul a l'indice 0.");
        }
        
        c_prime[0] /= pivot;
        d_prime[0] /= pivot;

        for (size_t i = 1; i < n; ++i) {
            double denominator = b[i] - a[i] * c_prime[i - 1];
            
            if (std::abs(denominator) < 1e-15) {
                throw std::runtime_error("Erreur Solver: Pivot nul a l'indice " + std::to_string(i));
            }
            
            double temp = 1.0 / denominator;
            
            if (i < n - 1) {
                c_prime[i] *= temp;
            }
            
            d_prime[i] = (d[i] - a[i] * d_prime[i - 1]) * temp;
        }

        // --- ÉTAPE 2 : REMONTÉE ---
        x[n - 1] = d_prime[n - 1];

        // Boucle inversée sécurisée avec size_t 
        // On évite le cast (int) et les problèmes de signe
        for (size_t i = n - 2; i != static_cast<size_t>(-1); --i) {
            x[i] = d_prime[i] - c_prime[i] * x[i + 1];
        }
    }

} // namespace edp