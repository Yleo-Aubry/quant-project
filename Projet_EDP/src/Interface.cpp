#include "edp/Interface.h"
#include <iostream>

namespace edp {



void Interface::askRunMode() {
    std::cout << "==========================================" << std::endl;
    std::cout << "      PRICER D'OPTION & SUITE DE TESTS    " << std::endl;
    std::cout << "==========================================" << std::endl;
    std::cout << "Veuillez choisir le mode d'execution :" << std::endl;
    std::cout << "[1] Pricer une option (Standard)" << std::endl;
    std::cout << "[2] Lancer les tests de validation PDE (Black-Scholes)" << std::endl;
    std::cout << "[3] Lancer les tests unitaires du Solver Lineaire" << std::endl;
    std::cout << "Votre choix : ";
    
    int choice = 0;
    std::cin >> choice;

    // Conversion sécurisée de l'entier vers l'Enum Class
    switch (choice) {
        case 1: runMode = RunMode::Pricer; break;
        case 2: runMode = RunMode::TestPDE; break;
        case 3: runMode = RunMode::TestSolver; break;
        default: runMode = RunMode::Unknown; break;
    }
    std::cout << std::endl;
}

void Interface::askParameters() {
    std::cout << "==========================================" << std::endl;
    std::cout << "      PRICER D'OPTION - CONFIGURATION     " << std::endl;
    std::cout << "==========================================" << std::endl;
    
    std::cout << "[1] Type d'option (1 = Call, 0 = Put) : ";
    int type;
    if (std::cin >> type) {
        isCall = (type == 1);
    }

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

    double defaultSmax = K * 4.0;
    std::cout << "[7] S_max (Borne haute, suggestion: " << defaultSmax << ") : ";
    std::cin >> S_max;

    std::cout << "[8] Nombre de pas de temps (M) : ";
    std::cin >> M;

    std::cout << "[9] Nombre de pas d'espace (N) : ";
    std::cin >> N;
    
    
    std::cout << "[10] Theta (0.5 = Crank-Nicolson, 1.0 = Implicite) : ";
    std::cin >> theta_scheme;

    std::cout << "\n==========================================" << std::endl;
    std::cout << "      Lancement du calcul...              " << std::endl;
    std::cout << "==========================================\n" << std::endl;
}

} // namespace edp