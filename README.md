
# Quadratic Exponential (Andersen)
This project aims at implementing the QE scheme from Leif Andersen. The full paper this work is based on can be found [here](https://www.ressources-actuarielles.net/EXT/ISFA/1226.nsf/0/1826b88b152e65a7c12574b000347c74/$FILE/LeifAndersenHeston.pdf).

## Background

The Heston model assumes the following SDEs for the dynamics of the underlying asset and its volatility :

$$ \begin{cases}
dS_t =\mu S_t\space dt + \sqrt{v_t}S_T\space dW_t^{S} \\

dv_t = \kappa(\theta - v_t)dt + \epsilon \sqrt{v_t} dW_t^{v} \\

\langle dW_t^{(S)} dW_t^{(v)} \rangle = \rho dt
\end{cases}$$

with : 

$\kappa : \text{Mean-reversion speed}\\$
$\theta : \text{Long-term variance}\\$
$\epsilon : \text{Volatility of volatility}\\$
$\rho : \text{Spot/variance correlation}\\$
$v_t : \text{Instantaneous variance}\\$


A basic approach of simulating a Heston process over discrete time intervals is the log-Euler scheme. 

$$ 
\log(S_{t+\Delta}) = \log(S_t) +(r - \frac{1}{2} v_t) \Delta + \sqrt{v_t\Delta} Z_x\\

v_{t+\Delta} = v_t + \kappa(\theta-v_t)\Delta + \epsilon \sqrt{v_t\Delta} Z_v
$$

$$
Z_x, Z_v \sim \mathcal{N}(0,1)\\ 
Corr(Z_x, Z_v) = \rho
$$

Although computationally efficient, a recurrent problem of the Euler Scheme is that it can lead to $v_t$ going negative, making the computation of $\sqrt{v_t}$ impossible. A classic remedy to this issue is the truncation method, adjusting $v_t$ such that $v_{t+\Delta} = max(v_t, 0)$. This ensure numerical stability, but introduces a discretization bias. 

In ***Efficient Simulation of the Heston Stochastic Volatility Mode*** (2006), Leif Andersen offered another method for discretizing the Heston process. 