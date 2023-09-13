#include <iostream>
#include <ctime>
#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"
#include "Option.hpp"
#include "Basket.hpp"



Basket::Basket(double maturity, int timeSteps, int modelSize, float strike, PnlVect* payoff_coeff) {
    T_ = maturity;
    nbTimeSteps_ = timeSteps;
    size_ = modelSize;
    strike_ = strike;
    payoff_coeff_ = payoff_coeff;
}

/**
 * Calcule la valeur du payoff de l'option basket sur la trajectoire
 *
 * @param[in] path est une matrice de taille (N+1) x d
 * contenant une trajectoire du modèle telle que créée
 * par la fonction asset.
 * @return phi(trajectoire)
*/
double Basket::payoff(const PnlMat* path) {
    // Récupérer le vecteur des prix à matuité des d actif. Ici d = path->n
    PnlVect* pnl_vect_maturity_path = pnl_vect_create(path->n); // allocation de memoire
    pnl_mat_get_row(pnl_vect_maturity_path, path, path->m - 1); // récupérer la dernière ligne
    double weighted_value = pnl_vect_scalar_prod(pnl_vect_maturity_path, payoff_coeff_); // multiplication par la composition
    double result =  weighted_value - strike_;

    pnl_vect_free(&pnl_vect_maturity_path);
    return std::max(static_cast<double>(0), result);
}