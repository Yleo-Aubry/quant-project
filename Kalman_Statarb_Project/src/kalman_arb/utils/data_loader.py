import numpy as np
import pandas as pd
import yfinance as yf
import os
from datetime import datetime, timedelta


RAW_DATA_PATH = os.path.join(os.getcwd(), 'data', 'raw')

def generate_synthetic_data(n_points=2000, dt=1/252):
    """
   
    
    Modèle :
    1. X_t ~ Geometric Brownian Motion (Asset X)
    2. Beta_t ~ Random Walk (Hedge Ratio dynamique)
    3. Y_t = alpha + Beta_t * X_t + Noise
    
   
    """
    print(f"[Synthetic] Génération de {n_points} points de données...")
    
    np.random.seed(42) 
    
 
    mu_x = 0.05
    sigma_x = 0.2
    x = np.zeros(n_points)
    x[0] = 100.0
    
  
    beta_true = np.linspace(0.8, 1.2, n_points) + np.random.normal(0, 0.02, n_points)
    
   
    alpha_true = 5.0
    noise_level = 0.5
    y = np.zeros(n_points)
    
    for t in range(1, n_points):
        
        dW = np.random.normal(0, np.sqrt(dt))
        x[t] = x[t-1] * np.exp((mu_x - 0.5*sigma_x**2)*dt + sigma_x*dW)
        
        
        y[t] = alpha_true + beta_true[t] * x[t] + np.random.normal(0, noise_level)
        
    # Création du DataFrame
    dates = [datetime.now() - timedelta(days=n_points-i) for i in range(n_points)]
    df = pd.DataFrame({
        'timestamp': dates,
        'asset_x': x,
        'asset_y': y,
        'beta_true': beta_true 
    })
    df.set_index('timestamp', inplace=True)
    
  
    output_path = os.path.join(RAW_DATA_PATH, 'synthetic_cointegration.csv')
    df.to_csv(output_path)
    print(f"[Synthetic] Fichier sauvegardé : {output_path}")

def download_market_data(ticker_x, ticker_y, start_date, end_date):
  
    print(f"[Market] Téléchargement de {ticker_x} et {ticker_y}...")
    
  
    data_x = yf.download(ticker_x, start=start_date, end=end_date, progress=False, auto_adjust=False)['Adj Close']
    data_y = yf.download(ticker_y, start=start_date, end=end_date, progress=False, auto_adjust=False)['Adj Close']
    
    
    df = pd.concat([data_x, data_y], axis=1, join='inner')
    df.columns = ['asset_x', 'asset_y']
    
 
    df.dropna(inplace=True)
    
    if len(df) == 0:
        raise ValueError("Erreur: Aucune donnée commune trouvée (vérifiez les tickers/dates).")
        

    filename = f"real_{ticker_x}_{ticker_y}.csv"
    output_path = os.path.join(RAW_DATA_PATH, filename)
    df.to_csv(output_path)
    print(f"[Market] Données alignées ({len(df)} points) sauvegardées : {output_path}")
if __name__ == "__main__":

    if not os.path.exists(RAW_DATA_PATH):
        os.makedirs(RAW_DATA_PATH)

    generate_synthetic_data()
    
 
    download_market_data('GLD', 'GDX', '2020-01-01', '2023-12-31')

    download_market_data('EWA', 'EWC', '2015-01-01', '2023-12-31')
