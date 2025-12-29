#include <iostream>
#include <memory> 
#include <iomanip>

#include "edp/Interface.h"
#include "edp/PDESolver.h"
#include "edp/Payoff.h"

int main() {
    try {
        // 1. Initialisation de l'interface
        edp::Interface ui;
        
        // 2. Demande du mode d'exécution
        ui.askRunMode();

        // 3. Aiguillage selon le choix
        
        if (ui.getRunMode() != edp::RunMode::Pricer) {
            std::cout << "\n[INFO ARCHITECTURE]" << std::endl;
            std::cout << "Les modules de tests ont ete compiles dans des executables separes." << std::endl;
            std::cout << "Veuillez lancer './Test_LinearSolver' ou './Test_PDESolver' depuis le dossier build." << std::endl;
            std::cout << "Ce programme est dedie au Pricing uniquement." << std::endl;
            return 0;
        }

        // --- MODE PRICER ---
        
        // Saisie des paramètres financiers et numériques
        ui.askParameters(); 

        // Création du Payoff via un Smart Pointer (Gestion mémoire automatique)
        std::unique_ptr<edp::Payoff> payoff;
        
        if (ui.getIsCall()) {
            payoff = std::make_unique<edp::PayoffCall>(ui.getK());
        } else {
            payoff = std::make_unique<edp::PayoffPut>(ui.getK());
        }

        // Initialisation du Solver
        
        edp::PDESolver solver(
            ui.getT(),
            ui.getR(),
            ui.getSigma(),
            ui.getS_max(),
            ui.getThetaScheme(), // 0.5 = CN, 1.0 = Implicite
            ui.getN(),
            ui.getM()
        );

        // Lancement du Pricing
        
        edp::PricingResults res = solver.solve(*payoff, ui.getS0());

        // Affichage des résultats
        std::cout << ">>> RESULTATS <<<" << std::endl;
        std::cout << "-----------------" << std::endl;
        std::cout << std::fixed << std::setprecision(5);
        std::cout << "Prix de l'option : " << res.price << std::endl;
        std::cout << "Delta            : " << res.delta << std::endl;
        std::cout << "Gamma            : " << res.gamma << std::endl;
        std::cout << "Theta (Schema)   : " << ui.getThetaScheme() << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "ERREUR FATALE : " << e.what() << std::endl;
        return 1;
    }

    return 0;
}