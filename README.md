
# Quadratic Exponential (Andersen)
This project aims at implementing the QE scheme from Leif Andersen. The full paper this work is based on can be found [here](https://www.ressources-actuarielles.net/EXT/ISFA/1226.nsf/0/1826b88b152e65a7c12574b000347c74/$FILE/LeifAndersenHeston.pdf).

## Background
### The Heston Model
The Heston model assumes the following SDEs for the dynamics of the underlying asset and its volatility :

$$dS_t = \mu S_t\,dt + \sqrt{v_t}\,S_t\,dW_t^{S},$$


$$dv_t = \kappa(\theta - v_t)\,dt + \epsilon \sqrt{v_t}\,dW_t^{v},$$


$$\langle dW_t^{S}, dW_t^{v} \rangle = \rho\,dt.$$

with : 

$\kappa : \text{Mean-reversion speed}$

$\theta : \text{Long-term variance}$

$\epsilon : \text{Volatility of volatility}$

$\rho : \text{Spot/variance correlation}$

$v_t : \text{Instantaneous variance}$

One of the challenge that arises when simulating a Heston process is the passing from continuous to discrete time intervals. While in the $v_t$ process, the Feller condition ($2\kappa \theta \ge \epsilon^2$) ensures that $v_t$ stays positive at all $t$, in discrete times nothing holds variance from going negative.


The objective is therefore to find a numerical scheme that allows to simulate a Heston model, ensuring :

• $v_t \ge 0 \space \forall \space t \in [0,T]$

• Limit the bias introduced in the model

Some scheme allow an unequal tradeoff between those requirements :

---
### The Euler Scheme
A basic approach of simulating a Heston process over discrete time intervals is the log-Euler scheme. 

$$ 
\log(S_{t+\Delta}) = \log(S_t) +(r - \frac{1}{2} v_t) \Delta + \sqrt{v_t\Delta} Z_x $$

$$v_{t+\Delta} = v_t + \kappa(\theta-v_t)\Delta + \epsilon \sqrt{v_t\Delta} Z_v
$$

$$
Z_x, Z_v \sim \mathcal{N}(0,1)\\ 
Corr(Z_x, Z_v) = \rho
$$

Although computationally efficient, a recurrent problem of the Euler Scheme is that it can lead to $v_t$ going negative, making the computation of $\sqrt{v_t}$ impossible. A classic remedy to this issue is the truncation method, adjusting $v_t$ such that $v_{t+\Delta} = max(v_t, 0)$. This ensure numerical stability, but introduces a discretization bias. 

___

### Broadie-Kaya scheme 

In ***Exact Simulation of Stochastic Volatility and
Other Affine Jump Diffusion Processes*** (2004), Mark Broadie and Ösgür Kaya proposed a method to sample directly $v_{t+\Delta}$ from its distribution. This result in a bias-free simulation of the path of $v$, but its computational complexity make it difficult to use for Monte-Carlo simulation. 

___

### Quadratic Exponential scheme

In ***Efficient Simulation of the Heston Stochastic Volatility Mode*** (2006), Leif Andersen offered another method for discretizing the Heston process. 

**• The non-central chi-squared distribution**

Andersen starts from the fact that the process followed by volatility in the Heston model is derived from the CIR interest rate model. This model follows a **non-central chi-squared distribution**, noted $\chi^{'2}(\upsilon, \lambda)$ with $\upsilon$ the degree of freedom and $\lambda$ the non-centrality parameter. For a process $X$ that follows :

$$dX_t = \kappa(\theta - X_t)dt + \sigma \sqrt{X_t} dW_t$$

the R.V $X_t \space | \space X_s$ for $t>s$ therefore follows :

$$ X_t \sim c\space\chi^{'2}(\upsilon, \lambda) $$

with 

$$ \upsilon = \frac{4\kappa\theta}{\epsilon^2} $$

$$ \lambda = \frac{4\kappa e^{-\kappa(t-s)}}{\epsilon^2(1-e^{-\kappa(t-s)})}$$

and has moments :

$$E(X_t | X_s) = \theta + (X_s - \theta)e^{-\kappa(t-s)}$$

$$Var(X_t | X_s) = \frac{X_s\space\epsilon^2 e^{-\kappa(t-s)}}{\kappa} \left(1 -e^{-\kappa(t-s)}\right) + \frac{\theta\epsilon^2}{2\kappa}\left(1 -e^{-\kappa(t-s)}\right)^2 $$

**• Andersen idea**

Andersen offers to approximate $X_t | X_s \sim \chi^{'2}$ through two different regimes. We define the ratio $\psi$ as : 

$$ \psi = \frac{Var(X_t | X_s)}{E(X_t | X_s)^{2}}$$

which measures the relative dispersion of the future variance and serves as the **regime selection criteria**. When $\psi \leq 1.5$, $X_t$ can be approximated by the **quadratic regime** :

$$X_t =a(Z + b)^2$$
$$-$$

$$Z\sim \mathcal{N}(0,1)$$

$$a = \frac{m}{1+b^2}$$

$$b^2 = \frac{2}{\psi} - 1 + \sqrt{\frac{2}{\psi}\left(\frac{2}{\psi}-1\right)}$$

When $\psi > 1.5$, $X_t$ is approximated by the **exponential regime** : 

$$ if \space U \leq p \space :\space X_t = 0$$

$$if \space U > p \space :\space X_t = -\frac{1}{\beta}\ln(\frac{1-U}{1-p})$$

$$-$$

$$ U \sim \mathcal{U}(0,1)$$
$$ p = \frac{\psi -1}{\psi +1}$$
$$\beta = \frac{1-p}{m}$$
