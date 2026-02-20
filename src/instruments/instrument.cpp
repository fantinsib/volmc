

#include "instruments/instrument.h"
#include "payoff/payoff.h"
#include "types/path.hpp"
#include "types/simulationresult.hpp"
#include "types/state.hpp"



double Instrument::compute_payoff(const SimulationResult& simulation) const {

    std::vector<Path> paths = simulation.get_paths();
    const size_t n_paths = simulation.get_npaths();

    double payoff_avg = 0;
    double K = contract_.K;

    for (Path p : paths) {
        payoff_avg += payoff_->compute(p, K);
    }
    return payoff_avg/static_cast<double>(n_paths);
};

