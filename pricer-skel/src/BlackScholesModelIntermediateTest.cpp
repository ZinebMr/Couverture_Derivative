#include "BlackScholesModel.hpp"
#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"
#include "pnl/pnl_random.h"
#include <iostream>
#include <ctime>

int main() {
    // Initialisez les paramètres du modèle
    int size = 20;  // Nombre d'actifs du modèle
    double r = 0.05;  // Taux d'intérêt
    double rho = 0.3;  // Paramètre de corrélation
    PnlVect* sigma = pnl_vect_create_from_list(20,0.2 ,0.4, 0.5, 0.6,0.2 ,0.4, 0.5, 0.6,0.2 ,0.4, 0.5, 0.6,0.2 ,0.4, 0.5, 0.6,0.2 ,0.4, 0.5, 0.6);  // Vecteur de volatilités
    PnlVect* spot = pnl_vect_create_from_list(20, 90.0 ,100.0, 120.0, 80.0,60.0, 70.0, 200.0, 201.0 , 90.0 ,56.2 ,90.0 ,100.0, 120.0, 80.0,60.0, 70.0, 200.0, 201.0 , 90.0 ,56.2  );  // Valeurs initiales des sous-jacents

    // Créez une instance du modèle Black-Scholes
    BlackScholesModel model;
    model.size_ = size;
    model.r_ = r;
    model.rho_ = rho;
    model.sigma_ = sigma;
    model.spot_ = spot;

    // Initialisez d'autres paramètres nécessaires
    double t = 0.4;  // Date jusqu'à laquelle on connait la trajectoire
    double T = 1.0;  // Date jusqu'à laquelle on simule la trajectoire
    int nbTimeSteps = 7;  // Nombre de pas de constatation
    PnlRng* rng = pnl_rng_create(PNL_RNG_MERSENNE);  // Créez un générateur de nombres aléatoires
    pnl_rng_sseed(rng, time(NULL));

    // Créez une matrice pour stocker la trajectoire
    PnlMat* path = pnl_mat_create_from_scalar(nbTimeSteps + 1,size,  0.0);

    // Créez la matrice past
    PnlMat* past = pnl_mat_create(3, size);

    // Remplissez la matrice past
    for (int i = 0; i < 3 ; ++i) {
        for (int j = 0; j < size; ++j) {
            double value = i * 20 + j + 1.0; 
            pnl_mat_set(past, i, j, value);
        }
    }
    // Appelez la méthode asset pour calculer la trajectoire
    model.asset(path, t, T, nbTimeSteps, rng, past);  

    // Affichez la trajectoire
    std::cout << "Trajectoire du sous-jacent : " << std::endl;
    pnl_mat_print(path);

    // Libérez la mémoire
    pnl_vect_free(&sigma);
    pnl_vect_free(&spot);
    pnl_mat_free(&path);
    pnl_rng_free(&rng);

    return 0;
}
