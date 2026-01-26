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

    State init_state(float S0, std::optional<float> v0) const override;
    State step(const State& state, float dt, std::mt19937& rng) const override;

    float psi_c() const {return psi_threshold_;}
    void set_psi_c(float p);

private:
    Heston model_;
    float psi_threshold_;

    float inv_psi(float u, float p, float beta) const;

};