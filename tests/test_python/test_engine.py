import pytest
from volmc import Path, State, SimulationResult
from volmc import Heston, BlackScholes
from volmc import EulerHeston, EulerBlackScholes, QE
from volmc import MonteCarlo



def test_basic_monte_carlo_euler_bs():

    bs = BlackScholes(0.02, 0.15)
    euler = EulerBlackScholes(bs)
    montecarlo = MonteCarlo(euler)

    montecarlo.configure(seed=1)
    sim = montecarlo.generate(100, 252, 1, 10)

    assert(isinstance(sim, SimulationResult))
    assert(len(sim.spot_values()) == 10)
    assert(len(sim.spot_values()[0]) == 253)
    
    
    with pytest.raises(RuntimeError):
        sim.var_values() #no variance values are returned for the simulation of a BS spot process

    for path in sim.spot_values():
        assert path[0] == 100
        assert path[-1] != 100

def test_monte_carlo_euler_bs_randomness():

    bs = BlackScholes(0.02, 0.15)
    euler = EulerBlackScholes(bs)

    montecarlo1 = MonteCarlo(euler)
    montecarlo2 = MonteCarlo(euler)
    montecarlo3 = MonteCarlo(euler)

    montecarlo1.configure(seed=1)
    montecarlo2.configure(seed=1)
    montecarlo3.configure(seed=2)

    s1 = montecarlo1.generate(100, 252, 1, 10).spot_values()
    s2 = montecarlo2.generate(100, 252, 1, 10).spot_values()
    s3 = montecarlo3.generate(100, 252, 1, 10).spot_values()

    for path in range(len(s1)):
        for step in range(len(s1[0])):
            if step == 0: continue
            assert(s1[path][step] == s2[path][step])
            assert(s1[path][step] != s3[path][step])