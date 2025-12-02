# Black–Scholes Pricer (C++)

This project implements a full Black–Scholes pricer in C++17, including dividend yield and all major Greeks (Δ, Γ, Vega, Theta, Rho).  
It is fully modular, using separate `.h` and `.cpp` files, and provides a simple command-line interface for testing.

---

## 📌 Features

- Pricing of **European Call and Put options**
- Continuous **dividend yield** `q`
- Computation of all Greeks:
  - Delta (call & put)
  - Gamma  
  - Vega  
  - Theta (call & put)  
  - Rho (call & put)
- Input parameters directly from the terminal
- Clean modular C++ structure

---

## 🧮 Mathematical Formulas

For a spot price \(S\), strike \(K\), maturity \(T\), volatility \(\sigma\), risk-free rate \(r\), and dividend yield \(q\):

### d₁ and d₂
\[
d_1 = \frac{\ln(S/K) + (r - q + \tfrac{1}{2}\sigma^2)T}{\sigma\sqrt{T}},
\qquad
d_2 = d_1 - \sigma\sqrt{T}.
\]

### Call price
\[
C = S e^{-qT} \Phi(d_1) - K e^{-rT} \Phi(d_2)
\]

### Put price
\[
P = K e^{-rT} \Phi(-d_2) - S e^{-qT} \Phi(-d_1)
\]

where \(\Phi\) is the CDF of the standard normal distribution and \(\phi\) its PDF.

---

## 🧮 Greeks

### Delta
\[
\Delta_{\text{call}} = e^{-qT}\Phi(d_1),
\qquad
\Delta_{\text{put}}  = e^{-qT}(\Phi(d_1)-1)
\]

### Gamma
\[
\Gamma = \frac{e^{-qT}\phi(d_1)}{S\sigma\sqrt{T}}
\]

### Vega
\[
V = S e^{-qT}\phi(d_1)\sqrt{T}
\]

### Theta
\[
\Theta_{\text{call}} =
- \frac{S e^{-qT}\phi(d_1)\sigma}{2\sqrt{T}}
+ q S e^{-qT}\Phi(d_1)
- r K e^{-rT}\Phi(d_2)
\]

\[
\Theta_{\text{put}} =
- \frac{S e^{-qT}\phi(d_1)\sigma}{2\sqrt{T}}
- q S e^{-qT}\Phi(-d_1)
+ r K e^{-rT}\Phi(-d_2)
\]

### Rho
\[
\rho_{\text{call}} = K T e^{-rT}\Phi(d_2),
\qquad
\rho_{\text{put}}  = -K T e^{-rT}\Phi(-d_2)
\]

---
## 💡 Example

### Example Input


S      = 100
K      = 100
T      = 1
r      = 0.05
sigma  = 0.20
q      = 0


### Typical Output

Call price : 10.4506
Put  price : 5.5735

--- Greeks (Call) ---
Delta (Call) : 0.6368
Gamma        : 0.0188
Vega         : 37.52
Theta (Call) : -6.41
Rho   (Call) : 53.23

--- Greeks (Put) ---
Delta (Put)  : -0.3632
Theta (Put)  : -1.66
Rho   (Put)  : -41.89




## 📁 Project Structure

```text
black-scholes-pricer/
│
├── src/
│   ├── main.cpp              # Command-line interface
│   ├── black_scholes.h       # Struct + function declarations
│   └── black_scholes.cpp     # Pricing formulas + Greeks implementation
│
└── README.md                 # Project documentation


