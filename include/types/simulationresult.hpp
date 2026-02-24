#pragma once 

#include "state.hpp"
#include "path.hpp"
#include "models/model.hpp"
#include "schemes/schemes.hpp"
#include <stdexcept>
#include <vector>
#include <memory>


/**
 * @brief Structure containing the paths as row-major vectors and representing 
 * the evolution of the spot and of the variance of the process
 * 
 */
struct SimulationResult {


    /**
     * @brief Construct a new SimulationResult object
     * 
     * @param paths A shared pointer to a vector containing the paths
     * @param seed The seed used to generate the path 
     * @param n_steps The number of steps of the generation 
     * @param n_paths the nuùber of paths of the generation
     */
    SimulationResult(std::shared_ptr<std::vector<double>> paths, size_t seed,
                    size_t n_steps, size_t n_paths);
    size_t get_npaths() const {return n_paths_;}
    size_t get_seed() const {return origin_seed_;}
    size_t get_nsteps() const {return n_steps_;}
    size_t get_path_size() const {return n_steps_+1;}

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
    const std::vector<double>& get_vol() const {
    if (!vols_ || vols_->empty()) {
        throw std::invalid_argument(
            "SimulationResult : no path for volatility was generated. "
            "Use MonteCarlo.configure to change generation settings."
        );
    }
    return *vols_;
    }


    private :
        std::shared_ptr<std::vector<double>> paths_;
        std::shared_ptr<std::vector<double>> vols_;
        const size_t origin_seed_;
        const size_t n_paths_;
        const size_t n_steps_;


};

