
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>  
#include <memory>
#include <optional>
#include <stdexcept>

#include "options/europeanoptions.hpp"
#include "options/optioncontract.hpp"



TEST_CASE("European option - basic construction"){

    auto call = std::make_shared<EuropeanCall>();
    OptionContract my_option(105, 1, call);
}