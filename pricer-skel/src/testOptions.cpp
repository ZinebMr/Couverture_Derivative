#include <iostream>
#include <ctime>
#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"
#include "Option.hpp"
#include "Performance.hpp"
#include "Basket.hpp"
#include "Asian.hpp"


int main() {
    // Seed the random number generator
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // Create random data for testing
    int numUnderlyings = 3; // Number of underlyings in your model
    int numTimeSteps = 5; // Number of time steps


    const PnlMat* randomPath = pnl_mat_create_from_list(5, 3, 90.0, 105.0, 110.0, 95.0, 100.0, 75.0, 85.0, 90.0, 90.0, 105.0, 95.0, 110.0, 102.0, 90.0, 130.0);

    // Create option objects for testing
    double maturity = 1.0; // Example maturity
    float strike = 100.0; // Example strike
    PnlVect* payoffCoeff = pnl_vect_create_from_list(numUnderlyings, 1.0, 2.0, 3.0); // Example payoff coefficients

    Basket basketOption(maturity, numTimeSteps, numUnderlyings, strike, payoffCoeff);
    Asian asianOption(maturity, numTimeSteps, numUnderlyings, strike, payoffCoeff);
    Performance performanceOption(maturity, numTimeSteps, numUnderlyings, payoffCoeff);

    // Calculate and display payoffs
    double basketPayoff = basketOption.payoff(randomPath);
    double asianPayoff = asianOption.payoff(randomPath);
    double performancePayoff = performanceOption.payoff(randomPath);

    std::cout << "Basket Option Payoff: " << basketPayoff << std::endl;
    std::cout << "Asian Option Payoff: \n" << asianPayoff << std::endl;
    std::cout << "Performance Option Payoff: " << performancePayoff << std::endl;

    // Clean up
/*      pnl_mat_free(&randomPath); */
    //pnl_vect_free(&payoffCoeff);

    return 0;
}