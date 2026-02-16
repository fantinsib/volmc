#pragma once
#include <algorithm>
#include <stdexcept>

class Payoff {

public:
    virtual ~Payoff() = default; 
    virtual double compute(double S) const = 0;

};


class CallPayoff : public Payoff {

public:

    //Constructor
    CallPayoff(double K) : strike_(K) {
        if (strike_<0) throw std::invalid_argument("Strike value cannot be negative");
    };

    /**
     * @brief Compute the value of the payoff 
     * for a call option (max(S-K), 0).
     * 
     * @param S 
     * @return double 
     */
    double compute(double S) const override {
        if (S<0) throw std::invalid_argument("z value cannot be negative");
        return std::max((S - strike_), 0.0);
    } ;
    double get_strike() const {return strike_;}

private:

    double strike_;

};

class PutPayoff : public Payoff {
    public:

    PutPayoff(double K) : strike_(K) {
        if (strike_<0) throw std::invalid_argument("Strike value cannot be negative");
    };

    /**
     * @brief Compute the value of the payoff 
     * for a put option (max(K-S), 0).
     * 
     * @param S 
     * @return double 
     */
    double compute(double S) const override {
        return std::max((strike_ - S), 0.0);
    } ;
    double get_strike() const {return strike_;}

private:

    double strike_;

};

class DigitalCallPayoff : public Payoff{
    
    public:

    DigitalCallPayoff(double K) : strike_(K) {
        if (strike_<0) throw std::invalid_argument("Strike value cannot be negative");
    };
    double compute(double S) const override {
        return (S > strike_) ? 1.0 : 0.0; 
    } ;
    double get_strike() const {return strike_;}

    private:

    double strike_;

};

class DigitalPutPayoff : public Payoff{
    
    public:

    DigitalPutPayoff(double K) : strike_(K) {
        if (strike_<0) throw std::invalid_argument("Strike value cannot be negative");
    };
    double compute(double S) const override {
        return (S < strike_) ? 1.0 : 0.0; 
    } ;
    double get_strike() const {return strike_;}

    private:

    double strike_;

};

enum Dir {Up, Down};
enum Act {In, Out};

class Barrier : public Payoff {
    public:
        Barrier(double K, double H, Dir, Act) : strike_(K), barr_(H);
        double compute(double S) {

        };

    private:
        double strike_;
        double barr_;
        bool touched_(Path path) {
            for (double p : path.spot()) {
                if (p >= barr_) return True;
            };
        };
}