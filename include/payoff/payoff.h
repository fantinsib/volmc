#pragma once
#include <algorithm>

class Payoff {

public:
    virtual ~Payoff() = default; 
    virtual double compute(double S) const = 0;

};


class CallPayoff : public Payoff {

public:

    CallPayoff(double K) : strike_(K) {};
    double compute(double S) const override {
        return std::max((S - strike_), 0.0);
    } ;
    double get_strike() const {return strike_;}

private:

    double strike_;

};

class PutPayoff : public Payoff {
    public:

    PutPayoff(double K) : strike_(K) {};
    double compute(double S) const override {
        return std::max((strike_ - S), 0.0);
    } ;
    double get_strike() const {return strike_;}

private:

    double strike_;

};

class DigitalCallPayoff : public Payoff{
    
    public:

    DigitalCallPayoff(double K) : strike_(K) {};
    double compute(double S) const override {
        return (S > strike_) ? 1.0 : 0.0; 
    } ;
    double get_strike() const {return strike_;}

    private:

    double strike_;

};