#pragma once


#include "options/options.hpp"
#include "payoff/payoff.h"
#include "types/path.hpp"
#include "types/simulationresult.hpp"
#include <memory>


struct Instrument {

    /**
     * @brief Generic class for a financial instrument
     * 
     * @param contract an OptionContract 
     * @param payoff the payoff of the instrument
     */
    Instrument(OptionContract& contract, std::unique_ptr<Payoff> payoff) : 
        contract_(contract),
        payoff_(std::move(payoff))
    {};

    /**
     * @brief Returns the payoff based on a Path
     * 
     * @param path A path followed by the underlying asset 
     * @return double 
     */
    double compute_payoff(const Path& path) const;

    /**
     * @brief Returns the payoff based on a simulation result
     * 
     * @param simulation A SimulationResult instance
     * @return double 
     * @note This method returns the average of the payoffs of 
     * all the Path of the Simulation 
     */
    double compute_payoff(const SimulationResult& simulation) const;

    private:
        OptionContract& contract_;
        std::unique_ptr<Payoff> payoff_; 

};