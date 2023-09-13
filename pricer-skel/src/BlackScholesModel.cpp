#include "BlackScholesModel.hpp"
#include <cmath>
#include <iostream>


//Implémentation de la méthode ComputeNewAssetValue
void BlackScholesModel::ComputeNewAssetValue(int assetIndex , PnlMat* correlationMatrix , PnlVect *g , int n , double step, PnlMat* path){
    double volatilités_assetIndex = pnl_vect_get(sigma_ , assetIndex);
    PnlVect *cholesky_assetIndex_line = pnl_vect_create(size_);
    pnl_mat_get_row(cholesky_assetIndex_line,correlationMatrix , assetIndex);
    double expoIput = (r_ - (volatilités_assetIndex * volatilités_assetIndex)/2.0 )* step  + volatilités_assetIndex * std::sqrt(step) * pnl_vect_scalar_prod( cholesky_assetIndex_line, g );
    double newAssetValue = pnl_mat_get(path ,n-1, assetIndex ) * std::exp(expoIput);
    pnl_mat_set(path , n , assetIndex , newAssetValue);
    pnl_vect_free(&cholesky_assetIndex_line);
}



// Implémentation de la fonction asset
void BlackScholesModel::asset(PnlMat* path, double T, int nbTimeSteps, PnlRng* rng) {
    //Création de la matrice de cholesky
    PnlMat* correlationMatrix = pnl_mat_create_from_scalar(size_,size_,rho_);
    for(int i = 0 ; i < size_ ; i++){
    pnl_mat_set_diag(correlationMatrix, 1.0, i);
    }
    int cholesky = pnl_mat_chol(correlationMatrix);
    //Initialisation de la matrice de retour
    double step = T / nbTimeSteps;
    pnl_mat_set_row(path , spot_ , 0);

    //Remplissage de la matrice de retour(valeurs des asset sur la trajectoire)
    for(int n = 1 ; n <= nbTimeSteps  ; n++){
        PnlVect *g = pnl_vect_create(size_);
        pnl_vect_rng_normal (g, size_, rng )  ;
        for(int assetIndex = 0 ; assetIndex < this->size_ ; assetIndex++){
            ComputeNewAssetValue( assetIndex ,  correlationMatrix , g ,  n ,  step,  path);
        }
        pnl_vect_free(&g);
    }
    pnl_mat_free(&correlationMatrix);     
}

// Implémentation de la fonction asset (version avec historique)
void BlackScholesModel::asset(PnlMat* path, double t, double T, int nbTimeSteps, PnlRng* rng, const PnlMat* past) {
    //Création de la matrice de cholesky
    PnlMat* correlationMatrix = pnl_mat_create_from_scalar(size_,size_,rho_);
    for(int i = 0 ; i < size_ ; i++){
        pnl_mat_set_diag(correlationMatrix, 1.0, i);
    }
    int cholesky = pnl_mat_chol(correlationMatrix);

    //Initialisation de la matrice de retour
    pnl_mat_set_subblock(path , past , 0 , 0  );

    //Remplissage de la matrice de retour(valeurs des asset sur la trajectoire) 
    double step = T / nbTimeSteps ;
    double nextDate = step * past->m ;
    PnlVect *g =  pnl_vect_create(size_);
    pnl_vect_rng_normal (g, size_, rng )  ;

    //Initilisation des valeurs des actifs pour t_i+1
    for (int asset_index = 0 ; asset_index < this->size_ ; asset_index++){
        ComputeNewAssetValue( asset_index ,  correlationMatrix , g ,  past->m ,  nextDate-t,  path);
    }

    //Remplissage du reste du path
    for (int n = past->m + 1; n <= nbTimeSteps ; n++){
        pnl_vect_rng_normal (g, size_, rng )  ;
        for (int asset_index = 0 ; asset_index < this->size_ ; asset_index++){
            ComputeNewAssetValue( asset_index ,  correlationMatrix , g ,  n ,  step,  path);
        }
    }
    pnl_vect_free(&g);
    pnl_mat_free(&correlationMatrix);
}

// Implémentation de la fonction shiftAsset
void BlackScholesModel::shiftAsset(PnlMat* shift_path, const PnlMat* path, int d, double h, double t, double timestep) {
    // Implémentation de la fonction shiftAsset ici
}
