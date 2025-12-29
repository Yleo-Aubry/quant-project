#ifndef EDP_PAYOFF_H
#define EDP_PAYOFF_H

#include <algorithm> // Pour std::max

namespace edp {

    /*
     * CLASSE PAYOFF 
     * Contrat : Définit le flux financier à maturité.
     * Utilisation : Header-only pour maximiser l'inlining lors des boucles de calcul.
     */
    class Payoff {
    public:
        
        virtual ~Payoff() = default;
        
        // Opérateur pur pour calcul du payoff
        [[nodiscard]] virtual double operator()(double spot) const = 0;
    };

    // --- Implémentation du CALL ---
    class PayoffCall : public Payoff {
    private:
        double K; // Strike price
    public:
        explicit PayoffCall(double strike) : K(strike) {}

        [[nodiscard]] double operator()(double spot) const override {
            return std::max(spot - K, 0.0);
        }
    };

    // --- Implémentation du PUT ---
    class PayoffPut : public Payoff {
    private:
        double K; // Strike price
    public:
        explicit PayoffPut(double strike) : K(strike) {}

        [[nodiscard]] double operator()(double spot) const override {
            return std::max(K - spot, 0.0);
        }
    };

} // namespace edp

#endif // EDP_PAYOFF_H