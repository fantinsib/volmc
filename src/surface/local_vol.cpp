#include "surface/local_vol.hpp"
#include <algorithm>
#include <stdexcept>

LocalVolatilitySurface::LocalVolatilitySurface(std::vector<double> times,
                           std::vector<double> spots,
                           std::vector<double> sigma):

                           times_(times),
                           spots_(spots),
                           loc_vol_(sigma) 
{
    size_t nt = times_.size();
    size_t nS = spots_.size();

    if (nt<2 || nS < 2)
        throw std::invalid_argument("LocalVolatilitySurface constructor : at least two points are required to build a local volatility surface");

    if (loc_vol_.size() != nt*nS)
        throw std::invalid_argument("LocalVolatilitySurface constructor : the local volatility vector has wrong dimension for inputs S and t.");
};

size_t get_l_bound_index(const std::vector<double>& vec, double val){

    if (val <= vec.front()) return 0;
    if (val >= vec.back()) return vec.size()-2;

    auto it = std::lower_bound(vec.begin(), vec.end(), val);

    return static_cast<size_t>(std::distance(vec.begin(), it)-1);

};

double LocalVolatilitySurface::sigma(double t, double S) const{


    size_t nS = spots_.size();
    size_t nt = times_.size();
 
    double S_clamped = std::clamp(S, spots_.front(), spots_.back());
    double t_clamped = std::clamp(t, times_.front(), times_.back());

    size_t low_S_idx = get_l_bound_index(spots_, S_clamped);
    size_t low_t_idx = get_l_bound_index(times_, t_clamped);

    low_S_idx = std::min(low_S_idx, nS -2);
    low_t_idx = std::min(low_t_idx, nt -2);

    double S0 = spots_[low_S_idx];
    double t0 = times_[low_t_idx];
    
    double S1 = spots_[low_S_idx+1];
    double t1 = times_[low_t_idx+1];
    
    double wt = (t1 == t0) ? 0.0 : (t_clamped - t0) / (t1 - t0);
    double wS = (S1 == S0) ? 0.0 : (S_clamped- S0) / (S1 - S0);

    wt = std::clamp(wt, 0.0, 1.0);
    wS = std::clamp(wS, 0.0, 1.0);

    auto at = [&](size_t ii, size_t jj) {
        return loc_vol_[ii * nS + jj]; // row-major par temps
    };

    const double v00 = at(low_t_idx,  low_S_idx);
    const double v01 = at(low_t_idx,   low_S_idx+1);
    const double v10 = at(low_t_idx+1, low_S_idx);
    const double v11 = at(low_t_idx+1, low_S_idx+1);

    const double v0 = (1.0 - wS) * v00 + wS * v01;
    const double v1 = (1.0 - wS) * v10 + wS * v11;
    double sigma = (1.0 - wt) * v0 + wt * v1;

    return sigma;

};


