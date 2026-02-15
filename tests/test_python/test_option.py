import pytest
from volmc import OptionContract, CallPayoff, PutPayoff, Instrument
from volmc import EulerBlackScholes, BlackScholes, MonteCarlo
import numpy as np


def test_option_constructor():

    K = 105
    S0 = 100
    r = 0.02
    sigma = 0.2
    T = 1

    contract = OptionContract(K, T)
    call_payoff = CallPayoff(K)

    call = Instrument(contract, call_payoff)

    bs = BlackScholes(r, sigma)
    mc = MonteCarlo(EulerBlackScholes(bs))

    simulation = mc.generate(S0, 252, T, 100)

    payoff = call.compute_payoff(simulation)

    print(payoff)
    


