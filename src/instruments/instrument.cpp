

#include "instruments/instrument.h"
#include "payoff/payoff.h"
#include "types/path.hpp"
#include "types/simulationresult.hpp"
#include "types/state.hpp"
#include <span>



double Instrument::compute_payoff(const SimulationResult& simulation) const {

    const std::vector<double>& paths = simulation.get_paths();
    const size_t n_paths = simulation.get_npaths();
    const size_t n_steps = simulation.get_nsteps();

    double payoff_avg = 0;
    double K = contract_.K;

    for (size_t p = 0; p < n_paths; p++) {
        std::span<const double> path_view(paths.data() + (p * n_steps), n_steps);
        payoff_avg += payoff_->compute(path_view, K);
    };
    return payoff_avg/static_cast<double>(n_paths);
};

