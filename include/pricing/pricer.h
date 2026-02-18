/*
 ____       _               
|  _ \ _ __(_) ___ ___ _ __ 
| |_) | '__| |/ __/ _ \ '__|
|  __/| |  | | (_|  __/ |   
|_|   |_|  |_|\___\___|_|  
*/
#pragma once
#include "instruments/instrument.h"
#include "types/marketstate.h"
#include "engine/montecarlo.hpp"
#include "engine/engine.hpp"
#include <unordered_map>

struct PricingResult {
    double price;
    double delta;
    double gamma;
    double vega;
    double theta;
};


class Pricer {

    public:
    Pricer(Instrument& instrument, MonteCarlo& generator);

    /**
     * @brief Performs a Monte Carlo simulation for the instrument and returns
     * the estimated price. 
     * 
     * @param market_state The market state containing the current spot price and the risk free rate
     * @param n_steps The number of steps in the simulation
     * @param n_paths The number of paths to simulate
     * @return double The estimated price
     */
    double compute_price(const MarketState& market_state, int n_steps, int n_paths);
    
    /**
     * @brief Performs a Monte Carlo simulation for the instrument and returns 
     * the estimated price and greeks
     * 
     * @param market_state The market state containing the current spot price and the risk free rate
     * @param n_steps The number of steps in the simulation
     * @param n_paths The number of paths to simulate
     * @return PricingResult A struct containing the price, the delta, the gamma, the theta and the vega of the option
     */
    PricingResult compute(const MarketState& market_state, int n_steps, int n_paths);


    /**
     * @brief Compute the delta of the option using the bump-and-revalue technique
     * 
     * @param market_state The market state containing the current spot price and the risk free rate
     * @param n_steps The number of steps in the simulation
     * @param n_paths The number of paths to simulate
     * @param h The finite difference bump 
     * @return double 
     */
    double compute_delta_bar(const MarketState& market_state, int n_steps, int n_paths, double h);

    

    private:

    MonteCarlo& generator_;
    Instrument& instrument_;



};