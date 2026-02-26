from __future__ import annotations
from ._volmc import _State
from ._volmc import _Path
from ._volmc import _Model, _BlackScholes, _Heston, _Dupire, _Vasicek
from ._volmc import _EulerHeston, _QE, _Scheme, _Euler
from ._volmc import _MonteCarlo
from ._volmc import _LocalVolatilitySurface
from ._volmc import _OptionContract, _Payoff, _PutPayoff, _CallPayoff, _DigitalCallPayoff,_DigitalPutPayoff, _Instrument, _Barrier, _Direction, _Nature
from ._volmc import _Pricer, _MarketState

from dataclasses import dataclass
from typing import TYPE_CHECKING

if TYPE_CHECKING:
    import numpy

#--------------------------------types

class State:
    def __init__(self, cpp_State):
        """
        State of the spot and the volatility at a time t.

        Returns
        -------
        State
        """
        self._s = cpp_State

    def __repr__(self):
        return f"State(S={self._s._spot():.4f}, V = {self._s._vol():.4f})"
    
    def __str__(self):
        return f"State(S={self._s._spot():.4f}, V = {self._s._vol():.4f})"
    
    def spot(self):
        """
        Returns the spot value of the current state.
        """
        self._s.spot()
    
    def vol(self):
        """
        Returns the vol value of the current state.
        """

        self._s.vol()

class Path:
    def __init__(self, cpp_Path):
        """
        Path followed by the spot and volatility over time t.

        Returns
        -------
        Path : list of State
        """
        self._p = cpp_Path
    
    def __len__(self):
        return self._p.__len__()

    def __repr__(self):
        return f"Path : n_steps = {len(self)}"
    
    def end_state(self):
        """
        Returns the end state of the path. 

        Returns
        -------
        State
        """
        s = self._p.end_state()
        return State(s)

    def __getitem__(self, i):
        if isinstance(i, slice):
            rng = range(*i.indices(len(self)))
            return [State(self._p.at(k)) for k in rng]

    def spot(self):
        return [(self._p.at(k)._spot()) for k in range(len(self))]
    
    def vol(self):
        return [(self._p.at(k)._vol()) for k in range(len(self))]
    
class SimulationResult:
    def __init__(self, cpp_simres):
        """
        Result of a MonteCarlo simulation
        """
        self.res = cpp_simres
        self.n_path = len(self.res.spot)
        self.n_steps = len(self.res.spot[0])

    def __repr__(self):
        return f"SimulationResult of {len(self.res.spot)} paths and {len(self.res.spot[0])} steps"
    
    def spot_values(self):
        """
        Returns a numpy matrix of the spot processes (one row = one process)
        """
        return self.res.spot
    
    def vol_values(self):
        """
        Returns a numpy matrix of the variance processes (one row = one process)
        """
        return self.res.vol
    
    def mean_terminal_spot(self):
        """
        Returns the mean final value of the spot processes
        """
        return (self.res.spot[:,-1]).sum()/self.n_path

#--------------------------------MODELS

class Model(_Model):
    pass

class BlackScholes(_BlackScholes):
    def __init__(self, mu:float, sigma:float):
        """
        Creates a BlackScholes model.

        Parameters
        ----------
        mu : float
            The drift of the asset.
        sigma : float 
            The volatility.
        """
        self._mu = mu
        self._sigma = sigma

        super().__init__(mu = mu, sigma = sigma)

    def __str__(self):
        return f"""
BLACK-SCHOLES MODEL
-------------------
mu : {self._mu}
sigma : {self._sigma}
"""

class Heston(_Heston):
    def __init__(self, 
                mu:float, 
                kappa:float,
                theta: float,
                epsilon: float,
                rho: float):
        """
        Creates a Heston model.

        Parameters
        ----------
        mu : float
            The drift.
        kappa : float
            Mean-reversion speed.
        theta : float
            Long-term variance.
        epsilon : float
            Vol-of-vol.
        rho : float
            Spot/Vol brownian correlation
        """

        self._mu = mu
        self._kappa = kappa
        self._thetha = theta 
        self._epsilon = epsilon
        self._rho = rho
        super().__init__(mu = mu, kappa=kappa, theta=theta, epsilon=epsilon, rho=rho)

    def feller_condition(self):
        return self._feller_condition()
    
    def __str__(self):
        return f"""
HESTON MODEL  
------------
mu : {self._mu} 
kappa : {self._kappa}
theta : {self._thetha} 
epsilon : {self._epsilon} 
rho : {self._rho}
        """
    
class Dupire(_Dupire):
    def __init__(self, r : float, q : float, local_volatility_surface : LocalVolatilitySurface):
        """
        Creates a Dupire model.

        Parameters
        ----------
        r : float
            The risk-free rate.
        q : float
            The dividend yield
        loc_vol_surface : LocalVolatilitySurface
            The local volatility surface.
        """
        self._q = q
        self._r = r
        super().__init__(r, q, local_volatility_surface)

    def __str__(self):
        return f"""
DUPIRE MODEL 
------------
r : {self._r} 
q : {self._q}
"""
    
class Vasicek(_Vasicek):
    def __init__(self, a: float, b: float, sigma: float):
        """
        Creates a Vasicek model

        Parameters
        ----------
        a : float
            Speed of reversion to the mean
        b : float
            Long-term mean rate
        sigma : float
            The volatility
        """
        self.a = a
        self.b = b
        self.sigma = sigma
        super().__init__(a, b, sigma)

    def __str__(self):
        return f"""
VASICEK MODEL 
------------
a : {self._a} 
b : {self._b}
sigma : {self._sigma}
"""


#--------------------------------SCHEMES

class Scheme(_Scheme):
    pass


class EulerHeston(_EulerHeston):

    def __init__(self, model):
        super().__init__(model)

class QE(_QE):
    def __init__(self, model, psi_c = 1.5):
        super().__init__(model, psi_c)


class Euler(_Euler):
    def __init__(self, model):
        """
        Generic Euler discretization

        Parameters
        ----------
        model : Model
            A model with a drift and diffusion process.
        """
        super().__init__(model)


#--------------------------------Engine

class MonteCarlo(_MonteCarlo):

    def __init__(self, scheme):
        super().__init__(scheme)

    def simulate_path(self, S0: float, n: int, T: float, v0: float | None = None):
        """
        Simulate one trajectory of an asset.

        Parameters
        ----------
        S0 : float
            Initial spot 
        n : int
            Number of steps of each path
        T : float
            Time interval
        v0 : float
            Initial volatility 

        Returns
        -------
        Path
        """
        cpp_path = super()._simulate_path(S0, n, T, v0)
        return Path(cpp_path)

    def generate(self, S0: float, n: int, T: float, n_paths: int, v0: float | None = None):
        """
        Returns a MonteCarlo simulation

        Parameters
        ----------
        S0 : float
            Initial spot 
        n : int
            Number of steps of each path
        T : float
            Time interval
        n_paths : int
            Number of paths to simulate
        v0 : float | None
            Initial volatility 

        Returns
        -------
        SimulationResult
        """
        if (v0 is not None):
            sim_res = super()._generate(S0, n, T, n_paths, v0)
        else:
            sim_res = super()._generate(S0, n, T, n_paths)
        return SimulationResult(sim_res)
    
    def configure(self, seed: int | None = None, n_jobs: int | None = None):
        """
        Add configurations to the MonteCarlo engine.

        Parameters
        ----------
        seed : int
            The seed to be used for randomness
        """
        self._configure(seed, n_jobs, None)


class LocalVolatilitySurface(_LocalVolatilitySurface):
    def __init__(self, 
                times : numpy.ndarray, 
                spots : numpy.ndarray, 
                volatility : numpy.ndarray):
        """
        Creates a local volatility surface.

        Parameters
        ----------
        times : numpy.ndarray
            The vector of times in years of the surface
        spots : numpy.ndarray
            The vector of spots of the surface
        volatility : the times * spots matrix of corresponding local volatilities
        """
    
        return super().__init__(times, spots, volatility)
    
    def sigma(self, t : float, S : float):
        """
        Computes the interpolated local volatility at time t for spot S.

        Parameters
        ----------
        t : float
            Time in years.
        S : float
            Spot price.

        Returns
        -------
        float 
            The interpolated local volatility
        """
        return self._sigma(t, S)
    
#-------------------------------- Options

class OptionContract(_OptionContract):

    def __init__(self, K : float, T: float):
        """
        Term sheet for an option contract 

        Parameters
        ----------
        K : float
            The strike price
        T : float 
            Time to maturity in years
        """
        super().__init__(K, T)
    
class CallPayoff(_CallPayoff):
    
    def __init__(self):
        """ 
        Payoff for a European call option

        Parameters
        ----------
        K : float 
            The strike price
        """
        super().__init__()

    
class PutPayoff(_PutPayoff):
    
    def __init__(self):
        """ 
        Payoff for a European put option

        Parameters
        ----------
        K : float 
            The strike price
        """
        super().__init__()

class DigitalPutPayoff(_DigitalPutPayoff):
    
    def __init__(self):
        """ 
        Payoff for a digital put option

        Parameters
        ----------
        K : float 
            The strike price
        """
        super().__init__()

    
class DigitalCallPayoff(_DigitalCallPayoff):
    
    def __init__(self):
        """ 
        Payoff for a digital call option

        Parameters
        ----------
        K : float 
            The strike price
        """
        super().__init__()

class Barrier(_Barrier):
    def __init__(self, H : float, direction : str, nature : str, payoff : Payoff):
        """
        Payoff for a barrier option

        Parameters
        ----------
        H : float
            The barrier value
        direction : str
            The direction from which the barrier is hit. Must be "up" or "down"
        nature : str
            The nature of the barrier. Must be "in" or "out"
        payoff : Payoff
            The payoff to be activated or deactivated if the barrier is hit 
        """
        if (direction.lower() == "up"):
            _dir = _Direction.Up
        elif (direction.lower() == "down"):
            _dir = _Direction.Down
        else:
            raise ValueError(f"Barrier : the direction must be 'up' or 'down', received {direction}.")
        
        if (nature.lower() == "in"):
            _nat = _Nature.In
        elif (nature.lower() == "out"):
            _nat = _Nature.Out
        else:
            raise ValueError(f"Barrier : the nature must be 'in' or 'out', received {nature}.")
        
        super().__init__(H, _dir, _nat, payoff)

class Instrument(_Instrument):
    def __init__(self, contract : OptionContract, payoff : Payoff):
        """
        Financial instrument. 

        Parameters
        ----------
        contract : OptionContract
            The term sheet
        payoff : Payoff
            The payoff of the instrument
        """

        super().__init__(contract, payoff)

    def compute_payoff(self, results: SimulationResult) -> float:
        """
        Returns the average payoff across all paths in a MonteCarlo SimulationResult.
        """
        return self._compute_payoff(results.res)

    # Backward-compatible alias
    def compute(self, results: SimulationResult) -> float:
        return self.compute_payoff(results)

def Call(K : float, T : float):
    """
    Creates a call option instrument. 

    Parameters
    ----------
    K : float
        The strike price
    T : float 
        The time to maturity in years
    """
    return Instrument(OptionContract(K, T), CallPayoff())

def Put(K : float, T : float):
    """
    Creates a put option instrument. 
    
    Parameters
    ----------
    K : float
        The strike price
    T : float 
        The time to maturity in years
    """
    return Instrument(OptionContract(K, T), PutPayoff()) 

def DigitalCall(K : float, T : float):
    """
    Creates a put option instrument. 
    
    Parameters
    ----------
    K : float
        The strike price
    T : float 
        The time to maturity in years
    """
    return Instrument(OptionContract(K, T), DigitalCallPayoff()) 

def DigitalPut(K : float, T : float):
    """
    Creates a put option instrument. 
    
    Parameters
    ----------
    K : float
        The strike price
    T : float 
        The time to maturity in years
    """
    return Instrument(OptionContract(K, T), DigitalPutPayoff())

def BlackScholesEngine(mu : float, sigma : float):
    """
    Returns a Monte Carlo engine with a Euler discretization of BlackScholes model

    Parameters
    ----------
    mu : float
        The drift rate for the Black Scholes model
    sigma : float
        The volatility for the Black Scholes model
    """
    model = BlackScholes(mu, sigma)
    scheme = Euler(model)
    return MonteCarlo(scheme)

def HestonEngine(mu : float, kappa : float, theta : float, epsilon : float, rho : float, scheme : str = "qe"):
    """
    Returns a Monte Carlo engine configured with a Heston model

    Parameters
    ----------
    mu : float
        The drift rate for the Heston model
    kappa : float
        The mean-reversion speed of the Heston model
    theta : float 
        The long-term variance of the Heston model
    epsilon : float 
        The vol-of-vol
    rho : float
        Spot/vol brownian correlation of the Heston model 
    scheme : str
        The scheme to use in ["qe", "euler"]
    
    """
    if scheme.strip().lower() not in ["qe", "euler"]:
        raise ValueError(f"HestonEngine : the scheme for a Heston model must be in ['qe', 'euler']. Received {scheme}")
    
    model = Heston(mu, kappa, theta, epsilon, rho)

    if scheme == "qe":
        return MonteCarlo(QE(model))
    if scheme == "euler":
        return MonteCarlo(Euler(model))




class MarketState(_MarketState):
    def __init__(self, S: float, r: float, v0 : float | None = None):
        """
        Representation of the state of the market at pricing time

        Parameters
        ----------
        S : float
            Spot price
        r : float
            Risk free rate
        v0 : float | None
            Optional initial volatility if required by the pricing engine
        """
        super().__init__(S, r, v0)

class Pricer(_Pricer):
    def __init__(self, marketstate : MarketState, n_steps : int, n_paths : int, engine : MonteCarlo):
        """
        Pricing engine for a financial instrument.

        Parameters
        ----------
        marketstate : MarketState
            The state of the market at time of pricing
        n_steps : int
            The number of steps in each path
        n_paths : int
            The number of paths to generate
        engine : MonteCarlo
            The Monte Carlo engine to be used for pricing
        """
        super().__init__(marketstate, n_steps, n_paths, engine)

    def price(self, instrument : Instrument):
        """
        Returns the Monte Carlo simulation price

        Parameters
        ----------
        instrument : Instrument
            The instrument to price
        """
        return self._compute_price(instrument)

    def delta(self, instrument : Instrument, h : float):
        """
        Computes the delta using bump-and-revalue.

        Parameters
        ----------
        instrument : Instrument
            The instrument to price
        h : float
            The finite difference bump
        """
        return self._delta(instrument, h)
    
    def gamma(self, instrument : Instrument, h : float):
        """
        Computes the gamma using bump-and-revalue.

        Parameters
        ----------
        instrument : Instrument
            The instrument to price
        h : float
            The finite difference bump
        """
        return self._gamma(instrument, h)

    def reconfigure(self, n_steps : int = None, n_paths : int = None, marketstate : MarketState = None):
        """
        Change the parameters of the pricing engine.

        Parameters
        ----------
        n_steps : int
            The number of steps to generates
        n_paths : int 
            The number of paths to generate
        marketstate : MarketState
            A marketstate
        """
        self._reconfigure(n_steps, n_paths, marketstate)