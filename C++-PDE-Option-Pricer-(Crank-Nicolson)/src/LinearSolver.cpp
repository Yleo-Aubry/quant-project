#include "LinearSolver.h"
#include <stdexcept> // Pour la gestion des erreurs (exceptions)
#include <cmath>     // Pour std::abs

namespace Solver {

    void thomasAlgorithm(const std::vector<double>& a,
                         const std::vector<double>& b,
                         const std::vector<double>& c,
                         const std::vector<double>& d,
                         std::vector<double>& x) {
        
        // Récupération de la taille du système
        size_t n = d.size();

        // --- VALIDATION DES ENTRÉES ---
        // On s'assure que tous les vecteurs ont la même dimension N.
        // C'est crucial pour éviter les "Segmentation Faults".
        if (a.size() != n || b.size() != n || c.size() != n || x.size() != n) {
            throw std::invalid_argument("Erreur Solver: Les vecteurs a, b, c, d, x doivent avoir la meme taille.");
        }

        // --- PRÉPARATION ---
        // L'algorithme de Thomas modifie les coefficients c et d durant la descente.
        // Comme nos entrées sont 'const' (lecture seule pour sécurité), 
        // nous devons créer des copies de travail temporaires : c_prime et d_prime.
        std::vector<double> c_prime(n);
        std::vector<double> d_prime(n);

        // --- ÉTAPE 1 : FORWARD ELIMINATION (Descente) ---
        // Le but est d'éliminer la diagonale inférieure 'a'.
        
        // Traitement du premier élément (i=0) spécifique car il n'a pas de prédécesseur.
        double pivot = b[0];
        
        // Vérification de la stabilité numérique : division par zéro
        if (std::abs(pivot) < 1e-15) {
             throw std::runtime_error("Erreur Solver: Pivot nul detecte a l'indice 0. Matrice singuliere ?");
        }
        
        // Calcul des nouveaux coefficients pour la première ligne
        c_prime[0] = c[0] / pivot;
        d_prime[0] = d[0] / pivot;

        // Boucle sur les lignes suivantes (de 1 à N-1)
        for (size_t i = 1; i < n; ++i) {
            // Le dénominateur correspond au nouveau pivot après élimination
            double denominator = b[i] - a[i] * c_prime[i - 1];
            
            // Vérification de sécurité à chaque étape
            if (std::abs(denominator) < 1e-15) {
                throw std::runtime_error("Erreur Solver: Pivot nul detecte a l'indice " + std::to_string(i));
            }
            
            double temp = 1.0 / denominator;
            
            // Calcul du c' (attention, le dernier c[n-1] n'existe pas, on l'ignore)
            if (i < n - 1) {
                c_prime[i] = c[i] * temp;
            }
            
            // Calcul du d'
            d_prime[i] = (d[i] - a[i] * d_prime[i - 1]) * temp;
        }

        // --- ÉTAPE 2 : BACKWARD SUBSTITUTION (Remontée) ---
        // On résout le système en partant de la fin, car la dernière ligne ne dépend que d'une variable.
        
        // Le dernier élément x[n-1] est directement égal à d'[n-1]
        x[n - 1] = d_prime[n - 1];

        // On remonte de l'avant-dernier élément (n-2) jusqu'au début (0).
        // Note : On utilise 'int' pour l'index 'i' car la condition de boucle est i >= 0.
        // Avec un 'size_t' (non signé), i >= 0 est toujours vrai, ce qui créerait une boucle infinie.
        for (int i = static_cast<int>(n) - 2; i >= 0; --i) {
            x[i] = d_prime[i] - c_prime[i] * x[i + 1];
        }
        
        // À la fin de cette fonction, le vecteur 'x' contient la solution du système.
    }
}
