#include "types/simulationresult.hpp"




float SimulationResult::avg_terminal_value(){

    std::vector<Path> all_path = pathbundle->paths;

    float sum_t_val = 0;
    for (Path p : all_path){
        sum_t_val += p.end_state().at(0);
    }
    return sum_t_val/n_paths;

}