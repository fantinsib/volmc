#pragma once 

#include "state.hpp"
#include "path.hpp"
#include "models/model.hpp"
#include "schemes/schemes.hpp"
#include <stdexcept>
#include <vector>


struct PathBundle {

    size_t n_paths;
    size_t n_steps;

    std::vector<Path> paths;

    std::vector<float> unravel_spot(){

        std::vector<float> spots;
        
        for (size_t p = 0; p<n_paths; p++){
            for (State& s : paths[p]){
                spots.push_back(s.spot());
            }
        }
        return spots;
    };

    std::vector<float> unravel_var(){
        std::vector<float> vars;

         
        for (size_t p = 0; p<n_paths; p++){

            for (State& s : paths[p]){
                if (!s.vol().has_value()) return std::vector<float>(0);
                vars.push_back(s.vol().value());
            }
    }
        return vars;

}
};

/**
 * @brief Structure containing a collection of Paths
 * 
 */
struct SimulationResult {

    size_t origin_seed;
    size_t n_paths;
    size_t n_steps;
    //Model model;
    //Scheme scheme;

    size_t get_npaths() {return n_paths;}
    size_t get_seed() {return origin_seed;}

    std::shared_ptr<PathBundle> pathbundle;


};

