#include "schemes/qe.hpp"

#include "models/heston/heston.hpp"
#include "types/state.hpp"
#include <optional>
#include <random>
#include <cmath>
#include <stdexcept>
#include <utility>




QE::QE(const Heston& model, float psi_threshold) :
    model_(model),
    psi_threshold_(psi_threshold)
    {
    if (psi_threshold > 2 || psi_threshold< 1) throw std::invalid_argument("QE Constructor : the value of psi_threshold must be in [1,2]");
    };

void QE::set_psi_c(float p){
    if (p>2 || p<1) throw std::invalid_argument("QE::set_psi_c : the value of psi_threshold must be in [1,2]");
    psi_threshold_ = p; 
};


std::pair<double, double> QE::init_state(double S0, std::optional<double> v0) const {

    if (!v0.has_value()) throw std::invalid_argument("QE::init_state : intial state must receive a value for initial volatility");
    std::pair res(S0, v0.value());
    return res;
}


std::pair<double, double> QE::step(const double S, double v, 
                        int i, 
                        float dt, 
                        std::mt19937& rng) const 
{

    std::normal_distribution<double> N;
    std::uniform_real_distribution<double> U(0.0f, 1.0f);

    
    double V = v*v;
    double exp_sp = std::exp(-model_.kappa * dt);

    double E_X = model_.theta + (V- model_.theta) * exp_sp;

    double VAR_X1 = (V * 
                    (model_.epsilon*model_.epsilon) * exp_sp) *
                    (1-exp_sp)
                    /model_.kappa;

    double VAR_X2 = ((model_.theta * model_.epsilon * model_.epsilon) *
                    (1-exp_sp) * 
                    (1-exp_sp))
                    /(2*model_.kappa);

    double VAR_X = VAR_X1 + VAR_X2;

    double psi = VAR_X/(E_X*E_X);
    double u = U(rng);
    double logSt;
    double V_next;
    double Z = N(rng);

    if (psi > psi_threshold_) { //exp regime
        
        double p = (psi-1)/(psi+1);
        double beta = (1-p)/E_X;
        V_next = inv_psi(u, p, beta);
        double V_int = 0.5 * (V+V_next);

        logSt = std::log(S) + 
                model_.mu * dt - 
                0.5 * V_int * dt + 
                (model_.rho / model_.epsilon) *
                (V_next - V - model_.kappa*(model_.theta - V_int)*dt) +
                std::sqrt((1-model_.rho*model_.rho)*V_int*dt)*Z;

    }

    else { //quadratic regime 

        double zq = N(rng);
        double dpsi = 2.0f/psi;
        double b_2 = dpsi - 1 + std::sqrt(dpsi*(dpsi-1));
        double a = E_X/(1+b_2);

        double sqrt_b2 = std::sqrt(b_2);
        V_next = a * (zq + sqrt_b2) * (zq + sqrt_b2);
        double V_int = 0.5 * (V + V_next);
        logSt = std::log(S) + 
                model_.mu * dt - 
                0.5 * V_int * dt + 
                (model_.rho / model_.epsilon) *
                (V_next - V - model_.kappa*(model_.theta - V_int)*dt) +
                std::sqrt((1.0f-model_.rho*model_.rho)*V_int*dt)*Z;

               
    }

    return std::pair<double, double>(std::exp(logSt), std::sqrt(V_next));
};

float QE::inv_psi(float u, float p, float beta) const{

    if (u<=p) return 0;
    else return (1/beta)*std::log((1-p)/(1-u));

};