#pragma once

#include <optional>
#include <stdexcept>
#include <cmath>
#include <vector>

/**
 * @brief Snapshot of an instant t in a Path
 * 
 * @note The State represent the generic state of an asset at time t through a vector. 
 * Convention is : position 0 is the value of the asset, position 1 is the variance, position 2 is the time t
 */
struct State {

    State(std::optional<double> a = std::nullopt, 
          std::optional<double> b=std::nullopt) : vec(3)
          {
            if (a.has_value()) at(0) = a.value();
            if (b.has_value()) {
                has_var_ = true;    
                at(1) = b.value();
            };

          };


    std::vector<double> vec;
    //at(0) : value | at(1) : variance
    double& at(size_t x) {
        if (x >= vec.size()) throw std::invalid_argument("Trying to get out of bounds object from vec");
        if (x == 1 && has_var_ == false) throw std::invalid_argument("Variance is non existent"); 
        return vec[x];
    }
    //at(0) : value | at(1) : variance
    const double& at(size_t x) const {
        if (x >= vec.size()) throw std::invalid_argument("Trying to get out of bounds object from vec");
        return vec[x];
    }

    //Sets the ith element of State equal to x
    void set(size_t i, double x) {vec[i] = x;}
    size_t dim() {return vec.size();}
    bool holds_var() const {return has_var_;}
    bool has_var_ = false;

};

struct EquityView {
    State& s;
    double& spot() {return s.at(0);}
    const double& spot() const {return s.at(0);}

    double& var() {return s.at(1);}
    const double& var() const {return s.at(1);}

    double vol() {return std::sqrt(s.at(1));}
    double vol() const {return std::sqrt(s.at(1));}
};

struct RatesState {
    State& s;
    double& rate() {return s.at(0);}
    const double& rate() const {return s.at(0);}

    double& var() {return s.at(1);}
    const double& var() const {return s.at(1);}

    double vol() {return std::sqrt(s.at(1));}
    double vol() const {return std::sqrt(s.at(1));}
};