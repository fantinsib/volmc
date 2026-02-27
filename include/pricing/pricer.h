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
#include <memory>
#include <optional>
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
    /**
     * @brief Creates a pricing engine
     * 
     * @param marketstate the market state at the time of pricing
     * @param n_steps the number of steps of the pricing
     * @param n_paths the number of paths required for pricing
     * @param generator a Monte Carlo simulator
     */
    Pricer(const MarketState& marketstate, 
           size_t n_steps,
           size_t n_paths,
           std::shared_ptr<MonteCarlo> generator);

    /**
     * @brief Reconfigure the Pricer
     * 
     * @param n_steps sets a new number of steps for pricing
     * @param n_paths sets a new number of paths for pricing
     * @param marketstate sets a new marketstate 
     */
    void reconfigure(std::optional<size_t> n_steps, std::optional<size_t> n_paths, std::optional<MarketState> marketstate);

    /**
     * @brief Performs a Monte Carlo simulation for the instrument and returns
     * the estimated price. 
     * 
     * @param instrument a shared ptr to the instrument to price
     * @return double The estimated price
     */
    double compute_price(std::shared_ptr<Instrument> instrument) const;
    
    /**
     * @brief Performs a Monte Carlo simulation for the instrument and returns 
     * the estimated price and greeks
     * 
     * @param instrument a shared ptr to the instrument to price
     * @param h The finite difference bump 
     * @return PricingResult A struct containing the price, the delta, the gamma, the theta and the vega of the option
     */
    PricingResult compute(std::shared_ptr<Instrument> instrument, std::optional<double> h) const;


    /**
     * @brief Compute the delta of the option using the bump-and-revalue technique
     * 
     * @param market_state The market state containing the current spot price and the risk free rate
     * @param n_steps The number of steps in the simulation
     * @param n_paths The number of paths to simulate
     * @param h The finite difference bump 
     * @return double 
     */
    double compute_delta_bar(std::shared_ptr<Instrument> instrument, double h) const;

    /**
     * @brief Compute the gamma of the option using the bump-and-revalue technique
     * 
     * @param market_state The market state containing the current spot price and the risk free rate
     * @param n_steps The number of steps in the simulation
     * @param n_paths The number of paths to simulate
     * @param h The finite difference bump 
     * @return double 
     */
    double compute_gamma_bar(std::shared_ptr<Instrument> instrument, double h) const;

    /**
     * @brief Compute the price of a series of instrument from a same simulation
     * 
     * @param instruments a std::vector<shared_ptr> of Instrument
     * @return std::vector<double> 
     */
    std::vector<double> batch_price(std::vector<std::shared_ptr<Instrument>> instruments) const;
    

    private:
    double S0_;
    double r_;
    size_t n_steps_;
    size_t n_paths_;
    std::optional<double> v0_;


    std::shared_ptr<MonteCarlo> generator_;
    
};