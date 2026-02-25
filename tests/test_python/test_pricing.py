from volmc.models import BlackScholes, Heston, Dupire, Vasicek
from volmc.schemes import Euler, QE
from volmc.pricing import MonteCarlo, Pricer
from volmc.types import *
from volmc.options import *

import numpy as np
import pytest

def norm_cdf(x):
    return 0.5 * (1 + np.sign(x) * np.sqrt(1 - np.exp(-2*x**2/np.pi)))

def bs_call_price(S, K, sigma, T, r):
    d1 = (np.log(S/K) + ((sigma**2)/2 + r)*T)/(sigma*np.sqrt(T))
    d2 = d1 - np.sqrt(T)*sigma

    return S* norm_cdf(d1) - K*np.exp(-r*T)*norm_cdf(d2)

def bs_put_price(S, K, sigma, T, r):
    d1 = (np.log(S/K) + ((sigma**2)/2 + r)*T)/(sigma*np.sqrt(T))
    d2 = d1 - np.sqrt(T)*sigma

    return -S* norm_cdf(-d1) + K*np.exp(-r*T)*norm_cdf(-d2)

def test_vanilla_pricing():

    S0 = 100
    K_c = 95
    K_p = 105
    T = 1
    n_steps = 252
    n_paths = 100_000
    r = 0.02
    sigma = 0.2

    model = BlackScholes(r, sigma)
    scheme = Euler(model)
    engine = MonteCarlo(scheme)
    engine.configure(1, -1)

    call = Call(K_c, T)
    put = Put(K_p, T)

    state = MarketState(S = S0, r = r)

    p_engine = Pricer(state, n_steps, n_paths, engine)

    mc_call = p_engine.price(call)
    mc_put = p_engine.price(put)

    call_price = bs_call_price(S0, K_c, sigma, T, r)
    put_price = bs_put_price(S0, K_p, sigma, T, r)

    assert(mc_call == pytest.approx(call_price, rel = 0.01))
    assert(mc_put == pytest.approx(put_price, rel = 0.01))

def test_pricer_randomness():
    
    S0 = 100
    K = 95
    T = 1
    n_steps = 252
    n_paths = 10_000
    r = 0.02
    sigma = 0.2

    model = BlackScholes(r, sigma)
    scheme = Euler(model)
    engine1 = MonteCarlo(scheme)
    engine2 = MonteCarlo(scheme)
    engine3 = MonteCarlo(scheme)
    
    engine1.configure(1, -1)
    engine2.configure(1, -1)
    engine3.configure(2, -1)

    call = Call(K, T)
    
    state = MarketState(S = S0, r = r)

    p_engine1 = Pricer(state, n_steps, n_paths, engine1)
    p_engine2 = Pricer(state, n_steps, n_paths, engine2)
    p_engine3 = Pricer(state, n_steps, n_paths, engine3)
    engine_price1 = p_engine1.price(call)
    engine_price2 = p_engine2.price(call)
    engine_price3 = p_engine3.price(call)

    assert(engine_price1 == engine_price2)
    assert(engine_price1 != engine_price3)


def test_pricing_with_heston():

    S0 = 100
    K = 102
    T = 1.2
    n_steps = 252
    n_paths = 10_000
    r = 0.02
    sigma = 0.2

    call = Call(K, T)
    put = Put(K, T)
    model = Heston(
            mu = 0.02,
            kappa=1.5,
            theta=0.04,
            epsilon=0.24,
            rho=-0.5)

    mc = MonteCarlo(QE(model))
    
    engine = Pricer(MarketState(S0, r, v0 = 0.2), n_steps, n_paths, mc)

    call_price = engine.price(call)
    put_price = engine.price(put)

def make_inputs():
    s = [80, 90, 100, 110, 120]
    t = [0.2, 0.4, 0.8, 1.0]
    v = [
    [0.32, 0.26, 0.22, 0.25, 0.30],  # t = 0.2
    [0.30, 0.25, 0.21, 0.24, 0.28],  # t = 0.4
    [0.28, 0.24, 0.20, 0.23, 0.26],  # t = 0.8
    [0.26, 0.23, 0.19, 0.22, 0.25],  # t = 1.0
    ]
    return t, s, v
  
def test_pricing_with_dupire():

    S0 = 100
    K = 102
    T = 1.2
    n_steps = 252
    n_paths = 10_000
    r = 0.02
    sigma = 0.2

    call = Call(K, T)

    surface = LocalVolatilitySurface(*make_inputs())

    model = Dupire(r, 0.01, surface)
    mc = MonteCarlo(Euler(model))
    mc.configure(1,-1)

    marketstate = MarketState(S0, r)
    
    engine = Pricer(marketstate, n_steps, n_paths, mc)
    engine.price(call)


