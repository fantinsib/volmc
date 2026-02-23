#pragma once 

#include "state.hpp"
#include "path.hpp"
#include "models/model.hpp"
#include "schemes/schemes.hpp"
#include <stdexcept>
#include <vector>
#include <memory>


/**
 * @brief Structure containing a collection of Paths and representing 
 * the evolution of the spot and of the variance of the process
 * 
 */
struct SimulationResult {

    SimulationResult(std::shared_ptr<std::vector<double>> paths, size_t seed,
                    size_t n_steps, size_t n_paths);
    size_t get_npaths() const {return n_paths_;}
    size_t get_seed() const {return origin_seed_;}
    size_t get_nsteps() const {return n_steps_;}

    /**
     * @brief Returns the average final value of
     * the spots in all the paths
     * 
     * @return float 
     */
    double avg_terminal_value();

    const std::vector<double>& get_paths() const {
        return *paths_;
    }


    private :
        std::shared_ptr<std::vector<double>> paths_;
        const size_t origin_seed_;
        const size_t n_paths_;
        const size_t n_steps_;


};

