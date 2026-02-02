
#pragma once

#include <optional>
#include "types/date.hpp"
#include "options/options.hpp"
#include <memory>

struct OptionContract {

    OptionContract(float K, float T, std::shared_ptr<Option> option_type, std::optional<Date> date_= std::nullopt);
    float K;
    float T;
    std::optional<Date> date;
    std::shared_ptr<Option> option_type;
};