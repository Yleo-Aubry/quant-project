import sys
import os

# --- BLOC DE DÉTECTION AUTOMATIQUE DU CODE SOURCE ---
# On récupère le dossier courant où se trouve main.py
current_dir = os.path.dirname(os.path.abspath(__file__))
# On construit le chemin vers le dossier 'src'
src_path = os.path.join(current_dir, 'src')

# On force Python à regarder dans 'src' AVANT d'importer quoi que ce soit
if src_path not in sys.path:
    sys.path.insert(0, src_path)
# ----------------------------------------------------

from kalman_arb.backtest.engine import BacktestEngine

def run_simulation():
    # 1. Sélection des données
    # On passe sur EWA vs EWC
    data_file = os.path.join('data', 'raw', 'real_EWA_EWC.csv')
    
    if not os.path.exists(data_file):
        # Fallback si le fichier n'existe pas, on reste sur GLD_GDX mais c'est moins bon
        print(f"Attention: {data_file} introuvable, retour sur GLD_GDX")
        data_file = os.path.join('data', 'raw', 'real_GLD_GDX.csv')

   

    # 2. Initialisation du moteur
    print(f"Lancement du backtest sur : {data_file}")
    # On commence avec 100,000 $
    engine = BacktestEngine(data_path=data_file, initial_capital=100000.0)
    
    # 3. Lancement
    engine.run()

if __name__ == "__main__":
    try:
        run_simulation()
    except Exception as e:
        print(f"\nCRASH DU PROGRAMME : {e}")
        import traceback
        traceback.print_exc()
        input("Appuyez sur Entrée pour fermer...")
