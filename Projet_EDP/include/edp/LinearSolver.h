#ifndef EDP_LINEARSOLVER_H
#define EDP_LINEARSOLVER_H

#include <vector>
#include <stdexcept> 

namespace edp {

    /**
     * @brief Résout un système linéaire tridiagonal Ax = d via l'algorithme de Thomas.
     * * L'algorithme de Thomas est une simplification du Pivot de Gauss optimisée
     * pour les matrices tridiagonales. Complexité : O(N).
     * * La matrice A est définie par trois vecteurs :
     * - a : diagonale inférieure (lower)
     * - b : diagonale principale (main)
     * - c : diagonale supérieure (upper)
     * * @param a Vecteur de la diagonale inférieure (taille N). a[0] n'est pas utilisé.
     * @param b Vecteur de la diagonale principale (taille N).
     * @param c Vecteur de la diagonale supérieure (taille N). c[N-1] n'est pas utilisé.
     * @param d Vecteur du second membre (taille N).
     * @param x Vecteur résultat (taille N). La fonction redimensionnera si nécessaire.
     * * @throw std::invalid_argument Si les tailles des vecteurs sont incohérentes.
     * @throw std::runtime_error Si le système est singulier (b[i] = 0 après pivot).
     */
    void thomasAlgorithm(const std::vector<double>& a,
                         const std::vector<double>& b,
                         const std::vector<double>& c,
                         const std::vector<double>& d,
                         std::vector<double>& x);

} // namespace edp

#endif // EDP_LINEARSOLVER_H