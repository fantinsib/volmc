


#include "types/path.hpp"
#include "engine/montecarlo.hpp"
#include "types/simulationresult.hpp"
#include "types/state.hpp"
#include <optional>
#include <random>
#include <stdexcept>
#include <exception>
#include <thread>
#include <omp.h>



Path MonteCarlo::simulate_path(float S0, 
                               size_t n, 
                               float T,
                               std::mt19937& rng, 
                               std::optional<float> v0
                               ){

    float dt = T/static_cast<float>(n);
    State state = scheme_->init_state(S0, v0);
    Path path; 
    path.set_size(static_cast<size_t>(n+1));
    path.set_step(state, 0);

    for (size_t step = 0; step < n; step ++){

        state = scheme_->step(state, step, dt, rng);
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
    std::exception_ptr eptr = nullptr;

    std::vector<size_t> seeds_vector(n_paths);
    for (size_t i = 0; i < n_paths; i++){
        seeds_vector[i] = rng_();
    }

    #pragma omp parallel for num_threads(n_jobs_)
    for (size_t p = 0; p < n_paths; p++){
        try {
            std::mt19937 rng(static_cast<unsigned int>(seeds_vector[p]));
            all_paths[p] = simulate_path(S0, n, T, rng, v0);
            }
        catch(...) {
            #pragma omp critical 
            {
                if (!eptr) eptr = std::current_exception();
            }
        }
    }
    if (eptr) std::rethrow_exception(eptr);
    std::shared_ptr<PathBundle> bundle= std::make_shared<PathBundle>();
    bundle->paths = std::move(all_paths);
    bundle->n_paths = n_paths;
    bundle->n_steps = n;
    return SimulationResult{seed_, n_paths, n,bundle}; 
}

void MonteCarlo::configure(std::optional<int> seed, std::optional<int> n_jobs){

    if (seed.has_value()) {
        if (seed.value()<0) throw std::invalid_argument("MonteCarlo::configure : seed value must be positive");
        seed_ = static_cast<size_t>(seed.value());
        rng_.seed(seed_);
        user_set_seed_ = true; 
    }

    if (n_jobs.has_value()) {
        int hw = static_cast<int>(std::thread::hardware_concurrency());
        hw = (hw == 0 ? 1 : hw); 
        if (n_jobs.value() < -1 || n_jobs.value() == 0) throw std::invalid_argument("MonteCarlo::configure : n_jobs value must be strictly positive or equal to -1");
        if (n_jobs.value() > hw || n_jobs.value() == -1) n_jobs_ = hw;
        else (n_jobs_ = n_jobs.value());
    }
}