#pragma once

#include "models/model.hpp"
#include <stdexcept>



struct Heston : Model {

    Heston(float mu, float kappa, float theta, float epsilon, float rho) :
    
    mu(mu), kappa(kappa), theta(theta), epsilon(epsilon), rho(rho)

    {
        if (kappa <= 0) throw std::invalid_argument("Heston Constructor : kappa must be strictly positive");
        if (theta <= 0) throw std::invalid_argument("Heston Constructor : theta must be strictly positive");
        if (epsilon <= 0) throw std::invalid_argument("Heston Constructor : epsilon must be strictly positive");
        if ((rho > 1) || (rho < -1))  throw std::invalid_argument("Heston Constructor : rho must be in [-1,1]");

        if ((2*kappa*theta) >= epsilon*epsilon) feller = true;
        else feller = false;
    }


    float mu;
    float kappa;
    float theta;
    float epsilon;
    float rho;

    bool feller_condition() {return feller;}

    private:

    bool feller;
};
