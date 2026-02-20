#pragma once
#include "types/path.hpp"
#include <algorithm>
#include <memory>
#include <stdexcept>

/**
 * @brief Base class for payoffs
 * 
 */
class Payoff {

public:
    virtual ~Payoff() = default;
    /**
     * @brief Computes the payoff value
     * 
     * @param path the path of the underlying asset
     * @param K the strike price 
     * @return double 
     */
    virtual double compute(Path& path, double K) const = 0;

    virtual std::shared_ptr<Payoff> clone () const = 0;
    
};


class CallPayoff : public Payoff {

public:

    //Constructor
    CallPayoff()  {
    };

    /**
     * @brief Compute the value of the payoff 
     * for a call option (max(S-K), 0).
     * 
     * @param path the path of the underlying asset
     * @param K the strike
     * @return double 
     */
    double compute(Path& path, double K) const override {

        if (K<0) throw std::invalid_argument("Strike value cannot be negative");
        double S = path.end_state().at(0);
        return std::max((S - K), 0.0);
    } ;
    std::shared_ptr<Payoff> clone() const override {
        return std::make_shared<CallPayoff>(*this);
    }

};

class PutPayoff : public Payoff {
    public:

    PutPayoff() {};

    /**
     * @brief Compute the value of the payoff 
     * for a put option (max(K-S), 0).
     * 
     * @param path the path of the underlying asset
     * @param K the strike
     * @return double 
     */
    double compute(Path& path, double K) const override {
        if (K<0) throw std::invalid_argument("Strike value cannot be negative");
        double S = path.end_state().at(0);
        return std::max((K - S), 0.0);
    } ;

    std::shared_ptr<Payoff> clone() const override {
        return std::make_shared<PutPayoff>(*this);
    }


};

class DigitalCallPayoff : public Payoff{
    
    public:

    DigitalCallPayoff() {};
    
    /**
     * @brief Compute the value of the payoff 
     * for a digital call option (1 if S > K, 0 else).
     * 
     * @param path the path of the underlying asset
     * @param K the strike
     * @return double 
     */
    double compute(Path& path, double K) const override {
        if (K<0) throw std::invalid_argument("Strike value cannot be negative");
        double S = path.end_state().at(0);
        return (S > K) ? 1.0 : 0.0; 
    } ;

    std::shared_ptr<Payoff> clone() const override {
        return std::make_shared<DigitalCallPayoff>(*this);
    }


};

class DigitalPutPayoff : public Payoff{
    
    public:

    DigitalPutPayoff() {};
    /**
     * @brief Compute the value of the payoff 
     * for a digital put option (1 if S < K, 0 else).
     * 
     * @param path the path of the underlying asset
     * @param K the strike
     * @return double 
     */
    double compute(Path& path, double K) const override {
        if (K<0) throw std::invalid_argument("Strike value cannot be negative");
        double S = path.end_state().at(0);
        return (S < K) ? 1.0 : 0.0; 
    } ;

    std::shared_ptr<Payoff> clone() const override {
        return std::make_shared<DigitalPutPayoff>(*this);
    }


};

enum Direction {Up, Down};
enum Nature {In, Out};

class Barrier : public Payoff {
    public:
        Barrier(double H, Direction direction, Nature nature , Payoff& payoff) : 
        barr_(H),
        dir_(direction),
        nat_(nature), 
        payoff_(payoff.clone()){};
            
        double compute(Path& path, double K) const override{
            bool touched = touched_(path);
            if (nat_ == In && touched) {return payoff_->compute(path, K);}
            else if (nat_ == Out && !touched) {return payoff_->compute(path, K);}
            else return 0.0;          
                };

    private:
        double barr_;
        Direction dir_; 
        Nature nat_;
        std::shared_ptr<Payoff> payoff_;
        
        bool touched_(const Path& path) const {
            
            if (dir_ == Up){
                for (const State& p : path) {
                    if (p.at(0) >= barr_) return true;
                };}

            if (dir_ == Down){
                for (const State& p : path) {
                    if (p.at(0) <= barr_) return true;
                };
            }
            return false;
        };
};