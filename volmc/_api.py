from ._volmc import _State
from ._volmc import _Path
from ._volmc import _Model, _BlackScholes, _Heston
from ._volmc import _EulerHeston, _EulerBlackScholes, _QE, _Scheme
from ._volmc import _MonteCarlo


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

    def __repr__(self):
        return f"SimulationResult of {len(self.res.spot)} paths and {len(self.res.spot[0])} steps"
    
    def spot_values(self):
        """
        Returns a numpy matrix of the spot processes (one row = one process)
        """
        return self.res.spot
    
    def var_values(self):
        """
        Returns a numpy matrix of the variance processes (one row = one process)
        """
        return self.res.var

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

        super().__init__(mu = mu, sigma = sigma)

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

        super().__init__(mu = mu, kappa=kappa, theta=theta, epsilon=epsilon, rho=rho)

    def feller_condition(self):
        return self._feller_condition()
    
#--------------------------------SCHEMES

class Scheme(_Scheme):
    pass


class EulerBlackScholes(_EulerBlackScholes):
    
    def __init__(self, model):
        """
        Scheme 
        """
        super().__init__(model)


class EulerHeston(_EulerHeston):

    def __init__(self, model):
        super().__init__(model)

class QE(_QE):
    def __init__(self, model, psi_c = 1.5):
        super().__init__(model, psi_c)


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
        if (v0):
            sim_res = super()._generate(S0, n, T, n_paths, v0)
        else:
            sim_res = super()._generate(S0, n, T, n_paths)
        return SimulationResult(sim_res)
    
    def configure(self, seed: int):
        """
        Add configurations to the MonteCarlo engine.

        Parameters
        ----------
        seed : int
            The seed to be used for randomness
        """
        self._configure(seed)