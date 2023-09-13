#include <iostream>
#include "MonteCarlo.hpp"  // Assurez-vous que le fichier d'en-tête est correctement inclus

int main() {
    // Créez une instance de BlackScholesModel, Option, et PnlRng
    PnlRng* rng = pnl_rng_create(PNL_RNG_MERSENNE);
    pnl_rng_sseed(rng, time(NULL));
    BlackScholesModel model;
    model.size_ = 4;
    model.r_ = 0.05;
    model.rho_ = 0.2;
    model.sigma_ = pnl_vect_create_from_list(4,0.2 ,0.2, 0.2, 0.2);
    model.spot_ = pnl_vect_create_from_list(4, 90.0 ,100.0, 120.0, 80.0);
    // Créez une instance de MonteCarlo en utilisant les instances précédemment créées
    MonteCarlo mc;
    mc.mod_ = &model;
    mc.opt_ = &option;
    mc.rng_ = &rng;
    mc.fdStep_ = 0.01;  // Réglez le pas de différence finie
    mc.nbSamples_ = 10000;  // Réglez le nombre de tirages Monte Carlo

    // Créer un instance de options 
    Option option;
    double T = 1.0;  // Date jusqu'à laquelle on simule la trajectoire
    int nbTimeSteps = 6;  // Nombre de pas de constatation

    double t = 0.4;  // Date jusqu'à laquelle on connait la trajectoire
    // Créez la matrice past
    PnlMat* past = pnl_mat_create(3, size);

    // Remplissez la matrice past
    for (int i = 0; i < 3 ; ++i) {
        for (int j = 0; j < size; ++j) {
            double value = i * 20 + j + 1.0; 
            pnl_mat_set(past, i, j, value);
        }
    }
    // Appelez la fonction price pour calculer le prix de l'option et l'écart type
    double prix, std_dev;
    mc.price(past , t , prix, std_dev);

    // Affichez les résultats
    std::cout << "Prix de l'option: " << prix << std::endl;
    std::cout << "Écart type de l'estimateur: " << std_dev << std::endl;

    return 0;
}
