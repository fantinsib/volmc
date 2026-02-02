#pragma once



#include <optional>
#include <random>

#include "types/state.hpp"

class Scheme{

public:
    virtual ~Scheme() = default; 

    virtual State init_state(float S0, std::optional<float> v0) const = 0;
    virtual State step(const State& state, int i, float dt, std::mt19937& rng) const = 0;

};