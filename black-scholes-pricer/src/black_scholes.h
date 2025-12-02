#ifndef BLACK_SCHOLES_H
#define BLACK_SCHOLES_H

struct BSResult {
    double call_price;
    double put_price;

    double delta_call;
    double delta_put;

    double gamma;
    double vega;

    double theta_call;
    double theta_put;

    double rho_call;
    double rho_put;
};

BSResult black_scholes_with_dividends(double S, double K, double r,
                                      double q, double sigma, double T);

#endif // BLACK_SCHOLES_H
