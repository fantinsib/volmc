


#include "types/path.hpp"
#include "engine/montecarlo.hpp"
#include "types/simulationresult.hpp"
#include "types/state.hpp"
#include <optional>
#include <random>
#include <stdexcept>




Path MonteCarlo::simulate_path(float S0, size_t n, float T, std::optional<float> v0){

    float dt = T/static_cast<float>(n);
    State state = scheme_->init_state(S0, v0);
    Path path; 
    path.set_size(static_cast<size_t>(n+1));
    path.set_step(state, 0);

    for (size_t step = 0; step < n; step ++){

        state = scheme_->step(state, dt, rng_);
        path.set_step(state, step+1);
    }

    return path;
}

SimulationResult MonteCarlo::generate(float S0, 
                                      size_t n, 
                                      float T, 
                                      size_t n_paths, 
                                      std::optional<float> v0){
    
    std::vector<Path> all_paths(n_paths);

    for (size_t p = 0; p < n_paths; p++){

       const Path path_n = simulate_path(S0, n, T, v0);
        all_paths[p] = path_n;


    }
    auto bundle= std::make_shared<PathBundle>();
    bundle->paths = all_paths;
    bundle->n_paths = n_paths;
    bundle->n_steps = n;
    return SimulationResult{seed_, n_paths, n,bundle}; 
}

void MonteCarlo::configure(std::optional<int> seed){

    if (seed.has_value()) {
        if (seed.value()<0) throw std::invalid_argument("MonteCarlo::configure : seed value must be strictly positive");
        seed_ = static_cast<size_t>(seed.value());
        rng_.seed(seed_);
    }
}