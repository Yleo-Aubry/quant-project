cmake_minimum_required(VERSION 3.10)
project(Projet_EDP)

set(CMAKE_CXX_STANDARD 14)

# Inclusion des headers
include_directories(include)

# Création de l'exécutable avec TOUS les fichiers .cpp
add_executable(Projet_EDP 
    src/main.cpp 
    src/PDESolver.cpp 
    src/LinearSolver.cpp 
    src/Interface.cpp    
)
