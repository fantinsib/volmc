#pragma once

#include <vector>
#include <optional>

#include "state.hpp"


/**
 * @brief Struct containing a vector of States 
 * representing the evolution of the asset over
 * the simulation. 
 * 
 */
struct Path{
public:

    using iterator = std::vector<State>::iterator;
    using const_iterator = std::vector<State>::const_iterator;

    iterator begin() {return states.begin();}
    iterator end() {return states.end();}

    const_iterator begin() const {return states.begin();}
    const_iterator end() const {return states.end();}

    State end_state() {return states.back();}
    float size() {return states.size();}
    void set_size(size_t size) {
        states.resize(size);
    };
    void set_step(State state, int i) {
        states[i] = state;
    }

private:
    std::vector<State> states;
};


