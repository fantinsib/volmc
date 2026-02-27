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
    
    A `MonteCarlo` engine can be created either by loading a model associated with a scheme at instanciation or using pre-set engine creators. See below for examples.
- `Pricer`
    - Take as input a `MarketState` representing the state of the market at time of pricing, the number of steps and paths to be used for pricing and a `MonteCarlo`engine.
        - `.price()` returns an Monte Carlo simulated price for an `Instrument`
        - `.batch_price()` prices a list of `Instrument` using the same simulation
        - `.delta()` returns the simulated delta using bump and revalue technique
        - `.gamma()` returns the simulated gamma using bump and revalue


### **`Options`**

`Instrument` is the base class for creating financial instruments. `Instrument` takes as input an `OptionContract` with strike price `K` and maturity in years `T` and a `Payoff`. Currently supported payoffs are the following : 
- `CallPayoff` : European Call Payoff with payoff $\max((S-K), 0)$
- `PutPayoff` : European Put Payoff with payoff $\max((K-S), 0)$
- `DigitalCallPayoff` : European Digital Call Payoff with payoff $1_{S>K}$
- `DigitalPutPayoff` : European Digital Put Payoff with payoff $1_{K>S}$
- `BarrierPayoff` : Barrier payoff, depending on direction, nature and barrier value

## Basic Demo

A comprehensive demo notebook is available [here](doc/demo_notebook.ipynb).

### Creating a model and simulating paths
```python
from volmc.models import Heston
from volmc.schemes import QE
from volmc.pricing import MonteCarlo

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
```python
#Alternatively :
from volmc.pricing import BlackScholesEngine, HestonEngine

mc_bs = BlackScholesEngine(mu = 0.02, sigma = 0.2)
mc_heston = HestonEngine(mu=0.02, 
                         kappa=2, 
                         theta= 0.05, 
                         epsilon=0.3, 
                         rho = -0.6)
mc_heston.configure(1, -1)
mc_heston.generate(100, 0.04, 252, 1 , 500_000)
```
### Creating instruments

```python
from volmc.options import *

contract = OptionContract(K = 95, T = 1.2)
call_option = Instrument(contract, CallPayoff())

#Alternatively :
call_option = Call(95, 1.2)
put_option = Put(105, 1.2)
```

```python
barrier_payoff = Barrier(H = 130, 
                         direction = "up",
                         nature = "out",
                         payoff = CallPayoff())

contract = OptionContract(110, 1.2)

#Creates an up-and-out call option with barrier value 130 and strike 110 
up_out_call = Instrument(contract, barrier_payoff)
```


### Pricing options

```python
#For a put option :

S = 50
K = 51
sigma = 0.15
n = 252
T = 1.2
r= 0.02

print(f"Black Scholes price : {bs_put_price(S, K, sigma, T, r):.4f}")
print(f"Black Scholes delta : {bs_delta_put(S, K, sigma, T, r):.4f}")
```

```python
Black Scholes price : 3.1635
Black Scholes delta : -0.4571
```

```python
from volmc.options import Put
from volmc.pricing import MonteCarlo, Pricer
from volmc.types import MarketState

bs = BlackScholes(r, sigma)
mc = MonteCarlo(Euler(bs))
# Alternatively :
# mc = BlackScholesEngine(r, sigma)
mc.configure(1, -1)

state = MarketState(S = S, r = r)

p = Pricer(marketstate= state, 
           n_steps = 252,
           n_paths = 300_000,
           engine = mc)

put = Put(K, T) #preset Put creation

mc_price = p.price(put)
mc_delta = p.delta(put, h = 0.0001)

print(f"Monte Carlo estimated price : {mc_price:.4f}")
print(f"Monte Carlo estimated delta : {mc_delta:.4f}")
```
```python
Monte Carlo estimated price : 3.1691
Monte Carlo estimated delta : -0.4533
```
### Batch pricing
Price a list of options with the same simulation :
```python

#Creating a series of instruments :
call_105 = Call(105, T)
call_92 = Call(92, T)
call_110 = Call(110, T)

put_105 = Put(105, T)
put_92 = Put(92, T)
put_110 = Put(110, T)

instrument_list = [call_105, call_92, call_110, put_105, put_92, put_110]

#Creating an engine :
mc = BlackScholesEngine(0.02, 0.2)
mc.configure(1, -1)

ms = MarketState(S0, r)

engine = Pricer(ms, 252, 100_000, mc)

#Pricing :
prices = engine.batch_price(in_list)
print(prices)
```
```bash
[7.677609682895932, 14.418038944821866, 5.860376924775665, 10.078080185562353, 4.126795220635833, 13.14227597623177]
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