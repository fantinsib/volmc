#include "schemes.hpp"
#include "models/heston/heston.hpp"
#include "types/state.hpp"

#include <optional>
#include <random>



/**
 * @brief Scheme for the discretization of the Black Scholes
 * model with log Euler method. 
 *
 * @param model : a BlackScholes model 
 *
 */
class EulerHeston : public Scheme 
{

    public:
    //constructor
    EulerHeston(Heston model):
    model(model)
    {};

    Heston model;

    /**
    * @brief Creates the initial state at time 0
    * 
    *
    * @param S0 the spot at time 0
    * @param v0 optional : the volatility at time 0 - required for Heston 
    * 
    * @return State : a State object representing the initial state
    */
    State init_state(double S0, std::optional<double> v0) const override;

    /**
    * @brief Generates a new Heston spot value and volatility level 
    * using a log-Euler discretization
    * 
    *
    * @param state current state
    * @param i the current step of the generation process
    * @param dt the time step
    * @param rng the random number generator
    * @return State : a State object containing the new spot price
    */
    State step(const State& state, int i, float dt, std::mt19937& rng) const override;

};