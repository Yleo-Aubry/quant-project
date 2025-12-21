import os

toml_content = """[build-system]
requires = ["setuptools>=61.0"]
build-backend = "setuptools.build_meta"

[project]
name = "kalman_arb"
version = "0.1.0"
authors = [
  { name="Quant Researcher", email="quant@example.com" },
]
description = "Event-driven backtester with Kalman Filter for Statistical Arbitrage"
readme = "README.md"
requires-python = ">=3.9"
dependencies = [
    "numpy",
    "pandas",
    "yfinance",
    "matplotlib"
]

[tool.setuptools.packages.find]
where = ["src"]
"""

# Création du fichier pyproject.toml
with open("pyproject.toml", "w", encoding="utf-8") as f:
    f.write(toml_content)

print("SUCCES : Fichier 'pyproject.toml' créé correctement.")

# Vérification des __init__.py critiques
init_files = [
    "src/kalman_arb/__init__.py",
    "src/kalman_arb/core/__init__.py", 
    "src/kalman_arb/strategy/__init__.py",
    "src/kalman_arb/backtest/__init__.py",
    "src/kalman_arb/utils/__init__.py"
]

print("Vérification des fichiers __init__.py...")
for path in init_files:
    if not os.path.exists(path):
        os.makedirs(os.path.dirname(path), exist_ok=True)
        with open(path, "w") as f: pass
        print(f"  [CREE] {path}")
    else:
        print(f"  [OK] {path}")

print("\nPRET : Vous pouvez lancer 'python -m pip install -e .'")
