#include "edp/PDESolver.h"
#include "edp/Payoff.h"

#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <algorithm>

// === OUTILS ANALYTIQUES (Black-Scholes) ===

// CDF normale standard
static double norm_cdf(double x) {
    return 0.5 * std::erfc(-x / std::sqrt(2.0));
}

// Prix analytique Black–Scholes Call européen
static double bs_call_price(double S0, double K, double T, double r, double sigma) {
    if (T <= 0.0) {
        return std::max(S0 - K, 0.0);
    }

    double vol_sqrtT = sigma * std::sqrt(T);
    double d1 = (std::log(S0 / K) + (r + 0.5 * sigma * sigma) * T) / vol_sqrtT;
    double d2 = d1 - vol_sqrtT;

    return S0 * norm_cdf(d1) - K * std::exp(-r * T) * norm_cdf(d2);
}

// === STRUCTURE DE DONNÉES ===

struct TestCase {
    double S0;
    double K;
    double T;
    double r;
    double sigma;
};

// === FONCTION PRINCIPALE ===

int main() {
    // Paramètres de grille fixes pour le benchmark
    const std::size_t N = 250;  // Espace
    const std::size_t M = 2500; // Temps

    // === 20 CAS DE TEST ===
    std::vector<TestCase> tests = {
        {100, 100, 1.0, 0.05, 0.20},
        { 80, 100, 1.0, 0.05, 0.20},
        {120, 100, 1.0, 0.05, 0.20},
        {100,  90, 1.0, 0.05, 0.20},
        {100, 110, 1.0, 0.05, 0.20},

        {100, 100, 0.5, 0.05, 0.20},
        {100, 100, 2.0, 0.05, 0.20},
        {100, 100, 1.0, 0.01, 0.20},
        {100, 100, 1.0, 0.10, 0.20},
        {100, 100, 1.0, 0.05, 0.10},

        {100, 100, 1.0, 0.05, 0.40},
        { 95, 100, 1.0, 0.03, 0.25},
        {105, 100, 1.0, 0.03, 0.25},
        { 90, 100, 2.0, 0.02, 0.30},
        {110, 100, 2.0, 0.02, 0.30},

        { 70, 100, 1.5, 0.04, 0.35},
        {130, 100, 1.5, 0.04, 0.35},
        {150, 100, 0.75,0.06, 0.18},
        {100, 120, 1.5, 0.04, 0.35},
        {100,  80, 1.5, 0.04, 0.35}
    };

    // Configuration de la sortie console (CSV)
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "id,S0,K,T,r,sigma,price_BS,price_PDE,abs_error\n";

    for (std::size_t i = 0; i < tests.size(); ++i) {
        const auto& t = tests[i];

        // Définition d'un S_max assez large pour éviter les effets de bord
        double S_max = std::max(5.0 * t.K, 300.0);

       
        edp::PDESolver solver(t.T, t.r, t.sigma, S_max, 0.5, N, M);
        
        // Création du Payoff
        edp::PayoffCall payoff(t.K);

        // Résolution
        edp::PricingResults pde_res = solver.solve(payoff, t.S0);
        
        // Comparaison avec l'analytique
        double bs = bs_call_price(t.S0, t.K, t.T, t.r, t.sigma);
        double err = std::fabs(pde_res.price - bs);

        std::cout
            << (i + 1) << ","
            << t.S0 << ","
            << t.K << ","
            << t.T << ","
            << t.r << ","
            << t.sigma << ","
            << bs << ","
            << pde_res.price << ","
            << err << "\n";
    }

    return 0;
}