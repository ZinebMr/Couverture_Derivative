#pragma once

#include <iostream>
#include <ctime>
#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"
#include "Option.hpp"

class Asian : public Option{
    public:
       
        float strike_; // Strike de l'option
        PnlVect* payoff_coeff_; // Vecteur de coefficients payoff

        Asian(double maturity, int timeSteps, int modelSize, float strike, PnlVect* payoff_coeff);
        
        /**
         * Calcule la valeur du payoff de l'option asiatique sur la trajectoire
         *
         * @param[in] path est une matrice de taille (N+1) x d
         * contenant une trajectoire du modèle telle que créée
         * par la fonction asset.
         * @return phi(trajectoire)
        */
        double payoff(const PnlMat* path);
};