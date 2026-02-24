import pytest
import numpy as np

from volmc.types import *
from volmc.models import Heston, BlackScholes, Dupire
from volmc.schemes import Euler, EulerHeston, QE
from volmc.pricing import MonteCarlo

def test_basic_monte_carlo_euler_bs():

    bs = BlackScholes(0.02, 0.15)
    euler = Euler(bs)
    montecarlo = MonteCarlo(euler)

    montecarlo.configure(seed=1)
    sim = montecarlo.generate(100, 252, 1, 10)

    assert(isinstance(sim, SimulationResult))
    assert(len(sim.spot_values()) == 10)
    assert(len(sim.spot_values()[0]) == 253)


    for path in sim.spot_values():
        assert path[0] == 100
        assert path[-1] != 100

def test_monte_carlo_euler_bs_randomness():

    bs = BlackScholes(0.02, 0.15)
    euler = Euler(bs)

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


def test_monte_carlo_euler_heston_with_v0():

    heston = Heston(0.02, 1.5, 0.04, 0.3, -0.7)
    euler = EulerHeston(heston)
    montecarlo = MonteCarlo(euler)

    montecarlo.configure(seed=7)
    sim = montecarlo.generate(100, 12, 1, 5, 0.04)

    assert(isinstance(sim, SimulationResult))
    assert(len(sim.spot_values()) == 5)
    assert(len(sim.spot_values()[0]) == 13)

    
    assert(len(sim.vol_values()) == 5)
    assert(len(sim.vol_values()[0]) == 13)

    for path in sim.spot_values():
        assert path[0] == 100

    #ensures that the specified initial vol for Heston model is the first of the vol path
    assert(sim.vol_values()[0][0] == pytest.approx(0.04)) 


def test_monte_carlo_qe_heston_with_v0():

    heston = Heston(0.02, 2.0, 0.04, 0.25, 0.3)
    qe = QE(heston, psi_c=1.5)
    montecarlo = MonteCarlo(qe)

    montecarlo.configure(seed=3)
    sim = montecarlo.generate(100, 8, 1, 4, 0.04)

    assert(isinstance(sim, SimulationResult))
    
    
    assert(len(sim.spot_values()) == 4)
    assert(len(sim.spot_values()[0]) == 9)
    
    assert(len(sim.vol_values()) == 4)
    assert(len(sim.vol_values()[0]) == 9)


def test_monte_carlo_heston_requires_v0():

    heston = Heston(0.02, 1.5, 0.04, 0.3, -0.7)


    with pytest.raises(ValueError):
        MonteCarlo(EulerHeston(heston)).generate(100, 12, 1, 3)


def test_monte_carlo_zero_time_raises():

    bs = BlackScholes(0.02, 0.15)
    montecarlo = MonteCarlo(Euler(bs))

    with pytest.raises(ValueError):
        montecarlo.generate(100, 12, 0, 3)
        

def test_model_parameter_validation():

    with pytest.raises(ValueError):
        BlackScholes(0.02, -0.1)

    with pytest.raises(ValueError):
        Heston(0.02, 0.0, 0.04, 0.3, 0.0)

    with pytest.raises(ValueError):
        Heston(0.02, 1.0, 0.0, 0.3, 0.0)

    with pytest.raises(ValueError):
        Heston(0.02, 1.0, 0.04, 0.0, 0.0)

    with pytest.raises(ValueError):
        Heston(0.02, 1.0, 0.04, 0.3, 1.5)


def test_qe_psi_threshold_bounds():

    heston = Heston(0.02, 1.5, 0.04, 0.3, -0.7)

    with pytest.raises(ValueError):
        QE(heston, psi_c=0.9)

    with pytest.raises(ValueError):
        QE(heston, psi_c=2.1)

    QE(heston, psi_c=1.0)
    QE(heston, psi_c=2.0)


def test_monte_carlo_negative_seed_raises():

    bs = BlackScholes(0.02, 0.15)
    montecarlo = MonteCarlo(Euler(bs))

    with pytest.raises(ValueError):
        montecarlo.configure(seed=-1)


def test_heston_feller_condition():

    heston_ok = Heston(0.02, 2.0, 0.04, 0.3, 0.0)
    heston_bad = Heston(0.02, 0.5, 0.01, 1.0, 0.0)

    assert(heston_ok.feller_condition() == True)
    assert(heston_bad.feller_condition() == False)


def test_mc_with_mt():

    heston = Heston(0.02, 2.0, 0.04, 0.3, -0.6)
    mc = MonteCarlo(QE(heston))

    mc.configure(1, -1)

    simulation = mc.generate(100, 252, 1, 1000, 0.2)

    assert(simulation.mean_terminal_spot() != 100)
    assert(simulation.n_path == 1000)
    assert(simulation.n_steps == 253)
    assert(len(simulation.spot_values()) == simulation.n_path)

def test_mc_with_mt_randomness():
        
    heston = Heston(0.02, 2.0, 0.04, 0.3, -0.6)
    qe = QE(heston)

    montecarlo1 = MonteCarlo(qe)
    montecarlo2 = MonteCarlo(qe)
    montecarlo3 = MonteCarlo(qe)

    montecarlo1.configure(seed=1)
    montecarlo2.configure(seed=1)
    montecarlo3.configure(seed=2)

    s1 = montecarlo1.generate(S0 = 100, v0 = 0.2, n= 252, T=1, n_paths= 10).spot_values()
    s2 = montecarlo2.generate(S0 = 100, v0 = 0.2, n= 252, T=1, n_paths= 10).spot_values()
    s3 = montecarlo3.generate(S0 = 100, v0 = 0.2, n= 252, T=1, n_paths= 10).spot_values()

    for path in range(len(s1)):
        for step in range(len(s1[0])):
            if step == 0: continue
            assert(s1[path][step] == s2[path][step])
            assert(s1[path][step] != s3[path][step])

def test_mc_dupire():

    s = [80, 90, 100, 110, 120]
    t = [0.2, 0.4, 0.8, 1.0]
    v = [
    [0.32, 0.26, 0.22, 0.25, 0.30],  # t = 0.2
    [0.30, 0.25, 0.21, 0.24, 0.28],  # t = 0.4
    [0.28, 0.24, 0.20, 0.23, 0.26],  # t = 0.8
    [0.26, 0.23, 0.19, 0.22, 0.25],  # t = 1.0
    ]

    surface = LocalVolatilitySurface(t, s, v)

    r = 0.04
    q = 0.01

    dupire = Dupire(r,q, surface)

    euler_dupire = Euler(dupire)

    mc = MonteCarlo(euler_dupire)

    sim = mc.generate(100, 252, 1, 10)

    assert(isinstance(sim, SimulationResult))
    assert(sim.mean_terminal_spot() != 100)
    assert(sim.n_path == 10)
    assert(sim.n_steps == 253)
    
    S = sim.spot_values()
    assert np.all(S > 0)

