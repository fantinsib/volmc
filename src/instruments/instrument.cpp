

#include "instruments/instrument.h"
#include "types/path.hpp"
#include "types/state.hpp"

double Instrument::compute_payoff(const Path& path) const {

    State f_state = path.end_state();
    float terminal_spot = f_state.spot();
    return payoff_->compute(terminal_spot);
};