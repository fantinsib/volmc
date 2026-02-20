#include "models/dupire/dupire.hpp"

#include "surface/local_vol.hpp"
#include "types/state.hpp"

Dupire::Dupire(float r, float q, 
           std::shared_ptr<LocalVolatilitySurface> loc_vol_surface):

           r_(r), q_(q), lv_surface_(loc_vol_surface)

{}

double Dupire::drift(double t, const State &state) const {
    return (r_ - q_)*state.at(0);
}

double Dupire::diffusion(double t, const State &state) const {
    double S = state.at(0);
    return lv_surface_->sigma(t, S)*S;
}

