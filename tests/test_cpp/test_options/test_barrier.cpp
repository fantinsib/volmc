#include "engine/montecarlo.hpp"
#include "instruments/instrument.h"
#include "models/black_scholes/black_scholes.hpp"
#include "options/options.hpp"
#include "payoff/payoff.h"
#include "schemes/euler.h"
#include "types/simulationresult.hpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>  
#include <memory>
#include <optional>
#include <stdexcept>
#include <cmath>


TEST_CASE("Barrier : basic usage"){

    double K = 105;
    double T = 1.1;
    OptionContract contract(K,T);
    CallPayoff call_payoff; 
    Barrier barrier(102, Up, In, call_payoff);
    Instrument up_and_in_call(contract, std::make_unique<Barrier>(barrier));
}

TEST_CASE("Barrier : payoff"){

    SECTION("Positive payoff - Up and in call"){

        //Strike 103, barrier 102 and initial spot at 101 with a constant growth rate of 0.02 -> barrier is met and call kicks in
        double K = 103;
        double T = 1.1;
        float r = .02;
        double S0 = 101;
        OptionContract contract(K,T);
        CallPayoff call_payoff; 
        Barrier barrier(102, Up, In, call_payoff);
        Instrument up_and_in_call(contract, std::make_unique<Barrier>(barrier));

        BlackScholes bs{r, 0.0000001};
        Euler euler(std::make_shared<BlackScholes>(bs));
        MonteCarlo mc(euler);
        mc.configure(1, -1);

        SimulationResult sim = mc.generate(S0, 100, T, 10);

        double payoff = up_and_in_call.compute_payoff(sim);
        REQUIRE(sim.avg_terminal_value() == Catch::Approx(S0 * std::exp(r * T)).margin(0.01));
        REQUIRE(payoff > 0);
    }
    SECTION("No payoff - Up and out call"){
        double K = 103;
        double T = 1.1;
        float r = 0.02;
        double S0 = 101;
        OptionContract contract(K,T);
        CallPayoff call_payoff; 
        Barrier barrier(102, Up, Out, call_payoff);
        Instrument up_and_in_call(contract, std::make_unique<Barrier>(barrier));

        BlackScholes bs{r, 0}; // 0 vol to ensure constant growth 
        Euler euler(std::make_shared<BlackScholes>(bs));
        MonteCarlo mc(euler);
        mc.configure(1, -1);

        SimulationResult sim = mc.generate(S0, 100, T, 10);

        double payoff = up_and_in_call.compute_payoff(sim);
        REQUIRE(sim.avg_terminal_value() == Catch::Approx(S0 * std::exp(r * T)).margin(0.01));
        REQUIRE(payoff == 0.0);
    }
    SECTION("Positive payoff - Down and in put"){
        //Strike 98, barrier 99 and initial spot 100 with constant negative growth at -0.05 -> put kicks in 
        double K = 98;
        double T = 1.1;
        float r = -0.05;
        OptionContract contract(K,T);
        PutPayoff put_payoff; 
        Barrier barrier(99, Down, In, put_payoff);
        Instrument down_and_in_put(contract, std::make_unique<Barrier>(barrier));

        BlackScholes bs{r, 0}; // 0 vol to ensure constant growth 
        Euler euler(std::make_shared<BlackScholes>(bs));
        MonteCarlo mc(euler);
        mc.configure(1, -1);

        SimulationResult sim = mc.generate(100, 100, T, 10);

        double payoff = down_and_in_put.compute_payoff(sim);
        REQUIRE(sim.avg_terminal_value() == Catch::Approx(100 * std::exp(r * T)).margin(0.01));
        REQUIRE(payoff > 0.0);
    }

    SECTION("No payoff - Down and out put"){
        //Strike 98, barrier 99 and initial spot 100 with constant negative growth at -0.05 -> put kicks out
        double K = 98;
        double T = 1.1;
        float r = -0.05;
        OptionContract contract(K,T);
        PutPayoff put_payoff; 
        Barrier barrier(99, Down, Out, put_payoff);
        Instrument down_and_out_put(contract, std::make_unique<Barrier>(barrier));

        BlackScholes bs{r, 0}; // 0 vol to ensure constant growth 
        Euler euler(std::make_shared<BlackScholes>(bs));
        MonteCarlo mc(euler);
        mc.configure(1, -1);

        SimulationResult sim = mc.generate(100, 100, T, 10);

        double payoff = down_and_out_put.compute_payoff(sim);
        REQUIRE(sim.avg_terminal_value() == Catch::Approx(100 * std::exp(r * T)).margin(0.01));
        REQUIRE(payoff == 0.0);
    }

    SECTION("Barrier met but no payoff - Up and in call"){
        //Strike at 110, barrier at 101 and S0 100 with const growth 0.05 -> call is activated but spot does not reach K 
        double K = 110;
        double S0 = 100;
        double H = 101;
        double T = 1.1;
        float r = 0.05;
        OptionContract contract(K,T);
        PutPayoff call_payoff; 
        Barrier barrier(100, Down, Out, call_payoff);
        Instrument down_and_out_put(contract, std::make_unique<Barrier>(barrier));

        BlackScholes bs{r, 0}; // 0 vol to ensure constant growth 
        Euler euler(std::make_shared<BlackScholes>(bs));
        MonteCarlo mc(euler);
        mc.configure(1, -1);

        SimulationResult sim = mc.generate(S0, 100, T, 10);

        double payoff = down_and_out_put.compute_payoff(sim);
        REQUIRE(sim.avg_terminal_value() == Catch::Approx(S0 * std::exp(r * T)).margin(0.01));
        REQUIRE(payoff == 0.0);
    }
}

TEST_CASE("Barrier : payoff dependency on H") {
    double K = 120;
    double S0 = 100;
    double H1 = 105;
    double H2 = 140;
    double T = 1;
    float r = 0.05;
    float sigma = 0.2;

    OptionContract contract(K,T);
    CallPayoff call_payoff; 

    Barrier barrier1(H1, Up, Out, call_payoff);
    Barrier barrier2(H2, Up, Out, call_payoff);

    Instrument up_out_call1(contract, std::make_unique<Barrier>(barrier1));
    Instrument up_out_call2(contract, std::make_unique<Barrier>(barrier2));
    
    BlackScholes bs{r, sigma}; // 0 vol to ensure constant growth 
    Euler euler(std::make_shared<BlackScholes>(bs));
    MonteCarlo mc(euler);
    mc.configure(1, -1);


    SimulationResult sim = mc.generate(S0, 100, T, 10);
    double payoff1 = up_out_call1.compute_payoff(sim);
    double payoff2 = up_out_call2.compute_payoff(sim);

    REQUIRE(payoff1 != payoff2);
}