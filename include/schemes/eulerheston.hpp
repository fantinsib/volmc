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
    EulerHeston(Heston model):
    model(model)
    {};

    Heston model;

    State init_state(float S0, std::optional<float> v0) const override;

    State step(const State& state, float dt, std::mt19937& rng) const override;

};