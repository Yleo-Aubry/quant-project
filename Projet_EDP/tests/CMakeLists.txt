# Activation des fonctionnalités de test standard de CMake
include(CTest)

# ==========================================
# TEST 1 : Benchmark du Solveur Linéaire
# ==========================================
add_executable(Test_LinearSolver Test_LinearSolver.cpp)

# Liaison avec le cœur de la librairie 
target_link_libraries(Test_LinearSolver PRIVATE EDP_Core)

# Drapeaux de compilation stricts 
target_compile_options(Test_LinearSolver PRIVATE -Wall -Wextra -Werror)

# Enregistrement du test (pour exécution via 'ctest')
add_test(NAME Benchmark_ThomasAlgorithm COMMAND Test_LinearSolver)


# ==========================================
# TEST 2 : Validation PDE vs Black-Scholes
# ==========================================
add_executable(Test_PDESolver Test_PDESolver.cpp)

# Liaison avec le cœur de la librairie
target_link_libraries(Test_PDESolver PRIVATE EDP_Core)

# Drapeaux de compilation stricts
target_compile_options(Test_PDESolver PRIVATE -Wall -Wextra -Werror)

# Enregistrement du test
add_test(NAME Validation_BlackScholes COMMAND Test_PDESolver)