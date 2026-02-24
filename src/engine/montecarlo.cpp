


#include "types/path.hpp"
#include "engine/montecarlo.hpp"
#include "types/simulationresult.hpp"
#include "types/state.hpp"
#include <memory>
#include <optional>
#include <random>
#include <stdexcept>
#include <exception>
#include <thread>
#include <omp.h>
#include <utility>



std::vector<double> MonteCarlo::simulate_path(double S0, 
                                        size_t n, 
                                        float T,
                                        std::mt19937& rng, 
                                        std::optional<float> v0
                                        )
    {
    double dt = T/static_cast<double>(n);
    std::pair<double, double> state = scheme_->init_state(S0, v0);
    std::vector<double> path(n+1);
    path[0] = state.first;
    
    for (size_t step = 1; step <= n; step ++){
        double S_t = state.first;
        double v_t = state.second;
        std::pair<double, double> state = scheme_->step(S_t, v_t, step, dt, rng);
        path[step] = state.first;
    }

    return path;
}


void MonteCarlo::generate_path_inplace(double* path, double S0, size_t n, double T, 
                                       std::mt19937& rng, std::optional<double> v0) {
    double dt = T / static_cast<double>(n-1);
    std::pair<double, double> state = scheme_->init_state(S0, v0);
    
    path[0] = state.first;
    
    for (size_t step = 1; step < n; ++step) {
        double S_t = state.first;
        double v_t = state.second;
        state = scheme_->step(S_t, v_t, step, dt, rng);
        path[step] = state.first;
    }
}



SimulationResult MonteCarlo::generate_spot(float S0, 
                                      size_t n, 
                                      float T, 
                                      size_t n_paths, std::optional<double> v0){
    
    std::vector<double> all_paths(n_paths*(n+1));
    std::exception_ptr eptr = nullptr;

    std::vector<size_t> seeds_vector(n_paths);
    for (size_t i = 0; i < n_paths; i++){
        seeds_vector[i] = rng_();
    }

    #pragma omp parallel for num_threads(n_jobs_)
    for (size_t p = 0; p < n_paths; p++){
        try {
            std::mt19937 rng(static_cast<unsigned int>(seeds_vector[p]));
                double* path_ptr = &all_paths[p * (n + 1)];
                generate_path_inplace(path_ptr, S0, n+1, T, rng, v0);
            }
        catch(...) {
            #pragma omp critical 
            {
                if (!eptr) eptr = std::current_exception();
            }
        }
    }
    if (eptr) std::rethrow_exception(eptr);

    return SimulationResult(std::make_shared<std::vector<double>>(all_paths), seed_,  n, n_paths); 
}

void MonteCarlo::configure(std::optional<int> seed, std::optional<int> n_jobs, std::optional<bool> return_variance){

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

    if (return_variance.has_value()) {
        return_variance_ = return_variance.value();
    }
}