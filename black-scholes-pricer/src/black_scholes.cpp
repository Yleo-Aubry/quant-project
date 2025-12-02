#include "black_scholes.h"
#include <cmath>
#include <limits>

// PDF de la loi normale standard
static double norm_pdf(double x) {
    const double INV_SQRT_2PI = 0.39894228040143267794; // 1/sqrt(2*pi)
    return INV_SQRT_2PI * std::exp(-0.5 * x * x);
}

// CDF de la loi normale standard
static double norm_cdf(double x) {
    return 0.5 * std::erfc(-x / std::sqrt(2.0));
}

BSResult black_scholes_with_dividends(double S, double K, double r,
                                      double q, double sigma, double T) {
    BSResult res{};

    if (S <= 0.0 || K <= 0.0 || sigma <= 0.0 || T <= 0.0) {
        double nan = std::numeric_limits<double>::quiet_NaN();
        res.call_price = res.put_price = nan;
        res.delta_call = res.delta_put = nan;
        res.gamma = res.vega = nan;
        res.theta_call = res.theta_put = nan;
        res.rho_call = res.rho_put = nan;
        return res;
    }

    double sqrtT = std::sqrt(T);
    double d1 = (std::log(S / K) + (r - q + 0.5 * sigma * sigma) * T) /
                (sigma * sqrtT);
    double d2 = d1 - sigma * sqrtT;

    double Nd1       = norm_cdf(d1);
    double Nd2       = norm_cdf(d2);
    double Nminus_d1 = norm_cdf(-d1);
    double Nminus_d2 = norm_cdf(-d2);
    double pdf_d1    = norm_pdf(d1);

    double disc_r = std::exp(-r * T);
    double disc_q = std::exp(-q * T);

    // Prix
    res.call_price = S * disc_q * Nd1 - K * disc_r * Nd2;
    res.put_price  = K * disc_r * Nminus_d2 - S * disc_q * Nminus_d1;

    // Greeks Call / Put
    res.delta_call = disc_q * Nd1;
    res.delta_put  = disc_q * (Nd1 - 1.0);

    res.gamma = (disc_q * pdf_d1) / (S * sigma * sqrtT);

    // Vega (par 1 de sigma, pas 1%)
    res.vega = S * disc_q * pdf_d1 * sqrtT;

    // Theta (par an)
    res.theta_call =
        - (S * disc_q * pdf_d1 * sigma) / (2.0 * sqrtT)
        + q * S * disc_q * Nd1
        - r * K * disc_r * Nd2;

    res.theta_put =
        - (S * disc_q * pdf_d1 * sigma) / (2.0 * sqrtT)
        - q * S * disc_q * Nminus_d1
        + r * K * disc_r * Nminus_d2;

    // Rho
    res.rho_call = K * T * disc_r * Nd2;
    res.rho_put  = -K * T * disc_r * Nminus_d2;

    return res;
}
