import pytest
from volmc import BlackScholes, Heston, LocalVolatilitySurface, Dupire, Vasicek, Euler, EulerDupire, MonteCarlo, QE

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


def test_euler():
    bs = BlackScholes(0.02, 0.2)
    
    surface = LocalVolatilitySurface(*make_inputs())
    dupire = Dupire(0.04, 0.02, surface)

    vasicek = Vasicek(1.5, 0.05, 0.2)

    euler_bs = Euler(bs)
    euler_du = Euler(dupire)
    euler_va = Euler(vasicek)

    mc = MonteCarlo(euler_bs)
    mc.configure(1)
    sim = mc.generate(100, 100, 1, 10)
    assert(len(sim.spot_values()) == 10)
    assert(sim.spot_values()[0][0] == 100)
    assert(sim.spot_values()[0][-1] != 100)

    mc = MonteCarlo(euler_du)
    mc.configure(1)
    sim = mc.generate(100, 100, 1, 10)
    assert(len(sim.spot_values()) == 10)
    assert(sim.spot_values()[0][0] == 100)
    assert(sim.spot_values()[0][-1] != 100)

    mc = MonteCarlo(euler_va)
    mc.configure(1)
    sim = mc.generate(0.04, 100, 1, 10)
    assert(len(sim.spot_values()) == 10)
    assert(sim.spot_values()[0][0] == pytest.approx(0.04))
    assert(sim.spot_values()[0][-1] != 0.04)

def test_qe():
    heston = Heston(0.02, 1.5, 0.05, 0.2, -0.5)
    
    surface = LocalVolatilitySurface(*make_inputs())


    mc = MonteCarlo(QE(heston))
    mc.configure(1)
    sim = mc.generate(100, 252, 1, 10, 0.04)
    assert(len(sim.spot_values()) == 10)
    assert(sim.spot_values()[0][0] == 100)
    assert(sim.spot_values()[0][-1] != 100)