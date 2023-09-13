#include "MonteCarlo.hpp"
#include <iostream>
#include <cmath>

void MonteCarlo::price(double& prix, double& std_dev) {
    double exp_r = std::exp(-1 * mod_->r_ * opt_->T_) ;
    prix = 0.0;
    double prix_carre = 0.0;
    for (int i = 1 ; i <= nbSamples_ ; i++){
        PnlMat* path = pnl_mat_create(opt_->nbTimeSteps_ + 1, mod_->size_);
        mod_->asset(path, opt_->T_ , opt_->nbTimeSteps_ , this->rng_ );
        double payOff = opt_->payoff(path);
        prix += payOff;
        prix_carre += payOff * payOff ;
        pnl_mat_free(&path);
    }
    prix =  exp_r * prix / nbSamples_;
    std_dev = std::sqrt ((exp_r * exp_r * (1.0 / nbSamples_ ) * prix_carre) - prix * prix)  / std::sqrt(nbSamples_) ;
}

void MonteCarlo::price(const PnlMat* past, double t, double& prix, double& std_dev) {
    double exp_r = std::exp(-1 * mod_->r_ * (opt_->T_ - t)) ;
    prix = 0.0;
    double prix_carre = 0.0;
    for (int i = 1 ; i <= nbSamples_ ; i++ ){
        PnlMat* path = pnl_mat_create(opt_->nbTimeSteps_ + 1 , mod_->size_);
        mod_->asset(path, t, opt_->T_, opt_->nbTimeSteps_ , this->rng_ , past);
        double payOff = opt_->payoff(path);
        prix += payOff;
        prix_carre += payOff * payOff ;
        pnl_mat_free(&path);
    }
    prix =  exp_r * prix / nbSamples_ ;
    std_dev =  std::sqrt ((exp_r * exp_r * (1.0 / nbSamples_ ) * prix_carre) - prix * prix ) / std::sqrt(nbSamples_);


}

void MonteCarlo::delta(const PnlMat* past, double t, PnlVect* delta, PnlVect* std_dev) {
    // Implémentation de la fonction delta (version à la date t)
    // Utilisez le modèle Black-Scholes (mod_), l'option (opt_), la trajectoire jusqu'à la date t (past) et le générateur (rng_) pour calculer le delta ici
    // Mettez le résultat dans delta et std_dev
}

void MonteCarlo::delta(PnlVect* delta, PnlVect* std_dev) {
    // Implémentation de la fonction delta (version à la date 0)
    // Utilisez le modèle Black-Scholes (mod_), l'option (opt_) et le générateur (rng_) pour calculer le delta ici
    // Mettez le résultat dans delta et std_dev
}
