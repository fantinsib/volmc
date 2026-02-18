#pragma once

struct MarketState {
    public:
        MarketState(double S0, double r) : S0_(S0), r_(r) {};
        double spot() const {return S0_;};
        double rf_rate() const {return r_;};

    private:
        double S0_;
        double r_;       
};