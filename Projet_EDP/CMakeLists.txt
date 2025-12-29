cmake_minimum_required(VERSION 3.15)

# Définition du projet et de la version
project(Projet_EDP VERSION 1.0.0 LANGUAGES CXX)

# Version 
set(CMAKE_CXX_STANDARDVersion 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) # Assure la portabilité stricte

# Organisation des sorties de compilation 
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Délégation vers les sous-modules
add_subdirectory(src)   # Compile la librairie de calcul
add_subdirectory(app)   # Compile l'exécutable principal
add_subdirectory(tests) # Compile les tests