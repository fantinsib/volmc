#pragma once

#include <optional>

/**
 * @brief Snapshot of an instant t in a Path
 * 
 */
struct State {

    double S;
    std::optional<double> v = std::nullopt;

    double spot() const {return S;}
    std::optional<double> vol() const {
        return v;
}

};