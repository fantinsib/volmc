#include "schemes.hpp"
#include "models/black_scholes/black_scholes.hpp"
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
class EulerBlackScholes : public Scheme 
{

    public:
    EulerBlackScholes(BlackScholes model):
    model(model)
    {};

    BlackScholes model;

    /**
    * @brief Creates the initial state at time 0
    * 
    *
    * @param S0 the spot at time 0
    * @param v0 optional : the volatility at time 0 - not required for Black Scholes
    * 
    * @return State : a State object representing the initial state
    */
    State init_state(float S0, std::optional<float> v0) const override;

    /**
    * @brief Generates a new Black-Scholes spot using a log-Euler discretization
    * 
    * Simulates log(S_{t+dt}) = log(S_t) + (mu - 0.5*sigma^2)*dt + sigma*sqrt(dt)*Z,
    * with Z ~ N(0,1).
    *
    * @param state current state
    * @param dt the time step
    * @param rng the random number generator
    * @return State : a State object containing the new spot price
    */
    State step(const State& state, float dt, std::mt19937& rng) const override;

};