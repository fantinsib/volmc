import pytest
from volmc import OptionContract, CallPayoff, PutPayoff, Instrument
from volmc import EulerBlackScholes, BlackScholes, MonteCarlo
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
    mc = MonteCarlo(EulerBlackScholes(bs))

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
    mc = MonteCarlo(EulerBlackScholes(bs))

    simulation = mc.generate(S0, 252, T, 100)

    payoff = call.compute_payoff(simulation)
    bs_price = bs_call_price(S0, K, sigma, T, r)

    assert(payoff*np.exp(-r*T) == pytest.approx(bs_price))


