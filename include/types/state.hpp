#pragma once

#include <optional>

struct State {

    float S;
    std::optional<float> v = std::nullopt;

    float spot() const {return S;}
    std::optional<float> vol() const {
        return v;
}

};