

#include "instruments/instrument.h"
#include "payoff/payoff.h"
#include "types/path.hpp"
#include "types/simulationresult.hpp"
#include "types/state.hpp"

double Instrument::compute_payoff(const Path& path) const {

    State f_state = path.end_state();
    float terminal_spot = f_state.spot();
    return payoff_->compute(terminal_spot);
};

double Instrument::compute_payoff(const SimulationResult& simulation) const {

    std::vector<Path> paths = simulation.get_paths();
    const size_t n_paths = simulation.get_npaths();

    double payoff_avg = 0;

    for (Path p : paths) {
        payoff_avg += payoff_->compute(p.end_state().spot());
    }
    return payoff_avg/static_cast<double>(n_paths);
};

