#pragma once
#include "types/state.hpp"

/**
 * @brief Base structure for models
 * 
 */
struct Model{
    
    virtual ~Model() = default;

    // Virtual function to compute the drift given S, var
    virtual double drift(double t, const State& state) const = 0;
    // Virtual function to compute the diffusion given S, var
    virtual double diffusion(double t, const State& state) const = 0; 

};

struct VectorModel{

};


struct EquityModel{

    
    virtual ~EquityModel() = default;

    // Virtual function to compute the drift given S, var
    virtual double drift(const EquityView& state) const = 0;
    // Virtual function to compute the diffusion given S, var
    virtual double diffusion(const EquityView& state) const = 0; 

};

struct RatesModel{
        
    virtual ~RatesModel() = default;

    // Virtual function to compute the drift given S, var
    virtual double drift(const RatesState& state) const = 0;
    // Virtual function to compute the diffusion given S, var
    virtual double diffusion(const RatesState& state) const = 0; 

};
