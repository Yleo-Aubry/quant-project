#pragma once
#include <algorithm> // Pour std::max

/*
 * CLASSE PAYOFF (Interface)
 * C'est le "Contrat" de l'option.
 * Elle définit combien l'option rapporte à maturité en fonction du prix du sous-jacent.
 */
class Payoff {
public:
    virtual ~Payoff() {}; // Indispensable pour nettoyer la mémoire proprement
    
    // L'opérateur () permet d'utiliser l'objet comme une fonction.
    // Ex: monPayoff(100.0) renvoie la valeur si le spot est 100.
    virtual double operator()(double spot) const = 0;
};

// --- Implémentation du CALL ---
class PayoffCall : public Payoff {
private:
    double K; // Strike price
public:
    PayoffCall(double strike) : K(strike) {}

    // Payoff du Call : max(S - K, 0)
    double operator()(double spot) const override {
        return std::max(spot - K, 0.0);
    }
};

// --- Implémentation du PUT ---
class PayoffPut : public Payoff {
private:
    double K; // Strike price
public:
    PayoffPut(double strike) : K(strike) {}

    // Payoff du Put : max(K - S, 0)
    double operator()(double spot) const override {
        return std::max(K - spot, 0.0);
    }
};
