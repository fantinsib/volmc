
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>  
#include <memory>
#include <optional>
#include <stdexcept>

#include "options/options.hpp"
#include "payoff/payoff.h"
#include "instruments/instrument.h"


TEST_CASE("European option - basic construction"){

    double K = 105;
    OptionContract contract(K, 0.8);
    
    Instrument european_call(contract, std::make_unique<CallPayoff>(K));
    
}