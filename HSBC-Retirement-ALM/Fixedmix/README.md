# ALM Simulation Engine - Fixed Mix Strategy

## Overview
This project implements a stochastic simulation engine (Monte Carlo) designed for Asset Liability Management (ALM) and retirement planning analysis. The tool projects wealth evolution based on a "Fixed Mix" allocation strategy, integrating dynamic cash flows (salary curve, elastic savings rate) and complex market scenarios.

The script generates a comprehensive visual dashboard and a terminal-based report quantifying Tail Risk and risk-adjusted performance metrics.

## Key Features

### 1. Stochastic Modeling
* **Monte Carlo Simulation:** Generates 500 market trajectories for Equity and Bond assets using correlated Geometric Brownian Motion.
* **Asset Correlation:** Accounts for the correlation matrix between asset classes defined in the input configuration.

### 2. Crisis & Stress Testing
* **Deterministic Crisis Injection:** Capability to inject specific market shocks (crashes) at defined dates.
* **Parametric Control:** Customizable drawdown depth, duration, and volatility expansion factors during crisis periods.

### 3. Cash Flow Logic
* **Dynamic Contributions:** Implements a logistic salary growth model with saturation levels.
* **Elastic Savings:** Savings rate adjusts based on income progression and maturity thresholds.

### 4. Financial KPIs
* **Risk Metrics:** Shortfall Probability, Value at Risk (VaR 95), and Maximum Underwater Duration.
* **Performance:** Median IRR (Internal Rate of Return), Sortino Ratio.
* **Real Terms:** Adjusts final capital and P&L for inflation to reflect real purchasing power.

## Technical Requirements

### Prerequisites
* Python 3.8+
* Input Data: `AssumptionForSimulation.xlsx` file containing:
    * *Asset Name*
    * *Expected Return*
    * *Volatility*
    * *Correlation*

### Dependencies
Install the required libraries using pip:

    pip install pandas numpy matplotlib scipy openpyxl

## Configuration

Configuration is managed directly within the `main.py` header (Section 1. CONFIGURATION).

### User Parameters
| Variable | Description |
| :--- | :--- |
| `PROFIL_CHOISI` | Asset Allocation Profile (e.g., EQUILIBRE, DYNAMIQUE). |
| `NB_ANNEES_ACCUMULATION` | Simulation time horizon (e.g., 40 years). |
| `DATE_DEBUT_T0` | Start date for the time series. |
| `SALAIRE_INITIAL` | Initial monthly net income. |
| `CAPITAL_INITIAL` | Initial lump sum investment. |

### Crisis Scenarios
To stress-test the portfolio, adjust the following parameters:
* `SIMULER_KRACH`: Boolean flag to enable/disable the module.
* `DATE_CRISE`: Trigger date for the market shock.
* `PARAMS_CRISE`: Dictionary defining the equity drop (`drop_eq`) and recovery dynamics.

## Outputs

### 1. Graphical Dashboard
The script produces a matplotlib window with four analysis panels:
1.  **Financial Flows:** Visualization of Income vs. Savings contribution over time.
2.  **Capital Projection:** Cone of uncertainty showing P95 (Optimistic), Median, and P5 (Pessimistic) trajectories against Nominal Cash invested.
3.  **Replacement Rate:** Decumulation phase projection (Pension/Last Salary ratio).
4.  **Annual Returns:** Distribution of annual returns distinguishing between historical backtests and future projections.

### 2. Terminal Report
A professional summary is printed to the console, focusing on decision-critical metrics:
* **Median IRR:** Annualized Internal Rate of Return.
* **Shortfall Risk:** Probability of ending with less than the total capital invested.
* **Real P5 P&L:** Net-of-inflation performance in the worst 5% of scenarios.

## Methodological Note
This model relies on Gaussian assumptions for random return generation. While the "Crisis" module allows for the injection of fat-tailed events, past performance or simulated results do not guarantee future outcomes. Users must ensure the consistency of the expected return and volatility assumptions provided in the source Excel file.

---

### Disclaimer
*This software is provided for informational and educational purposes only. It does not constitute financial investment advice. The author assumes no responsibility for the accuracy of the results or decisions made based on this tool.*
