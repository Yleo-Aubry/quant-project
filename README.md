# Quant Project Portfolio

This repository contains a growing collection of quantitative finance projects implemented in C++ and Python.  
The goal is to build a clean and well-structured portfolio of models and numerical methods used in financial engineering.

Each project is contained in its own folder, with its own source code, documentation, and build instructions.

---

## 📂 Available Projects

### 1. PDE Option Pricer (Crank-Nicolson) 
Location: `C++-PDE-Option-Pricer-(Crank-Nicolson)/` 

An advanced numerical engine solving the **Black-Scholes Partial Differential Equation (PDE)**. Unlike the analytic formula, this solver uses Finite Difference Methods (FDM) to price options.

- **Method:** Unconditionally stable **Crank-Nicolson** scheme.
- **Maths:** Log-transformed Heat Equation & **Thomas Algorithm (TDMA)** for $O(N)$ linear resolution.
- **Features:** Computes Price, **Delta**, and **Gamma** directly from the grid.
- **Build System:** Professional **CMake** architecture.
- **Precision:** $< 10^{-3}$ error vs closed-form solutions.

### 2. Black–Scholes Analytic Pricer (C++)
Location: `black-scholes-pricer/`

A foundational implementation of the closed-form solutions for European options.

- European call & put pricing  
- Continuous dividend yield  
- Full set of Greeks (Δ, Γ, Vega, Theta, Rho)  
- Modular structure (`.h` + `.cpp`)  
- Command-line user interface  

---

## 🛠️ Technologies

- **Languages:** C++ (C++17)
- **Build Systems:** CMake, Make
- **Compilers:** GCC / MinGW-w64 / Clang
- **Tools:** VS Code, Git

---

## 📄 License

This project is released under the **MIT License**.  
See the `LICENSE` file for details.

---

## 👤 Author

**Yléo Aubry** ENSAE Paris – Quantitative Finance Track
