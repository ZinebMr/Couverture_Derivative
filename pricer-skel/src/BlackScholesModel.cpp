#include "BlackScholesModel.hpp"
#include <cmath>
#include <iostream>

/**
 * @brief Calcule du prix pour un asset et stockage dans la matrice des trajectoires
 * 
 * @param assetIndex indice de l'asset : d
 * @param correlationMatrix 
 * @param g vecteurs gaussiens
 * @param n 
 * @param step 
 * @param path matrice des trajectoires à modifier
 */
void BlackScholesModel::ComputeNewAssetValue(int assetIndex , PnlMat* correlationMatrix , PnlVect* g , int n , double step, PnlMat* path){
    double volatilités_assetIndex = pnl_vect_get(sigma_, assetIndex); // Récupérer la volatilté de l'asset 'd' ici assetIndex
    
    PnlVect* cholesky_assetIndex_line = pnl_vect_create(size_); // Création du vecteur Cholesky
    pnl_mat_get_row(cholesky_assetIndex_line, correlationMatrix, assetIndex); // Copier la ligne correspondante à la correation de l'asset courant dans le vecteur Cholesky
    
    // Calcul du prix à l'instant t du prix de l'asset d
    double expoIput = (r_ - (volatilités_assetIndex * volatilités_assetIndex)/2.0 )* step  + volatilités_assetIndex * std::sqrt(step) * pnl_vect_scalar_prod(cholesky_assetIndex_line, g);
    double newAssetValue = pnl_mat_get(path, n-1, assetIndex ) * std::exp(expoIput);

    // Stocker le prix dans la matrice
    pnl_mat_set(path, n, assetIndex, newAssetValue);

    // Libération de la mémoire
    pnl_vect_free(&cholesky_assetIndex_line);
}

/**
 * @brief Genere une trajectoire du modele et la stocke dans path
 * 
 * @param path 
 * @param T 
 * @param nbTimeSteps 
 * @param rng 
 */
void BlackScholesModel::asset(PnlMat* path, double T, int nbTimeSteps, PnlRng* rng){
    // Création de la matrice de cholesky
    PnlMat* correlationMatrix = pnl_mat_create_from_scalar(size_, size_, rho_);
    for(int i = 0 ; i < size_ ; i++)
    {
        pnl_mat_set_diag(correlationMatrix, 1.0, i);
    }
    int cholesky = pnl_mat_chol(correlationMatrix);

    // Initialisation de la matrice de retour
    double step = T / nbTimeSteps;
    pnl_mat_set_row(path, spot_, 0);

    // Remplissage de la matrice de retour(valeurs des asset sur la trajectoire)
    for(int n = 1 ; n <= nbTimeSteps  ; n++){
        PnlVect *g = pnl_vect_create(size_);
        pnl_vect_rng_normal (g, size_, rng );

        for(int assetIndex = 0 ; assetIndex < this->size_ ; assetIndex++)
        {
            ComputeNewAssetValue(assetIndex, correlationMatrix, g , n, step, path);
        }
        pnl_vect_free(&g);
    }
    pnl_mat_free(&correlationMatrix);     
}

void BlackScholesModel::asset(PnlMat* path, double t, double T, int nbTimeSteps, PnlRng* rng, const PnlMat* past){
    //Création de la matrice de cholesky
    PnlMat* correlationMatrix = pnl_mat_create_from_scalar(size_, size_, rho_);
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
    for (int asset_index = 0 ; asset_index < this->size_ ; asset_index++)
    {
        ComputeNewAssetValue(asset_index, correlationMatrix, g, past->m, nextDate-t, path);
    }

    //Remplissage du reste du path
    for (int n = past->m + 1; n <= nbTimeSteps ; n++)
    {
        pnl_vect_rng_normal (g, size_, rng)  ;
        for (int asset_index = 0 ; asset_index < this->size_ ; asset_index++)
        {
            ComputeNewAssetValue(asset_index, correlationMatrix , g, n, step, path);
        }
    }
    pnl_vect_free(&g);
    pnl_mat_free(&correlationMatrix);
}

// Implémentation de la fonction shiftAsset
void BlackScholesModel::shiftAsset(PnlMat* shift_path, const PnlMat* path, int d, double h, double t, double timestep){
    pnl_mat_set_subblock(shift_path, path, 0, 0);
    int indice_ligne_debut = (int)round(t/timestep);
    for (int i = indice_ligne_debut; i < path->m; i++){
        pnl_mat_set (shift_path, i, d-1, pnl_mat_get(shift_path, i, d-1)*(1.0+h));
    }
}
