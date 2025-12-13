#include <iostream>
#include <memory> // Pour std::unique_ptr (plus propre que new/delete manuel)

// Nos headers
#include "Interface.h"
#include "PDESolver.h"
#include "Payoff.h"

int main() {
    // 1. Initialisation de l'interface et saisie
    Interface ui;
    ui.askParameters();

    // 2. Création du Payoff (Call ou Put)
    // On utilise un pointeur de base 'Payoff*' pour le polymorphisme
    Payoff* payoff = nullptr;
    
    if (ui.getIsCall()) {
        payoff = new PayoffCall(ui.getK());
    } else {
        payoff = new PayoffPut(ui.getK());
    }

    // 3. Initialisation du Solver
    // Attention à l'ordre des arguments défini dans PDESolver.h :
    // PDESolver(T, r, sigma, S_max, N, M)
    PDESolver solver(
        ui.getT(),
        ui.getR(),
        ui.getSigma(),
        ui.getS_max(),
        ui.getN(),
        ui.getM()
    );

    // Note : Ton PDESolver actuel ne semble pas prendre 'theta' dans le constructeur.
    // Si tu modifies PDESolver.cpp pour l'utiliser, tu l'ajouteras ici via ui.getTheta().

    // 4. Lancement du Pricing
    // La méthode solve prend le payoff et le S0 (Spot initial)
    PricingResults res = solver.solve(*payoff, ui.getS0());

    // 5. Affichage des résultats
    std::cout << ">>> RESULTATS <<<" << std::endl;
    std::cout << "-----------------" << std::endl;
    std::cout << "Prix de l'option : " << res.price << std::endl;
    std::cout << "Delta            : " << res.delta << std::endl;
    std::cout << "Gamma            : " << res.gamma << std::endl;
    // std::cout << "Theta (Grecque)  : " << res.theta << std::endl; // Si calculé

    // Nettoyage de la mémoire
    delete payoff;

    return 0;
}
