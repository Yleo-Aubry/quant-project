#include <iostream>
#include <iomanip>
#include "black_scholes.h"

int main() {
    double S, K, T, r, sigma, q;

    std::cout << "=== Black–Scholes Pricer (C++) ===\n\n";

    std::cout << "Spot S = ";
    std::cin >> S;

    std::cout << "Strike K = ";
    std::cin >> K;

    std::cout << "Maturity T (years) = ";
    std::cin >> T;

    std::cout << "Risk-free rate r (e.g. 0.05 for 5%) = ";
    std::cin >> r;

    std::cout << "Volatility sigma (e.g. 0.20 for 20%) = ";
    std::cin >> sigma;

    std::cout << "Dividend yield q (0 if none) = ";
    std::cin >> q;

    BSResult res = black_scholes_with_dividends(S, K, r, q, sigma, T);

    std::cout << std::fixed << std::setprecision(6);

    std::cout << "\n=== Results ===\n";
    std::cout << "Call price : " << res.call_price << "\n";
    std::cout << "Put  price : " << res.put_price  << "\n\n";

    std::cout << "--- Greeks (Call) ---\n";
    std::cout << "Delta (Call) : " << res.delta_call << "\n";
    std::cout << "Gamma        : " << res.gamma      << "\n";
    std::cout << "Vega         : " << res.vega       << "\n";
    std::cout << "Theta (Call) : " << res.theta_call << "\n";
    std::cout << "Rho   (Call) : " << res.rho_call   << "\n\n";

    std::cout << "--- Greeks (Put) ---\n";
    std::cout << "Delta (Put)  : " << res.delta_put  << "\n";
    std::cout << "Theta (Put)  : " << res.theta_put  << "\n";
    std::cout << "Rho   (Put)  : " << res.rho_put    << "\n";

    return 0;
}
