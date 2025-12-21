"""
Core Kalman Filter Implementation for Dynamic Pairs Trading.

Reference:
    "Stochastic Foundations of Dynamic Pairs Trading: A Bayesian State-Space Approach"
    Section 4: Bayesian Derivation of the Kalman Filter
"""

import numpy as np
from dataclasses import dataclass
from typing import Tuple, Dict, Optional

@dataclass
class FilterState:
    """
    Représente l'état du filtre à l'instant t.
    
    Attributes:
        theta (np.ndarray): Vecteur d'état [alpha, beta]^T de dimension (2, 1).
        P (np.ndarray): Matrice de covariance de l'erreur d'estimation (2, 2).
    """
    theta: np.ndarray
    P: np.ndarray

class KalmanFilter:
    """
    Implémentation d'un Filtre de Kalman Linéaire pour l'estimation de paramètres
    variant dans le temps (Time-Varying Parameters).
    
    Modèle Espace-État (LGSSM):
        State: theta_t = F * theta_{t-1} + w_t,  w_t ~ N(0, Q)
        Meas : y_t     = H_t * theta_t   + v_t,  v_t ~ N(0, R)
    """

    def __init__(self, 
                 R: float = 1e-3, 
                 delta: float = 1e-4,
                 initial_state: Optional[FilterState] = None):
        """
        Initialisation du moteur.

        Args:
            R (float): Variance du bruit de mesure (scalaire).
            delta (float): Paramètre de régularisation pour le bruit de processus Q.
                           On suppose Q = (delta / (1-delta)) * I.
                           Cela permet au filtre de s'adapter aux changements de régime.
            initial_state (FilterState, optional): État a priori (Prior).
        """
        # Matrices du système (Section 3: Exact Discretization)
        # F est l'identité car on suppose une marche aléatoire pour alpha et beta
        self.F = np.eye(2)
        
        # Q est la matrice de covariance du bruit de processus
        # Une valeur plus élevée rend le filtre plus réactif (mais plus bruité)
        self.Q = np.eye(2) * (delta / (1 - delta))
        
        # R est la matrice de covariance du bruit de mesure (1x1)
        self.R = np.array([[R]])

        # Initialisation de l'état (Priors diffus si non spécifié)
        if initial_state:
            self.state = initial_state
        else:
            self.state = FilterState(
                theta=np.zeros((2, 1)),  # Prior neutre
                P=np.eye(2) * 1.0        # Incertitude initiale élevée
            )

    def update(self, y: float, x: float) -> Dict[str, float]:
        """
        Exécute une itération complète (Predict + Update).
        
        Mathématiques (Section 4.3 du White Paper):
        1. Prediction (Time Update)
        2. Innovation Calculation
        3. Gain Computation (Schur Complement)
        4. State Correction
        
        Args:
            y (float): Prix de l'actif dépendant (Asset Y).
            x (float): Prix de l'actif indépendant (Asset X).
            
        Returns:
            Dict contenant les métriques critiques pour le trading (Z-Score).
        """
        # --- 0. Construction de la matrice d'observation H_t ---
        # H_t = [1, x_t]
        H = np.array([[1.0, x]])

        # --- 1. PREDICT STEP (A Priori) ---
        # theta_{t|t-1} = F * theta_{t-1|t-1}
        theta_pred = self.F @ self.state.theta
        
        # P_{t|t-1} = F * P_{t-1|t-1} * F^T + Q
        P_pred = self.F @ self.state.P @ self.F.T + self.Q

        # --- 2. INNOVATION (Residuals) ---
        # y_hat = H * theta_{t|t-1}
        y_hat = H @ theta_pred
        
        # e_t = y_t - y_hat (Innovation brute)
        error = y - y_hat.item()

        # S_t = H * P_{t|t-1} * H^T + R (Variance de l'innovation)
        S = H @ P_pred @ H.T + self.R
        S_scalar = S.item()

        # --- 3. UPDATE STEP (A Posteriori) ---
        # Gain de Kalman Optimal: K = P_{t|t-1} * H^T * S^{-1}
        # Note: On utilise l'inverse scalaire car y est 1D
        K = P_pred @ H.T / S_scalar

        # theta_{t|t} = theta_{t|t-1} + K * e_t
        theta_new = theta_pred + K * error

        # P_{t|t} = (I - K * H) * P_{t|t-1}
        # Utilisation de la forme de Joseph pour la stabilité numérique (optionnel ici mais pro)
        # P_{t|t} = (I - KH) P (I - KH)^T + K R K^T
        # Ici on utilise la forme simple dérivée en Section 4.3 pour la rapidité
        I = np.eye(2)
        P_new = (I - K @ H) @ P_pred

        # Mise à jour de l'état interne
        self.state = FilterState(theta=theta_new, P=P_new)

        # --- 4. SIGNAL GENERATION ---
        # Z-Score = e_t / sqrt(S_t) (Section 6.2)
        if S_scalar <= 0:
            z_score = 0.0
        else:
            z_score = error / np.sqrt(S_scalar)

        return {
            "alpha": theta_new[0, 0],
            "beta": theta_new[1, 0],
            "error": error,
            "variance": S_scalar,
            "z_score": z_score
        }

    def batch_filter(self, ys: np.ndarray, xs: np.ndarray) -> Dict[str, np.ndarray]:
        """
        Traite un historique complet de données (Backtesting vectorisé).
        Permet de vérifier la convergence sur tout le dataset synthétique.
        """
        n = len(ys)
        alphas = np.zeros(n)
        betas = np.zeros(n)
        z_scores = np.zeros(n)
        errors = np.zeros(n)
        
        # Boucle temporelle
        for t in range(n):
            metrics = self.update(ys[t], xs[t])
            alphas[t] = metrics['alpha']
            betas[t] = metrics['beta']
            z_scores[t] = metrics['z_score']
            errors[t] = metrics['error']
            
        return {
            "alpha": alphas,
            "beta": betas,
            "z_score": z_scores,
            "error": errors
        }
