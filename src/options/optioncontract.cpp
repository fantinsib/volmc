
#include "options/optioncontract.hpp"
#include "options/options.hpp"
#include <memory>
#include <stdexcept>


OptionContract::OptionContract(float K, 
                               float T, 
                               std::shared_ptr<Option> option_type, 
                               std::optional<Date> date_) :

                               K(K), T(T),option_type(option_type), date(date_)


{   
    if (K<0) 
        throw std::invalid_argument("OptionContract constructor : value for K cannot be negative");
    if (T<0)
        throw std::invalid_argument("OptionContract constructor : value for T cannot be negative");
}
