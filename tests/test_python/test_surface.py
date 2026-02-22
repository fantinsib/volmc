r"""
                 __                
 ___ _   _ _ __ / _| __ _  ___ ___ 
/ __| | | | '__| |_ / _` |/ __/ _ \
\__ \ |_| | |  |  _| (_| | (_|  __/
|___/\__,_|_|  |_|  \__,_|\___\___|

"""


import pytest
from volmc.types import LocalVolatilitySurface

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


def test_local_vol_surface_constructor():

    t,s,v = make_inputs()
    
    loc_vol = LocalVolatilitySurface(t,s,v)

    assert(isinstance(loc_vol, LocalVolatilitySurface))
    assert(loc_vol.sigma(t[0], s[0]) == v[0][0])
    assert(loc_vol.sigma(t[2], s[3]) == v[2][3])

def test_local_vol_surface_interpolation():
    
    t,s,v = make_inputs()
    
    loc_vol = LocalVolatilitySurface(t,s,v)
    assert(loc_vol.sigma(0.3, 80) == 0.31)
    assert(loc_vol.sigma(0.8, 95)==0.22)

def test_local_vol_surface_interp_oob():
    
    t,s,v = make_inputs()
    
    loc_vol = LocalVolatilitySurface(t,s,v)

    assert(loc_vol.sigma(0.2, 130)==0.3)
    assert(loc_vol.sigma(1.2, 120)==0.25)

def test_local_vol_surface_error_wrong_dim_v():
    
    t = [0.2,0.4,0.6]
    s = [100, 110, 120]
    v = [[0.3,0.27,0.3],
         [0.27, 0.26, 0.28]] #invalid dimensions 
    
    with pytest.raises(ValueError):
        loc_vol = LocalVolatilitySurface(t,s,v)

