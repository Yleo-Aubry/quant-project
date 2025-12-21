import sys
import os
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

# Ajout du dossier src au path pour l'import
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..', 'src')))

from kalman_arb.core.kalman import KalmanFilter

def test_convergence_on_synthetic_data():
    print("--- Test de Convergence Mathématique (Kalman Core) ---")
    
    # 1. Chargement des données synthétiques
    data_path = os.path.join('data', 'raw', 'synthetic_cointegration.csv')
    if not os.path.exists(data_path):
        print(f"Erreur: Le fichier {data_path} n'existe pas. Lancez data_loader.py d'abord.")
        return

    df = pd.read_csv(data_path)
    print(f"Données chargées : {len(df)} points")
    
    # 2. Initialisation du Filtre
    # On fixe delta bas pour avoir une estimation stable
    kf = KalmanFilter(R=0.5, delta=1e-4) 
    
    # 3. Exécution en batch
    print("Exécution du filtre...")
    results = kf.batch_filter(df['asset_y'].values, df['asset_x'].values)
    
    # 4. Calcul de l'erreur (RMSE) entre Beta Estimé et Beta Réel
    beta_est = results['beta']
    beta_true = df['beta_true'].values
    
    # On ignore les 50 premiers points (temps de chauffe/convergence du filtre)
    warmup = 50
    rmse = np.sqrt(np.mean((beta_est[warmup:] - beta_true[warmup:])**2))
    
    print(f"RMSE (Erreur Moyenne) sur Beta : {rmse:.4f}")
    
    # Validation du test
    if rmse < 0.1:
        print("[SUCCESS] Le filtre converge correctement vers la vérité terrain.")
    else:
        print("[FAILURE] Le filtre diverge ou est mal calibré.")

    #  Sauvegarde d'un plot pour inspection visuelle
    plt.figure(figsize=(12, 6))
    plt.plot(beta_true, label='Vrai Beta (Généré)', color='black', alpha=0.6)
    plt.plot(beta_est, label='Beta Estimé (Kalman)', color='red', linestyle='--')
    plt.title(f"Validation Convergence Kalman (RMSE={rmse:.4f})")
    plt.legend()
    plt.grid(True)
    plt.savefig("tests/test_convergence_plot.png")
    print("Graphique de validation sauvegardé dans tests/test_convergence_plot.png")

if __name__ == "__main__":
    test_convergence_on_synthetic_data()
