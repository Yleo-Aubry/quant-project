# Fixed Mix Strategy - Lifecycle Backtesting Engine

## 1. What is the Fixed Mix Strategy?
The **Fixed Mix** (or Constant Mix) is a dynamic asset allocation strategy that maintains a constant ratio between asset classes (e.g., 60% Equities / 40% Bonds) throughout the investment horizon.

Unlike a "Buy and Hold" strategy where allocation drifts with market movements, the Fixed Mix strategy requires **periodic rebalancing** (monthly in this simulation).
* **Mechanism:** When equities rise, the portfolio sells the surplus to buy bonds (selling high). When equities fall, the portfolio sells bonds to buy equities (buying low).
* **Goal:** To maintain a constant risk exposure and capitalize on volatility harvesting (mean reversion).

## 2. Investor Profiles
The program supports 5 distinct risk profiles, defining the target fixed allocation ($w_{equity}$ / $w_{bond}$) and the specific assets used:

* **PRUDENT:** 20% Equity / 80% Bonds (Gov Bonds)
* **MODERE:** 40% Equity / 60% Bonds (Inflation Linked)
* **EQUILIBRE:** 60% Equity / 40% Bonds (Corporate Bonds)
* **DYNAMIQUE:** 80% Equity / 20% Bonds (High Yield)
* **AGRESSIF:** 95% Equity / 5% Bonds (High Yield)

## 3. Data & Backtesting Period
The engine performs a historical backtest based on real market data provided by **HSBC**.
* **Historical Range:** December 2001 – 2025.
* **Assets Covered:** Global Equities (hedged/unhedged), US Treasuries, Corporate Bonds, and High Yield Bonds.

The simulation reconstructs the path of a portfolio over this specific historical timeframe to assess how the strategy would have performed in real market conditions.

## 4. Modeling Monthly Contributions (Quadratic Lifecycle)
To ensure the simulation reflects realistic investor behavior, the program implements a **Quadratic Lifecycle Contribution Model** (based on *Bruder et al., 2025*).

Instead of assuming constant savings, the monthly contribution $C(t)$ follows a parabolic curve defined on the active lifecycle $[StartAge, EndAge]$:
* **Ramp-up:** Contributions increase during the early career.
* **Peak:** Maximum saving capacity is reached at **age 53**.
* **Decline:** Contributions decrease as the investor approaches retirement.

$$C(t) = a(t - t_{peak})^2 + C_{max}$$

This model allows us to test the strategy's sensitivity to the timing of cash flows (sequence of returns risk).

## 5. Performance Calculation Methodology
The script computes the portfolio's evolution month by month considering:
1.  **Asset Returns:** Actual historical monthly variations.
2.  **Dynamic Inflows:** Monthly injections based on the quadratic model.
3.  **Rebalancing Costs/Logic:** Adjusting weights back to the target profile.

### Computed Metrics
The output focuses on:
* **Capital Evolution:** Tracking the portfolio value vs. total capital invested over time.
* **Total Invested:** The sum of all monthly contributions.
* **Final Valuation:** The portfolio value at the end of the historical period.
