# C++ PDE Option Pricer (Crank-Nicolson)

![Language](https://img.shields.io/badge/language-C++17-blue.svg)
![Build](https://img.shields.io/badge/build-CMake-green.svg)
![License](https://img.shields.io/badge/license-MIT-lightgrey.svg)

## Overview

A high-performance Option Pricing engine implemented in **C++** from scratch. It solves the **Black-Scholes Partial Differential Equation (PDE)** using the **Crank-Nicolson** finite difference scheme.

Unlike standard analytical solvers, this project demonstrates numerical analysis techniques applied to quantitative finance, focusing on stability, memory management, and computational complexity.

## Key Features

* **Numerical Method:** Unconditionally stable **Crank-Nicolson** scheme ($O(\Delta t^2, \Delta x^2)$).
* **Linear Algebra:** Custom implementation of the **Thomas Algorithm (TDMA)** to solve tridiagonal systems in **$O(N)$** complexity (replacing standard $O(N^3)$ matrix inversion).
* **Mathematical Optimization:** Implements the **Log-transformed Black-Scholes equation** (Heat Equation) to ensure constant diffusion coefficients and pre-computed matrices.
* **Greeks:** Automatic calculation of **Delta** and **Gamma** sensitivities derived directly from the finite difference grid.
* **Architecture:** Modular Object-Oriented Design using **Strategy Pattern** for Payoffs and separation of concerns (Mathematical Solver vs. Financial Engine).

## Mathematical Framework

The solver transforms the standard Black-Scholes PDE via $x = \ln(S)$ to obtain a convection-diffusion equation with constant coefficients:

$$\frac{\partial V}{\partial t} + \frac{1}{2}\sigma^2 \frac{\partial^2 V}{\partial x^2} + (r - \frac{1}{2}\sigma^2) \frac{\partial V}{\partial x} - rV = 0$$

The discretized system $Ax = d$ is solved at each time step using a fast tridiagonal solver. Boundary conditions are handled via exact Dirichlet asymptotic behavior for optimal precision.

## Project Structure

```text
├── CMakeLists.txt        # Build configuration
├── include/              # Header files (Interfaces)
│   ├── LinearSolver.h    # TDMA Algorithm declaration
│   ├── Payoff.h          # Abstract Payoff class & derivatives
│   ├── PDESolver.h       # Core Pricing Engine declaration
│   └── Interface.h       # User Interface utils
└── src/                  # Source files (Implementation)
    ├── LinearSolver.cpp  # Tridiagonal Matrix Solver
    ├── PDESolver.cpp     # Finite Difference logic
    ├── Interface.cpp     # Console UI implementation
    └── main.cpp          # Entry point & Unit Tests
