import pytest
from volmc.models import *
from volmc.types import LocalVolatilitySurface


def test_black_scholes_constructor_valid():

    bs = BlackScholes(0.01, 0.2)
    assert(isinstance(bs, BlackScholes))

    bs_zero = BlackScholes(0.01, 0.0)
    assert(isinstance(bs_zero, BlackScholes))


def test_black_scholes_constructor_invalid_sigma():

    with pytest.raises(ValueError):
        BlackScholes(0.01, -0.0001)


def test_heston_constructor_valid_params():

    h = Heston(0.02, 1.5, 0.04, 0.3, -0.7)
    assert(isinstance(h, Heston))

    Heston(0.02, 1.0, 0.04, 0.3, -1.0)
    Heston(0.02, 1.0, 0.04, 0.3, 1.0)


def test_heston_constructor_invalid_params():

    with pytest.raises(ValueError):
        Heston(0.02, 0.0, 0.04, 0.3, 0.0)

    with pytest.raises(ValueError):
        Heston(0.02, 1.0, 0.0, 0.3, 0.0)

    with pytest.raises(ValueError):
        Heston(0.02, 1.0, 0.04, 0.0, 0.0)

    with pytest.raises(ValueError):
        Heston(0.02, 1.0, 0.04, 0.3, -1.1)

    with pytest.raises(ValueError):
        Heston(0.02, 1.0, 0.04, 0.3, 1.1)


def test_heston_feller_condition_flags():

    h_ok = Heston(0.02, 2.0, 0.04, 0.3, 0.0)
    h_bad = Heston(0.02, 0.5, 0.01, 1.0, 0.0)

    assert(h_ok.feller_condition() == True)
    assert(h_bad.feller_condition() == False)

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


def test_dupire_constructor():
    t,s, v = make_inputs()

    surface = LocalVolatilitySurface(t,s,v)

    r = 0.03
    q = 0.01

    dupire = Dupire(r,q,surface)

def test_vasicek_constructor():

    vasicek = Vasicek(1.5, 0.05, 0.1)

    assert(isinstance(vasicek, Vasicek))