#include <iostream>
#include <ctime>
#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"
#include "Option.hpp"
#include "Performance.hpp"


Performance::Performance(double maturity, int timeSteps, int modelSize, PnlVect* payoff_coeff) {
    T_ = maturity;
    nbTimeSteps_ = timeSteps;
    size_ = modelSize;
    payoff_coeff_ = payoff_coeff;
}

/**
 * Calcule la valeur du payoff de l'option Performance sur la trajectoire
 *
 * @param[in] path est une matrice de taille (N+1) x d
 * contenant une trajectoire du modèle telle que créée
 * par la fonction asset.
 * @return phi(trajectoire)
*/
double Performance::payoff(const PnlMat* path) {

    // Produit des trajectoires avec les coeff de payoff
    PnlVect* pnl_vect_weighted_values= pnl_mat_mult_vect(path ,payoff_coeff_);

    // Selectioner les vecteurs numerateur et denominateur
    PnlVect* pnl_vect_weighted_values_num = pnl_vect_create(pnl_vect_weighted_values->size - 1); //allocation de memoire
    PnlVect* pnl_vect_weighted_values_denom = pnl_vect_create(pnl_vect_weighted_values->size - 1); 

    pnl_vect_extract_subvect(pnl_vect_weighted_values_num , pnl_vect_weighted_values, 1, pnl_vect_weighted_values->size - 1); //on elimine la 1ere ligne
    pnl_vect_extract_subvect(pnl_vect_weighted_values_denom , pnl_vect_weighted_values, 0, pnl_vect_weighted_values->size - 1); //on elimine la derniere ligne

    // Division du numérateur par le dénominateur
    pnl_vect_div_vect_term(pnl_vect_weighted_values_num, pnl_vect_weighted_values_denom); 
    pnl_vect_minus_scalar(pnl_vect_weighted_values_num, 1); // retrancher le 1

    // Sommer sur les différents instants i = 1 à N
    double result = 1;
    for (int i = 1 ; i<pnl_vect_weighted_values_num->size ; i++) 
    {
        double element_i = std::max(GET(pnl_vect_weighted_values_num, i), static_cast<double>(0)); // prendre le max entre 0 et resultat
        result += element_i;
    }

    return result;
}



