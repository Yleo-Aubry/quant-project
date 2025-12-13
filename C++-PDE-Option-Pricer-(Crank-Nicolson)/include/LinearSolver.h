#pragma once
#include <vector>


namespace Solver {

    /**
     * @brief Résout un système linéaire tridiagonal Ax = d via l'algorithme de Thomas.
     * * L'algorithme de Thomas est une simplification du Pivot de Gauss optimisée
     * pour les matrices tridiagonales. Il est beaucoup plus rapide : O(N) contre O(N^3).
     * * La matrice A est définie par trois vecteurs :
     * - a : diagonale inférieure (lower)
     * - b : diagonale principale (main)
     * - c : diagonale supérieure (upper)
     * * @param a Vecteur de la diagonale inférieure (taille N). a[0] n'est pas utilisé mathématiquement.
     * @param b Vecteur de la diagonale principale (taille N).
     * @param c Vecteur de la diagonale supérieure (taille N). c[N-1] n'est pas utilisé.
     * @param d Vecteur du second membre (taille N).
     * @param x Vecteur résultat (taille N). Il sera rempli par la fonction.
     * * @throw std::invalid_argument Si les tailles des vecteurs ne correspondent pas.
     * @throw std::runtime_error Si le système est singulier (division par zéro).
     */
    void thomasAlgorithm(const std::vector<double>& a,
                         const std::vector<double>& b,
                         const std::vector<double>& c,
                         const std::vector<double>& d,
                         std::vector<double>& x);
}
