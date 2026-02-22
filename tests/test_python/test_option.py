import pytest
from volmc.options import *
from volmc.models import BlackScholes
from volmc.schemes import Euler
from volmc.pricing import MonteCarlo
import numpy as np



def norm_cdf(x):
    return 0.5 * (1 + np.sign(x) * np.sqrt(1 - np.exp(-2*x**2/np.pi)))

def bs_call_price(S, K, sigma, T, r):
    d1 = (np.log(S/K) + ((sigma**2)/2 + r)*T)/(sigma*np.sqrt(T))
    d2 = d1 - np.sqrt(T)*sigma

    return S* norm_cdf(d1) - K*np.exp(-r*T)*norm_cdf(d2)

def test_option_constructor():

    K = 105
    S0 = 100
    r = 0.02
    sigma = 0.2
    T = 1

    contract = OptionContract(K, T)
    call_payoff = CallPayoff()

    call = Instrument(contract, call_payoff)

    bs = BlackScholes(r, sigma)
    mc = MonteCarlo(Euler(bs))

    simulation = mc.generate(S0, 252, T, 100)

    payoff = call.compute_payoff(simulation)

    
def test_basic_pricing():

    K = 100
    S0 = 100
    r = 0.02
    sigma = 0
    T = 1

    contract = OptionContract(K, T)
    call_payoff = CallPayoff()

    call = Instrument(contract, call_payoff)

    bs = BlackScholes(r, sigma)
    mc = MonteCarlo(Euler(bs))

    simulation = mc.generate(S0, 252, T, 1000)

    payoff = call.compute_payoff(simulation)
    bs_price = bs_call_price(S0, K, sigma, T, r)

    assert(payoff*np.exp(-r*T) == pytest.approx(bs_price, rel=0.001))

def test_barrier():
    K = 102
    S0 = 100
    r = 0.02
    sigma = 0
    T = 1

    contract = OptionContract(K, T)
    barr_payoff = Barrier(101, "up", "in", CallPayoff())

    up_in_call = Instrument(contract, barr_payoff)

    bs = BlackScholes(r, sigma)
    mc = MonteCarlo(Euler(bs))
    mc.configure(1, -1)
    simulation = mc.generate(S0, 252, T, 100)

    payoff = up_in_call.compute_payoff(simulation)
    assert(payoff > 0)

def test_barrier_error():
    K = 102
    S0 = 100
    r = 0.02
    sigma = 0
    T = 1

    contract = OptionContract(K, T)
    barr_payoff_ok = Barrier(101, "Up", "iN", CallPayoff())

    with pytest.raises(ValueError):
        Barrier(101, "in", 'out', CallPayoff())

def test_barrier_payoff_dep_on_H():
    K = 120
    H1 = 105
    H2 = 140
    S0 = 100
    r = 0.05
    sigma = 0.2
    T = 1.4

    contract = OptionContract(K, T)
    barr_payoff1 = Barrier(H1, "up", "out", CallPayoff())
    barr_payoff2 = Barrier(H2, "up", "out", CallPayoff())

    up_out_call1 = Instrument(contract, barr_payoff1)
    up_out_call2 = Instrument(contract, barr_payoff2)

    bs = BlackScholes(r, sigma)
    mc = MonteCarlo(Euler(bs))
    mc.configure(1, -1)
    simulation = mc.generate(S0, 252, T, 100)

    payoff1 = up_out_call1.compute_payoff(simulation)
    payoff2 = up_out_call2.compute_payoff(simulation)
    assert(payoff1 != payoff2)

def test_barrier_parity():


    K = 120
    H = 110
    S0 = 100
    r = 0.02
    sigma = 0.15
    T = 1.4

    call_price = bs_call_price(S0, K, sigma, T, r)
    contract = OptionContract(K,T)
    up_in_pay = Barrier(H, "up", "in", CallPayoff())
    up_out_pay = Barrier(H, "up", "out", CallPayoff())
    vanilla = Instrument(contract, CallPayoff())
    up_in = Instrument(contract, up_in_pay)
    up_out = Instrument(contract, up_out_pay)

    bs = BlackScholes(r, sigma)
    mc = MonteCarlo(Euler(bs))
    mc.configure(1, -1)
    
    simulation = mc.generate(S0, 500, T, 10_000)
    
    payoff_up_in = up_in.compute_payoff(simulation)
    payoff_up_out = up_out.compute_payoff(simulation)
    payoff_vanilla = vanilla.compute_payoff(simulation)

    assert(payoff_vanilla == pytest.approx((payoff_up_in+payoff_up_out)))
    
