# Définition des sources de la librairie
set(SOURCES
    Interface.cpp
    LinearSolver.cpp
    PDESolver.cpp
    
)

# Création de la librairie 
add_library(EDP_Core STATIC ${SOURCES})

# Configuration des dossiers d'inclusion
target_include_directories(EDP_Core PUBLIC 
    ${CMAKE_SOURCE_DIR}/include
)

# Options de compilation 
target_compile_options(EDP_Core PRIVATE
    -Wall -Wextra -Wpedantic -Werror
)