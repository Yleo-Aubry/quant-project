# Création de l'exécutable final "PricerApp"
add_executable(PricerApp main.cpp)

# Liaison avec la librairie métier (EDP_Core)
target_link_libraries(PricerApp PRIVATE EDP_Core)

# Options de compilation
target_compile_options(PricerApp PRIVATE -Wall -Wextra -Werror)