#include "schemes.hpp"
#include "models/heston/heston.hpp"
#include "types/state.hpp"

#include <optional>
#include <random>


/**
 * @brief Scheme for the discretization of the Heston model with the
 * Quadratic Exponential method. 
 *
 * @param model : A Heston model
 * 
 */
class QE : public Scheme {

public:
    explicit QE(const Heston& model, float psi_threshold = 1.5f);

    State init_state(double S0, std::optional<double> v0) const override;
    /**
     * @brief Generates a new Heston spot value and volatility level 
     * using a Quadratic Exponential discretization
     * 
     * @param state current state
     * @param i the current step of the generation process
     * @param dt the time step
     * @param rng the random number generator
     * @return State : a State object containing the new spot price
     */
    State step(const State& state, int i, float dt, std::mt19937& rng) const override;

    float psi_c() const {return psi_threshold_;}
    void set_psi_c(float p);

private:
    Heston model_;
    float psi_threshold_;

    float inv_psi(float u, float p, float beta) const;

};