/*
__     __        _          _    
\ \   / /_ _ ___(_) ___ ___| | __
 \ \ / / _` / __| |/ __/ _ \ |/ /
  \ V / (_| \__ \ | (_|  __/   < 
   \_/ \__,_|___/_|\___\___|_|\_\
*/

#pragma once 
#include "models/model.hpp"
#include "types/state.hpp"
#include <stdexcept>

class Vasicek : public RatesModel {
    
    public :
    /**
     * @brief Creates a Vasicek model
     *
     * @param a speed of reversion
     * @param b long term mean
     * @param sigma the volatility
     * 
     */
    Vasicek(double a, double b, double sigma) : 
        a_(a),
        b_(b),
        sigma_(sigma) {
            if (a<=0) throw std::invalid_argument("Vasicek : arg 'a' must be strictly greater than zero");
            if (b<=0) throw std::invalid_argument("Vasicek : arg 'b' must be strictly greater than zero");
            if (sigma<=0) throw std::invalid_argument("Vasicek : arg 'sigma' must be strictly greater than zero");
        };

    virtual ~Vasicek() override = default;
    double drift(double t, State& state) override;
    double diffusion(double t, State& state) override;

    private :

    double a_;
    double b_;
    double sigma_;

};