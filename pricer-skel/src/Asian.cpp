#include <iostream>
#include <ctime>
#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"
#include "Option.hpp"
#include "Asian.hpp"

// Constructeur de l'option asiatiques
Asian::Asian(double maturity, int timeSteps, int modelSize, float strike, PnlVect* payoff_coeff) {
    T_ = maturity;
    nbTimeSteps_ = timeSteps;
    size_ = modelSize;
    strike_ = strike;
    payoff_coeff_ = payoff_coeff;
}

/**
 * Calcule la valeur du payoff de l'option asiatique sur la trajectoire
 *
 * @param[in] path est une matrice de taille (N+1) x d
 * contenant une trajectoire du modèle telle que créée
 * par la fonction asset.
 * @return phi(trajectoire)
*/
double Asian::payoff(const PnlMat* path) {

    PnlVect* pnl_vect_sum_trajectory = pnl_vect_create(path->n);
    pnl_mat_sum_vect(pnl_vect_sum_trajectory, path, 'r'); //vecteur contenant la somme des trajectoires pour chaque underlying
    double product = pnl_vect_scalar_prod(payoff_coeff_, pnl_vect_sum_trajectory);
    double result = product/ (nbTimeSteps_ + 1) - static_cast<double>(strike_);
    
    // Free
    pnl_vect_free(&pnl_vect_sum_trajectory);
    return std::max(result, static_cast<double>(0));
}

