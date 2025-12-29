#include "edp/LinearSolver.h" // Correction du chemin
#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <algorithm>

// Multiplication matrice tridiagonale * vecteur
// d = A * x
// Utile pour vérifier si A * x_trouvé est proche de d_initial
static std::vector<double> tridiag_mul(
    const std::vector<double>& a,
    const std::vector<double>& b,
    const std::vector<double>& c,
    const std::vector<double>& x
) {
    std::size_t n = x.size();
    std::vector<double> d(n, 0.0);
    for (std::size_t i = 0; i < n; ++i) {
        d[i] += b[i] * x[i];
        if (i > 0)     d[i] += a[i] * x[i - 1];
        if (i + 1 < n) d[i] += c[i] * x[i + 1];
    }
    return d;
}

void runBenchmark() {
    // Entête du CSV
    std::cout << "test_id,n,variation_strength,i,x_true,x,error\n";
    std::cout << std::scientific << std::setprecision(10);

    std::vector<std::size_t> sizes = {5, 10, 20, 40, 80};
    std::vector<double> strengths = {0.0, 0.01, 0.05, 0.10};

    int test_id = 1;

    for (std::size_t n : sizes) {
        for (double s : strengths) {

            // 1. Construction d'une matrice tridiagonale test
            std::vector<double> a(n, 0.0), b(n, 0.0), c(n, 0.0);
            for (std::size_t i = 0; i < n; ++i) {
                if (i > 0)     a[i] = -1.0 - s * i;
                if (i + 1 < n) c[i] = -1.0 + s * i;

                // Assurance de la dominance diagonale stricte pour stabilité
                double off = 0.0;
                if (i > 0)     off += std::abs(a[i]);
                if (i + 1 < n) off += std::abs(c[i]);

                b[i] = 2.0 + off; 
            }

            // 2. Solution exacte arbitraire
            std::vector<double> x_true(n);
            for (std::size_t i = 0; i < n; ++i)
                x_true[i] = 1.0 + 0.1 * static_cast<double>(i);

            // 3. Calcul du second membre d correspondant (problème inverse)
            std::vector<double> d = tridiag_mul(a, b, c, x_true);

            // 4. Résolution par Thomas 
            std::vector<double> x(n, 0.0);
            try {
                
                edp::thomasAlgorithm(a, b, c, d, x);
            } catch (const std::exception& e) {
                std::cerr << "Erreur fatale test " << test_id << ": " << e.what() << std::endl;
                continue;
            }

            // 5. Sortie CSV
            for (std::size_t i = 0; i < n; ++i) {
                double err = std::abs(x[i] - x_true[i]);
                std::cout
                    << test_id << ","
                    << n << ","
                    << s << ","
                    << i << ","
                    << x_true[i] << ","
                    << x[i] << ","
                    << err
                    << "\n";
            }
            ++test_id;
        }
    }
}

int main() {
    try {
        runBenchmark();
    } catch (const std::exception& e) {
        std::cerr << "Exception non gérée : " << e.what() << std::endl;
        return 1;
    }
    return 0;
}