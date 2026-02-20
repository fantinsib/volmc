#pragma once 

#include "state.hpp"
#include "path.hpp"
#include "models/model.hpp"
#include "schemes/schemes.hpp"
#include <stdexcept>
#include <vector>
#include <memory>

struct PathBundle {

    size_t n_paths;
    size_t n_steps;
    bool has_variance = false;

    std::vector<Path> paths;

    /**
     * @brief Returns a vector of all the spot values
     * in row-major order
     * 
     * @return std::vector<float> 
     */
    std::vector<float> unravel_spot(){

        std::vector<float> spots;
        for (size_t p = 0; p<n_paths; p++){
            for (State& s : paths[p]){
                spots.push_back(s.at(0));
            }
        }
        return spots;
    };

    /**
     * @brief Returns a vector of all the var values
     * in row-major order
     * 
     * @return std::vector<float> 
     */
    std::vector<float> unravel_var(){
        std::vector<float> vars;
        for (size_t p = 0; p<n_paths; p++){
            for (State& s : paths[p]){
                if (!has_variance) return std::vector<float>(0);
                vars.push_back(s.at(1));
            }
    }
        return vars;

}
};

/**
 * @brief Structure containing a collection of Paths and representing 
 * the evolution of the spot and of the variance of the process
 * 
 */
struct SimulationResult {

    size_t origin_seed;
    size_t n_paths;
    size_t n_steps;

    size_t get_npaths() const {return n_paths;}
    size_t get_seed() const {return origin_seed;}
    size_t get_nsteps() const {return n_steps;}

    /**
     * @brief Returns the average final value of
     * the spots in all the paths
     * 
     * @return float 
     */
    float avg_terminal_value();

    /**
     * @brief Returns a vector of all the Paths in the simulation
     * 
     * @return std::vector<Path> 
     */
    std::vector<Path> get_paths() const {return pathbundle->paths;}

    std::shared_ptr<PathBundle> pathbundle;


};

