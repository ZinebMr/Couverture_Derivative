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
    prix =  exp_r * (1.0 / nbSamples_ ) * prix;
    std_dev = std::sqrt ((exp_r * exp_r * (1.0 / nbSamples_ ) * prix_carre) - prix * prix) ;
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
    prix =  exp_r * (1.0 / nbSamples_ ) * prix;
    std_dev =  std::sqrt ((exp_r * exp_r * (1.0 / nbSamples_ ) * prix_carre) - prix * prix) ;


}

void MonteCarlo::delta(const PnlMat* past, double t, PnlVect* delta, PnlVect* std_dev) {
    // Implémentation de la fonction delta (version à la date t)
    // Utilisez le modèle Black-Scholes (mod_), l'option (opt_), la trajectoire jusqu'à la date t (past) et le générateur (rng_) pour calculer le delta ici
    // Mettez le résultat dans delta et std_dev
}

void MonteCarlo::delta(PnlVect* delta, PnlVect* std_dev) {
    // Implémentation de la fonction delta (version à la date t)
    // Utilisez le modèle Black-Scholes (mod_), l'option (opt_), la trajectoire jusqu'à la date t (past) et le générateur (rng_) pour calculer le delta ici
    // Mettez le résultat dans delta et std_dev
}


void MonteCarlo::price_delta(double& prix, double& std_dev , PnlVect* delta, PnlVect* std_dev_delta){

    //Calcul du prix et de son écart type
    
    double exp_r = std::exp(-1 * mod_->r_ * opt_->T_) ;
    prix = 0.0;
    double prix_carre = 0.0;
    double diff_payoff_carre = 0.0;
    pnl_vect_set_zero(delta );
    pnl_vect_set_zero(std_dev_delta );
    for (int i = 1 ; i <= nbSamples_ ; i++){
        PnlMat* path = pnl_mat_create(opt_->nbTimeSteps_ + 1, mod_->size_);
        mod_->asset(path, opt_->T_ , opprice_deltat_->nbTimeSteps_ , this->rng_ );
        double payOff = opt_->payoff(path);
        prix += payOff;
        prix_carre += payOff * payOff ;

        //Calcul du delta et de son écart-type
        for(int d = 1 ; d <= mod_->size_ ; d++){
            PnlMat* shift_path = pnl_mat_create(opt_->nbTimeSteps_ + 1, mod_->size_);
            mod_->shiftAsset(shift_path, path, d, fdStep_ , 0 , (opt_->T_ / opt_->nbTimeSteps_));


            double payoff_1 = opt_->payoff(shift_path);
            mod_->shiftAsset(shift_path,path , d , -1.0 * fdStep_ , 0 , opt_->T_ / opt_->nbTimeSteps_   );
            double payoff_2 = opt_->payoff(shift_path);
            double valDelta = pnl_vect_get (delta  , d-1 ) + (payoff_1 - payoff_2 ) * (exp_r /(2 * nbSamples_ * fdStep_ * pnl_vect_get(mod_->spot_ , d-1))); 
            pnl_vect_set (delta  , d-1 , valDelta);

            double valDeltaCompo = pnl_vect_get (std_dev_delta  , d-1 ) + (std::pow (payoff_1 - payoff_2 , 2.0)) * nbSamples_ *std::pow ((exp_r /2 *  nbSamples_ * fdStep_ * pnl_vect_get(mod_->spot_ , d-1)),2.0);
            pnl_vect_set (std_dev_delta  , d-1 , valDeltaCompo);
            if(i == nbSamples_){
                double valDeltaCompoFinal = (pnl_vect_get (std_dev_delta  , d-1 ) - std::pow (pnl_vect_get(delta ,d-1 ) , 2.0)) / std::sqrt(nbSamples_);
                pnl_vect_set(std_dev_delta  , d-1 , valDeltaCompoFinal);
            }
            pnl_mat_free(&shift_path);
        }
        pnl_mat_free(&path); 
    }
    prix =  exp_r * (1.0 / nbSamples_ ) * prix;
    std_dev = std::sqrt ((exp_r * exp_r * (1.0 / nbSamples_ ) * prix_carre) - prix * prix)/std::sqrt(nbSamples_) ;
}