

#include "instruments/instrument.h"
#include "payoff/payoff.h"
#include "types/path.hpp"
#include "types/simulationresult.hpp"
#include "types/state.hpp"
#include <span>



double Instrument::compute_payoff(const SimulationResult& simulation) const {

    const std::vector<double>& paths = simulation.get_paths();
    const size_t n_paths = simulation.get_npaths();
    const size_t p_size = simulation.get_path_size();

    double payoff_avg = 0;
    double K = contract_.K;

    for (size_t p = 0; p < n_paths; p++) {
        std::span<const double> path_view(paths.data() + (p * p_size), p_size);
        payoff_avg += payoff_->compute(path_view, K);
    };
    return payoff_avg/static_cast<double>(n_paths);
};

