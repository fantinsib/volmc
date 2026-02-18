#pragma once

#include <optional>
#include <stdexcept>
#include <cmath>
/**
 * @brief Snapshot of an instant t in a Path
 * 
 */
struct State {

    double S;
    std::optional<double> v = std::nullopt;

    double spot() const {return S;}
    const std::optional<double> vol() const {
        if (!v.has_value()) return std::nullopt;
        return std::sqrt(*v);
    }

    const std::optional<double>& variance() const { return v; }


};