#pragma once


#include "options/options.hpp"
#include "payoff/payoff.h"
#include "types/path.hpp"
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

    double compute_payoff(Path& path) const;

    private:
        OptionContract& contract_;
        std::unique_ptr<Payoff> payoff_; 

};