#ifndef EDP_INTERFACE_H
#define EDP_INTERFACE_H

#include <cstddef> 

namespace edp {

enum class RunMode {
    Pricer,
    TestPDE,
    TestSolver,
    Unknown
};

class Interface {
private:
    RunMode runMode = RunMode::Unknown;

    // Paramètres financiers
    double S0 = 0.0;      
    double K = 0.0;       
    double T = 0.0;       
    double r = 0.0;       
    double sigma = 0.0;   

    // Paramètres Grille / Solver
    double S_max = 0.0;   
    double theta_scheme = 0.5; 
    size_t M = 100;       
    size_t N = 100;      

    bool isCall = true;    

public:
    Interface() = default; 
    
    void askRunMode();
    void askParameters(); 

    // Getters
    [[nodiscard]] RunMode getRunMode() const { return runMode; }

    [[nodiscard]] double getS0() const { return S0; }
    [[nodiscard]] double getK() const { return K; }
    [[nodiscard]] double getT() const { return T; }
    [[nodiscard]] double getR() const { return r; }
    [[nodiscard]] double getSigma() const { return sigma; }
    
    [[nodiscard]] double getS_max() const { return S_max; }
    [[nodiscard]] size_t getM() const { return M; }
    [[nodiscard]] size_t getN() const { return N; }
    
    // RENOMMÉ ICI AUSSI
    [[nodiscard]] double getThetaScheme() const { return theta_scheme; }
    
    [[nodiscard]] bool getIsCall() const { return isCall; }
};

} // namespace edp

#endif // EDP_INTERFACE_H