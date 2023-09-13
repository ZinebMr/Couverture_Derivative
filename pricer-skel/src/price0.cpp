#include <fstream>
#include <iostream>
#include "MonteCarlo.hpp"
#include "pnl/pnl_random.h"
#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"
#include "Asian.hpp"
#include "Basket.hpp"
#include "Performance.hpp"
#include "Option.hpp"
#include "json_helper.hpp"
#include "PricingResults.hpp"
#include <nlohmann/json.hpp>

int main(int argc, char **argv){
    if (argc != 2) {
        std::cerr << "Wrong number of arguments. Exactly one argument is required" << std::endl;
        std::exit(0);
    }

    // ===================== Lecture du fichier contenant les paramètres du modèle ============================
    std::ifstream ifs(argv[1]); // recupérer le nom du fichier
    nlohmann::json j = nlohmann::json::parse(ifs); // parser le fichier
    
    // ===================== Deserialization de l'input : Stockage de tous les paramètres =====================
    int size;
    j.at("option size").get_to(size);

    std::string option_type;
    option_type = j.at("option type").get<std::string>();
    
    float strike;
    if (option_type == "asian" || option_type == "basket")
    { 
        j.at("strike").get_to(strike); 
    }

    PnlVect* spot;
    j.at("spot").get_to(spot);
    if (spot->size == 1 && size > 1) {
        pnl_vect_resize_from_scalar(spot, size, GET(spot, 0));
    }

    double maturity;
    j.at("maturity").get_to(maturity);

    PnlVect* volatility;
    j.at("volatility").get_to(volatility);
    if (volatility->size == 1 && size > 1) {
        pnl_vect_resize_from_scalar(volatility, size, GET(volatility, 0));
    }

    double interest_rate;
    j.at("interest rate").get_to(interest_rate);

    double correlation;
    j.at("correlation").get_to(correlation);


    PnlVect* payoff_coeff;
    j.at("payoff coefficients").get_to(payoff_coeff);
    if (payoff_coeff->size == 1 && size > 1) {
        pnl_vect_resize_from_scalar(payoff_coeff, size, GET(payoff_coeff, 0));
    }

    int nb_time_steps;
    j.at("timestep number").get_to(nb_time_steps);

    int sample_number;
    j.at("sample number").get_to(sample_number);
    

    // ===================== Initialiser le modèle Black and Scholes ===============================================
    BlackScholesModel model;
    model.size_ = size;
    model.r_ = interest_rate;
    model.rho_ = correlation;
    model.sigma_ = volatility;
    model.spot_ = spot;
    PnlRng* rng = pnl_rng_create(PNL_RNG_MERSENNE);
    pnl_rng_sseed(rng, time(NULL));

    // =================== Créez une instance de MonteCarlo en utilisant les paramètres précédemment créées ========
    MonteCarlo mc;
    mc.mod_ = &model;
    mc.rng_ = rng;
    mc.fdStep_ = maturity / nb_time_steps;
    mc.nbSamples_ = sample_number;

    // ===================== Initialiser le bon type d'option =======================================================
    if (option_type == "asian"){
        Asian current_option(maturity, nb_time_steps, sample_number, strike, payoff_coeff);
        mc.opt_ = static_cast<Option*>(&current_option); // Ajouter l'option à l'instance MonteCarlo
    }
    else if (option_type == "basket")
    {
        Basket current_option(maturity, nb_time_steps, sample_number, strike, payoff_coeff);
        mc.opt_ = static_cast<Option*>(&current_option);
    }
    else if (option_type == "performance")
    {
        Performance current_option(maturity, nb_time_steps, sample_number, payoff_coeff);
        mc.opt_ = static_cast<Option*>(&current_option);
    }
    else
    {
        std::cerr << "Wrong type of option !" << std::endl;
        std::exit(0);
        return 0;
    }

    // ===================== Calculer le prix de l'option avec la méthode price de MonteCarlo =======================
    double prix, std_dev;
    mc.price(prix, std_dev);
    PnlVect* delta_intial = pnl_vect_create_from_zero(size);
    PnlVect* delta_std_dev_intial = pnl_vect_create_from_zero(size);

    // ===================== Stocker les résultats de pricing dans la structure PricingResults =======================
    PricingResults res(prix, std_dev, delta_intial, delta_std_dev_intial);
    
    // ===================== Afficher les resultats sur la console ====================================================
    std::cout << res << std::endl;
    return 0;
}
