#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>  
#include <optional>
#include <stdexcept>

#include "surface/local_vol.hpp"


TEST_CASE("Local Volatility Surface - Basic constructor") {

    std::vector<double> t {1,2,3, 4};
    std::vector<double> s {100,200,300};
    std::vector<double> vol{0.20, 0.19, 0.21,   
                            0.22, 0.20, 0.23,   
                            0.25, 0.24, 0.26,
                            0.27, 0.28, 0.29};

    LocalVolatilitySurface surface = LocalVolatilitySurface(t,s,vol);

}


TEST_CASE("Local Volatility Surface - Constructor Errors") {

    SECTION("Inequal inputs") {
        std::vector<double> t {1,2, 3};
        std::vector<double> s {100,200,300};
        std::vector<double> vol {0.1,0.2,0.3};

        REQUIRE_THROWS_AS(LocalVolatilitySurface(t,s,vol), std::invalid_argument);
    };

    SECTION("Insufficient vector size"){
        std::vector<double> t {1};
        std::vector<double> s {100};
        std::vector<double> vol {0.1};

        REQUIRE_THROWS_AS(LocalVolatilitySurface(t,s,vol), std::invalid_argument);
    };
}

TEST_CASE("Local Volatility Surface - Interpolation") {

    std::vector<double> spot{90, 100, 110};
    std::vector<double> time{0.2, 0.5, 0.8};
    std::vector<double> vol{0.20, 0.19, 0.21,   
                            0.22, 0.20, 0.23,   
                            0.25, 0.24, 0.26};

    LocalVolatilitySurface surface(time, spot, vol);
    
    SECTION("Defined point"){
        double sigma = surface.sigma(0.2, 100);

        REQUIRE(sigma == 0.19);
    }

    SECTION("Interpolation along S") {
        double sigma = surface.sigma(0.2, 105);

        REQUIRE(sigma == Catch::Approx(0.2));

    }

    SECTION("Interpolation along t"){

        double sigma = surface.sigma(0.35, 90);

        REQUIRE(sigma == Catch::Approx(0.21));   
    }

    SECTION("Out of bounds - S"){

        double sigma = surface.sigma(0.8, 120);
        REQUIRE(sigma == Catch::Approx(0.26));
    }
        
    SECTION("Out of bounds - t"){

        double sigma = surface.sigma(1.1, 110);
        REQUIRE(sigma == Catch::Approx(0.26));
    }

}
