# Dynamic Statistical Arbitrage via Kalman Filtering

## 1. Executive Summary

This repository hosts a production-grade algorithmic trading system implementing a **Dynamic Statistical Arbitrage** strategy. Unlike traditional pairs trading approaches that rely on static cointegration tests (e.g., Engle-Granger or Johansen), this system utilizes a **Linear Gaussian State-Space Model (LGSSM)** estimated via a recursive **Kalman Filter**.

The core objective is to capture mean-reverting opportunities between two correlated assets while adapting dynamically to structural market breaks. The algorithm estimates the time-varying hedge ratio ($\beta_t$) and intercept ($\alpha_t$) in real-time to generate a standardized signal ($Z-Score$).

**Performance Metrics (Backtest Period: 2015-2024):**
* **Asset Pair:** iShares MSCI Australia (EWA) vs. iShares MSCI Canada (EWC).
* **Strategy Logic:** Mean Reversion on Kalman-derived residuals.
* **Sharpe Ratio:** 0.66 (Unlevered).
* **Total Return:** +40.90% (Unlevered, Cash-Efficiency Adjusted).
* **Transaction Cost Model:** Slippage and commission simulation included implicitly via conservative execution prices.

---

## 2. Theoretical Framework

### 2.1. State-Space Representation
The relationship between the dependent asset price ($y_t$) and the independent asset price ($x_t$) is modeled as:

$$
y_t = \alpha_t + \beta_t x_t + \epsilon_t, \quad \epsilon_t \sim \mathcal{N}(0, R)
$$

The state vector $\theta_t = [\alpha_t, \beta_t]^T$ is assumed to follow a Random Walk process to allow for parameter drift (time-varying cointegration):

$$
\theta_t = \theta_{t-1} + \omega_t, \quad \omega_t \sim \mathcal{N}(0, Q)
$$

Where:
* $R$ is the measurement noise variance.
* $Q$ is the process noise covariance matrix, controlled by the delta parameter ($\delta$).

### 2.2. Bayesian Estimation
The system implements the Kalman Filter predict-update cycle at each time step $t$:

1.  **Prediction:** Projects the state estimate and covariance a priori.
2.  **Innovation:** Calculates the measurement residual $e_t = y_t - \hat{y}_{t|t-1}$.
3.  **Correction:** Updates the state parameters using the optimal Kalman Gain ($K_t$).

The trading signal is derived from the standardized innovation (Z-Score):
$$
Z_t = \frac{e_t}{\sqrt{S_t}}
$$
Where $S_t$ is the covariance of the innovation.

---

## 3. Detailed System Architecture

The project follows strict separation of concerns, isolating mathematical computation from trading logic and data management.

```text
Kalman-StatArb-Project/
├── data/                           # DATA STORAGE LAYER
│   ├── raw/                        # Original CSV files (e.g., Yahoo Finance exports).
│   └── processed/                  # Optimized Parquet files for high-performance I/O.
├── docs/
│   └── whitepaper/                 # LaTeX documentation and theoretical proofs.
├── src/
│   └── kalman_arb/                 # SOURCE PACKAGE
│       ├── backtest/               # SIMULATION ENGINE LAYER
│       │   ├── engine.py           # Core Event-Loop. It iterates through historical data row-by-row to prevent look-ahead bias. It manages the portfolio state (cash, equity, holdings) and executes the position sizing logic.
│       │   └── events.py           # Data Structures. Defines 'MarketEvent' (Standardized tick data) and 'SignalEvent' (Trade instructions passed from Strategy to Engine).
│       ├── core/                   # MATHEMATICAL KERNEL
│       │   └── kalman.py           # Pure NumPy implementation of the Kalman Filter. It contains the matrix algebra operations (Prediction, Update, Gain) and maintains the 'FilterState' (Covariance P and State Theta).
│       ├── strategy/               # DECISION LAYER
│       │   └── mean_reversion.py   # Finite State Machine (FSM). It consumes market data, updates the Kalman Filter, and applies threshold logic (entry_std, exit_std) to generate signals. It ensures no duplicate orders are sent.
│       └── utils/                  # INFRASTRUCTURE & I/O
│           └── data_loader.py      # Data Ingestion. Handles API connections (yfinance), data cleaning, timestamp alignment, and storage.
├── tests/                          # QUALITY ASSURANCE
│   ├── test_core_kalman.py         # Unit Test. Verifies mathematical convergence (RMSE) using a synthetic dataset where the hidden Beta is known.
│   └── test_strategy_logic.py      # Unit Test. Validates the FSM state transitions and signal thresholds using mocked market events.
├── main.py                         # ENTRY POINT. Configures the environment, loads data, initializes the engine, and executes the full simulation.
├── pyproject.toml                  # CONFIGURATION. Dependency management, build system, and package metadata.
└── fix_config.py                   # UTILITY. Script to enforce correct pyproject.toml creation on Windows environments.
```
## 6. Strategy Configuration & Hyperparameter Sensitivity

The robustness of the trading algorithm depends heavily on the calibration of the State-Space Model parameters. These values control the trade-off between the filter's reactivity to market shocks and its stability against noise.

The configuration is located in `src/kalman_arb/strategy/mean_reversion.py`.

| Parameter | Symbol | Value | Mathematical Role & Impact |
| :--- | :---: | :---: | :--- |
| **Transition Covariance** | $\delta$ | `1e-5` | **Process Noise Variance.** Controls the "learning rate" of the filter. A value of $10^{-5}$ implies that the hidden state vector $\theta_t = [\alpha, \beta]$ is assumed to be relatively stable, evolving slowly over time. Increasing this value makes the filter adapt faster to structural breaks but increases the risk of overfitting random market noise ("whiplash"). |
| **Measurement Variance** | $R$ | `0.01` | **Observation Noise.** Represents the assumed variance of the error term $\epsilon_t$. A higher $R$ tells the filter to treat observed price deviations largely as noise rather than a fundamental shift in the relationship, resulting in a smoother, less volatile Beta estimate. |
| **Entry Threshold** | $\sigma_{entry}$ | `1.0` | **Signal Sensitivity.** The strategy enters a position when the Z-Score exceeds this threshold. Note: In an adaptive Kalman model, residuals are minimized dynamically. Therefore, a Z-Score of $1.0$ is functionally equivalent to a higher threshold (e.g., $2.0$) in a static OLS model, as the filter naturally compresses deviations. |
| **Exit Threshold** | $\sigma_{exit}$ | `0.0` | **Mean Reversion Target.** The strategy closes positions only when the spread fully reverts to its theoretical fair value (Z-Score $\approx$ 0). This maximizes the profit per trade capture but may increase the holding period duration. |

### Calibration Logic
The current settings ($\delta=10^{-5}, R=0.01$) are optimized for **Inter-Day trading** on developed market ETFs (EWA/EWC).
* **For High-Frequency Trading (HFT):** $\delta$ should be increased to capture micro-structure changes rapidly.
* **For Macro-Trading:** $\delta$ should be decreased to filter out short-term volatility and capture long-term economic convergence.

---

## 7. Legal Disclaimer & Risk Disclosure

**IMPORTANT NOTICE: PLEASE READ CAREFULLY.**

This software repository (`Kalman-StatArb-Project`) is strictly for **educational, research, and informational purposes only**. It is not intended to be, and does not constitute, financial advice, investment advice, trading advice, or any other advice.

**1. No Financial Advice**
The content, code, and documentation provided herein do not constitute a recommendation to buy, sell, or hold any security, financial product, or instrument. The strategies implemented are theoretical demonstrations of mathematical concepts (Linear Gaussian State-Space Models) applied to financial time series.

**2. Risk of Loss**
Algorithmic and quantitative trading involves a substantial risk of loss and is not suitable for every investor. The valuation of financial instruments may fluctuate, and as a result, investors may lose more than their original investment. The high degree of leverage often associated with statistical arbitrage can work against you as well as for you.

**3. Hypothetical Performance**
The performance results presented in this documentation are based on simulated backtesting. Hypothetical or simulated performance results have certain inherent limitations. Unlike an actual performance record, simulated results do not represent actual trading. Also, since the trades have not actually been executed, the results may have under- or over-compensated for the impact, if any, of certain market factors, such as lack of liquidity, slippage, and spread costs. No representation is being made that any account will or is likely to achieve profits or losses similar to those shown.

**4. Software Warranty**
The software is provided "AS IS", without warranty of any kind, express or implied, including but not limited to the warranties of merchantability, fitness for a particular purpose, and non-infringement. In no event shall the authors or copyright holders be liable for any claim, damages, or other liability, whether in an action of contract, tort, or otherwise, arising from, out of, or in connection with the software or the use or other dealings in the software.

