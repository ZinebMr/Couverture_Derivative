#include "BlackScholesModel.hpp"
#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"
#include "pnl/pnl_random.h"
#include <iostream>
#include <ctime>


int main() {
    // Créez un objet BlackScholesModel
    BlackScholesModel model;

    // Initialisez les attributs du modèle (size_, r_, rho_, sigma_, spot_)
    model.size_ = 20;
    model.r_ = 0.05;
    model.rho_ = 0.2;
    model.sigma_ = pnl_vect_create_from_list(20,0.2 ,0.4, 0.5, 0.6,0.2 ,0.4, 0.5, 0.6,0.2 ,0.4, 0.5, 0.6,0.2 ,0.4, 0.5, 0.6,0.2 ,0.4, 0.5, 0.6);
    model.spot_ = pnl_vect_create_from_list(20, 90.0 ,100.0, 120.0, 80.0,60.0, 70.0, 200.0, 201.0 , 90.0 ,56.2 ,90.0 ,100.0, 120.0, 80.0,60.0, 70.0, 200.0, 201.0 , 90.0 ,56.2  );

    // Créez un générateur de nombres aléatoires PNL
    PnlRng* rng = pnl_rng_create(PNL_RNG_MERSENNE);
    pnl_rng_sseed(rng, time(NULL));

    // Définissez les paramètres pour le calcul de la trajectoire
    double T = 1.0;             // Maturité
    int nbTimeSteps = 10;      // Nombre de dates de constatation
    PnlMat* path = pnl_mat_create_from_zero(nbTimeSteps + 1, model.size_);

    // Appelez la méthode asset pour générer une trajectoire
    model.asset(path, T, nbTimeSteps, rng);

    // Affichez la trajectoire générée (à des fins de vérification)
    std::cout << "Trajectoire générée : " << std::endl;
    pnl_mat_print(path);

    // Libérez la mémoire
    pnl_mat_free(&path);
    pnl_rng_free(&rng);

    return 0;
}


