#pragma once

#include <optional>
#include <stdexcept>

struct MarketState {
    public:
        MarketState(double S0, double r, std::optional<double> v0 = std::nullopt) : S0_(S0), r_(r), v0_(v0) {};
        double spot() const {return S0_;};
        double rf_rate() const {return r_;};
        bool has_vol() const {return v0_.has_value();};
        std::optional<double> vol() const {
            return v0_;
        }

    private:
        double S0_;
        double r_;
        std::optional<double> v0_; 
};