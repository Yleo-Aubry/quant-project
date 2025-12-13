#ifndef INTERFACE_H
#define INTERFACE_H

#include <iostream>
#include <limits>

class Interface {
private:
    // Paramètres financiers
    double S0;      // Prix du sous-jacent (Spot)
    double K;       // Strike
    double T;       // Maturité
    double r;       // Taux d'intérêt
    double sigma;   // Volatilité

    // Paramètres Grille / Solver
    double S_max;   // Borne haute du maillage (nécessaire pour PDESolver)
    double theta;   // Paramètre temporel (0.5 = Crank-Nicolson)
    size_t M;       // Pas de temps (type size_t comme dans PDESolver)
    size_t N;       // Pas d'espace

    bool isCall;    // Type d'option

public:
    Interface();
    
    // Fonction principale d'interaction
    void askParameters();

    // Getters
    double getS0() const { return S0; }
    double getK() const { return K; }
    double getT() const { return T; }
    double getR() const { return r; }
    double getSigma() const { return sigma; }
    
    // Le solver demande S_max, N et M
    double getS_max() const { return S_max; }
    size_t getM() const { return M; }
    size_t getN() const { return N; }
    
    // Note: Ton PDESolver actuel ne prend pas theta dans son constructeur,
    // mais je le garde ici au cas où tu modifies le solver plus tard.
    double getTheta() const { return theta; }
    
    bool getIsCall() const { return isCall; }
};

#endif
