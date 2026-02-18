#pragma once

#include <optional>
#include <stdexcept>
#include <cmath>
/**
 * @brief Snapshot of an instant t in a Path
 * @param S The spot price
 * @param v The variance
 */
struct State {

    double S;
    std::optional<double> var = std::nullopt;

    double spot() const {return S;}
    const std::optional<double> vol() const {
        if (!var.has_value()) return std::nullopt;
        return std::sqrt(*var);
    }

    const std::optional<double>& variance() const { return var; }


};