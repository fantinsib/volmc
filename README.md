# Monte Carlo 

This project aims at implementing a light Monte Carlo engine for asset pricing models with non-constant volatility. 

## Current features 

This project is organised around modules : models, schemes, options, pricing

### **`Model`**

The Model class represents the parameters for the stochastic dynamic.

- Black-Scholes model (`BlackScholes`)
    - `mu` : the drift rate
    - `sigma` : the volatility
- Heston model (`Heston`)
    - `mu` : the drift rate
    - `kappa` : mean-reversion speed
    - `theta` : long-term variance
    - `epsilon` : vol-of-vol
    - `rho` : spot/vol brownian correlation
- Dupire model (`Dupire`)            
    - `r` : the risk-free rate
    - `q` : the dividend yield
    - `local_volatility_surface` : the local volatility surface as a `LocalVolatilitySurface` instance
- Vasicek model (`Vasicek`)
    - `a`: the mean-reversion speed
    - `b` : mean long-term rate
    - `sigma` : the volatility

### **`Schemes`**

Schemes describe how to discretize a model at each time step. 

- Generic Euler discretization (`Euler`), supported for Black-Scholes, Dupire, Vasicek
- Heston Euler-type discretization with full truncation (`EulerHeston`)
- Quadratic Exponential for Heston (`QE`)


### **`Pricing`** 

- `MonteCarlo`
    - `.generate()` : returns a specified number of paths. 
        - `S0` initial spot value
        - `v0` optional initial variance value for Heston model
        - `n` the number of steps in each path
        - `T` the time period of generation
        - `n_paths` the number of paths to generate
    - `.configure()` : use to set the seed of the engine and the `n_jobs` parameter for the number of CPU cores to use (-1 for maximum)
- `Pricer`
    - Take as input a `MarketState` representing the state of the market at time of pricing, the number of steps and paths to be used for pricing and a `MonteCarlo`engine.
        - `.price()` returns an Monte Carlo simulated price for an `Instrument`


### **`Options`**

`Instrument` is the base class for creating financial instruments. `Instrument` takes as input an `OptionContract` with strike price `K` and maturity in years `T` and a `Payoff`. Currently supported payoffs are the following : 
- `CallPayoff` : European Call Payoff with payoff $\max((S-K), 0)$
- `PutPayoff` : European Put Payoff with payoff $\max((K-S), 0)$
- `DigitalCallPayoff` : European Digital Call Payoff with payoff $1_{S>K}$
- `DigitalPutPayoff` : European Digital Put Payoff with payoff $1_{K>S}$
- `Barrier` : Barrier payoff, depending on direction, nature and barrier value

## Basic Demo

A comprehensive demo notebook is available [here](doc/demo_notebook.ipynb).

```python
from volmc import Heston, QE, MonteCarlo, SimulationResult

heston = Heston(mu=0.02, 
                kappa=2, 
                theta= 0.05, 
                epsilon=0.3, 
                rho = -0.6)

print(f"Feller condition met : {heston.feller_condition()}")

mc = MonteCarlo(QE(heston))
mc.configure(seed=1, n_jobs = -1)

sim = mc.generate(S0 = 100, 
                  v0 = 0.04, 
                  n = 252, 
                  T = 1, 
                  n_paths = 500_000)

S = sim.spot_values()
V = sim.var_values()
```

**Output**

![Spot Process](doc/img/spot_process_qe.png)

![Var Process](doc/img/var_process_QE.png)

## Basic Pricing Example

```python
S = 50
K = 51
sigma = 0.15
n = 252
T = 1.2
r= 0.02

print(f"Black Scholes price : {bs_put_price(S, K, sigma, T, r):.4f}")
```

```python
Black Scholes price : 3.1635
```

```python
from volmc.options import Put
from volmc.pricing import MonteCarlo, Pricer
from volmc.types import MarketState

bs = BlackScholes(r, sigma)
mc = MonteCarlo(Euler(bs))
mc.configure(1, -1)

state = MarketState(S = S, r = r)

p = Pricer(marketstate= state, 
           n_steps = 252,
           n_paths = 300_000,
           engine = mc)

put = Put(K, T) #preset Put creation

mc_price = p.price(put)

print(f"Monte Carlo estimated price : {mc_price:.4f}")
```
```python
Monte Carlo estimated price : 3.1691
```

The following graph shows the convergence of the price estimation depending on the number of paths generated. Red line indicates the exact Black Scholes price for this put. 

![Convergence](doc/img/convergence.png)

## Notes and additional documentation 

The core of this module is coded in C++ and accessed through the [Python API](volmc/_api.py). 

### Theoretical background

Some notes on the theoretical background of this project can be found in the [doc folder](doc/background).


### To be added
- Optimizations for faster computation
- More models & schemes
- Variance reduction for MonteCarlo 