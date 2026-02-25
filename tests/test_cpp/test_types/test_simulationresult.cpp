


#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>  
#include <memory>
#include <optional>
#include <stdexcept>

#include <types/simulationresult.hpp>


TEST_CASE("SimulationResult - Basic usage"){

    std::vector<double> my_path{100, 101, 102, 103,
                                   100, 99, 98, 97};

    SECTION("Constructor") {
        SimulationResult res(std::make_shared<std::vector<double>>(my_path),  1, 3, 2);
    }

    SECTION("Dimension errors") {
        REQUIRE_THROWS_AS(SimulationResult(std::make_shared<std::vector<double>>(my_path),  1, 2, 2), std::invalid_argument);
    }

    SECTION("Access to data") {
        SimulationResult res( std::make_shared<std::vector<double>>(my_path),  1, 3, 2);
        const std::vector<double>& paths = res.get_paths();

    }

}


TEST_CASE("SimulationResult - Average terminal value") {

    std::vector<double> my_path{100, 101, 102, 103,
                                   100, 99, 98, 103,
                                100, 99, 101, 103};

    SimulationResult res(std::make_shared<std::vector<double>>(my_path), 1, 3, 3);

    REQUIRE(res.avg_terminal_value() == 103.0);

}


TEST_CASE("SimulationResult - Error accessing vol") {

    std::vector<double> my_path{100, 101, 102, 103,
                                   100, 99, 98, 103,
                                100, 99, 101, 103};

    SimulationResult res(std::make_shared<std::vector<double>>(my_path), 1, 3, 3);

    REQUIRE_THROWS_AS(res.get_vol(), std::invalid_argument);

}
