#pragma once



#include "surface/local_vol.hpp"
#include "models/model.hpp"
#include "types/state.hpp"

#include <memory>

struct Dupire : public Model {

public:
    /**
     * @brief Constructs a Dupire model
     * 
     * @param r the risk-free rate
     * @param q the dividend yield
     * @param loc_vol_surface the local volatility surface of the model
     */
    Dupire(float r, float q, 
           std::shared_ptr<LocalVolatilitySurface> loc_vol_surface);
    
    virtual ~Dupire() override = default;
    double sigma(float t, float S) const {return lv_surface_->sigma(t, S);}
    double drift(double t, const State& state) const override; 
    double diffusion(double t, const State& state) const override;
    
private:
    float r_; 
    float q_;
    std::shared_ptr<LocalVolatilitySurface> lv_surface_;

    
};
