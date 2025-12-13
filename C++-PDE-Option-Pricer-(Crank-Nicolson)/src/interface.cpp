#include "Interface.h"

Interface::Interface() 
    : S0(100), K(100), T(1), r(0.05), sigma(0.2), 
      S_max(300), theta(0.5), M(100), N(100), isCall(true) {}

void Interface::askParameters() {
    std::cout << "==========================================" << std::endl;
    std::cout << "      PRICER D'OPTION - CONFIGURATION     " << std::endl;
    std::cout << "==========================================" << std::endl;

    std::cout << "[1] Type d'option (1 = Call, 0 = Put) : ";
    int type;
    if (std::cin >> type) isCall = (type == 1);

    std::cout << "[2] Prix du sous-jacent (S0) : ";
    std::cin >> S0;

    std::cout << "[3] Strike (K) : ";
    std::cin >> K;

    std::cout << "[4] Maturite (T en annees) : ";
    std::cin >> T;

    std::cout << "[5] Taux sans risque (r) : ";
    std::cin >> r;

    std::cout << "[6] Volatilite (sigma) : ";
    std::cin >> sigma;

    std::cout << "\n--- Parametres du Moteur EDP ---" << std::endl;

    // Calcul automatique suggéré pour S_max
    double defaultSmax = K * 4.0;
    std::cout << "[7] S_max (Borne haute, suggestion: " << defaultSmax << ") : ";
    std::cin >> S_max;

    std::cout << "[8] Nombre de pas de temps (M) : ";
    std::cin >> M;

    std::cout << "[9] Nombre de pas d'espace (N) : ";
    std::cin >> N;
    
    std::cout << "[10] Theta (0.5 = Crank-Nicolson) : ";
    std::cin >> theta;

    std::cout << "\n==========================================" << std::endl;
    std::cout << "      Lancement du calcul...              " << std::endl;
    std::cout << "==========================================\n" << std::endl;
}
