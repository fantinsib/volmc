/*
                       _                        _       
 _ __ ___   ___  _ __ | |_ ___    ___ __ _ _ __| | ___  
| '_ ` _ \ / _ \| '_ \| __/ _ \  / __/ _` | '__| |/ _ \ 
| | | | | | (_) | | | | ||  __/ | (_| (_| | |  | | (_) |
|_| |_| |_|\___/|_| |_|\__\___|  \___\__,_|_|  |_|\___/ 

*/

#include "engine.hpp"
#include "schemes/schemes.hpp"
#include <memory>
#include <optional>
#include <random>
#include "types/path.hpp"
#include "types/simulationresult.hpp"


/**
 * @brief Construct a new Monte Carlo engine
 *
 * @param scheme : a scheme instanciated with a model to simulate 
 * 
 */
class MonteCarlo : Engine 
{

public:
    //Constructor with a ptr towards a scheme 
    explicit MonteCarlo(std::shared_ptr<Scheme> scheme) : 
        scheme_(std::move(scheme)) {
            std::random_device rd;
            std::mt19937 rng(rd());
            seed_ = rng();
            rng_.seed(seed_);
        }
    //Constructor overload for passing the scheme by value
    template <class SchemeT>
    explicit MonteCarlo(SchemeT scheme)
    : scheme_(std::make_shared<SchemeT>(std::move(scheme))) {
            std::random_device rd;
            std::mt19937 rng(rd());
            seed_ = rng();
            rng_.seed(seed_);
    }



    /**
     * @brief Simulates the scheme over a specified time interval
     * 
     * @param S0 the initial spot
     * @param n the number of steps 
     * @param T the time horizon
     * @param v0 optional initial volatility 
     * @return Path object
     */
    Path simulate_path(float S0, size_t n, float T, std::mt19937& rng,std::optional<float> v0 = std::nullopt);

    /**
     * @brief Simulates n_paths paths that follow the scheme.  
     * 
     * @param S0 the initial spot
     * @param n the number of steps
     * @param T the time horizon
     * @param n_path the number of paths to simulate
     * @param v0 the initial volatility 
     * @return SimulationResult 
     */
    SimulationResult generate(float S0, size_t n, float T, size_t n_path, std::optional<float> v0 = std::nullopt);
    
    /**
     * @brief Method allowing to configure the engine 
     * 
     * @param seed : the seed to be used for the generation
     */
    void configure(std::optional<int> seed = std::nullopt, std::optional<int> n_jobs = std::nullopt);
    
    //returns the current seed
    int get_seed() {return seed_;}

    // Reset the state of the random number generator to its initial state
    void reset_rng() {rng_.seed(seed_);};
    
    // Forgets the previously set seed and set a new random seed
    void reset_seed() {
        std::random_device rd;
        std::mt19937 rng(rd());
        seed_ = rng();
        user_set_seed_ = false; 
    }

    private:
    const std::shared_ptr<Scheme> scheme_; 

    size_t seed_;
    std::mt19937 rng_;
    int n_jobs_ = 1;
    bool user_set_seed_ = false; 

    
};