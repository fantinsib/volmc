#include "types/simulationresult.hpp"
#include <memory>




SimulationResult::SimulationResult(std::shared_ptr<std::vector<double>> paths, size_t seed,
                    size_t n_paths, size_t n_steps):
                    origin_seed_(seed),
                    n_paths_(n_paths),
                    n_steps_(n_steps),
                    paths_(std::move(paths))
    {
        size_t paths_size = paths_->size();
        if (n_paths_*n_steps_ != paths_size) throw std::invalid_argument("SimulationResult constructor : dimension of path vector does not match specified dimensions") ;
    }

double SimulationResult::avg_terminal_value(){
    double total_count = 0;
    for (size_t p = 0; p < n_paths_; p++){
        total_count += (*paths_)[(p + 1) * (n_steps_ + 1) - 1];
    }
    return total_count/static_cast<double>(n_paths_);
}