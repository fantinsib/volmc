
#include "options/options.hpp"
#include <memory>
#include <stdexcept>


OptionContract::OptionContract(double K, 
                               double T) :
                               K(K), 
                               T(T)
                            
{   
    if (K<0) 
        throw std::invalid_argument("OptionContract constructor : value for K cannot be negative");
    if (T<0)
        throw std::invalid_argument("OptionContract constructor : value for T cannot be negative");
}
