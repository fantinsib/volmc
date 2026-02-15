#pragma once

struct OptionContract {

    /**
     * @brief Struct representing the Option term sheet
     * 
     * @param K The strike price
     * @param T The time to maturity in years
     */
    OptionContract(double K, double T);
    double K;
    double T;

};