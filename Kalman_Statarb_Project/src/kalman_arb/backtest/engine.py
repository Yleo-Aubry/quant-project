import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from datetime import datetime
from typing import List, Dict

from kalman_arb.backtest.events import MarketEvent, SignalType
from kalman_arb.strategy.mean_reversion import KalmanMeanReversionStrategy

class BacktestEngine:
    """
    Moteur d'exécution Event-Driven.
    Simule le passage du temps et gère le portefeuille fictif.
    """
    def __init__(self, data_path: str, initial_capital: float = 10000.0):
        self.data_path = data_path
        self.initial_capital = initial_capital
        self.current_capital = initial_capital
        self.equity_curve = [] # Historique de la valeur du portefeuille
        self.trades = []       # Historique des transactions
        
        # Position actuelle (Quantités)
        self.pos_y = 0
        self.pos_x = 0
        
        # Instanciation de la stratégie
        self.strategy = KalmanMeanReversionStrategy(entry_std=1.0, exit_std=0.0)

    def load_data(self) -> pd.DataFrame:
        """
        Charge et prépare les données CSV.
        Correction : Utilise index_col=0 pour être compatible avec 'Date' (Yahoo) et 'timestamp' (Synthétique).
        """
        # On dit à pandas : la colonne 0 est l'index (date), peu importe son nom.
        df = pd.read_csv(self.data_path, index_col=0, parse_dates=True)
        
        # On s'assure que l'index est bien trié temporellement
        df.sort_index(inplace=True)
        
        # Standardisation des noms de colonnes (au cas où)
        # On s'attend à ce que le CSV ait asset_x et asset_y
        # Si les noms diffèrent légèrement, on pourrait les renommer ici, 
        # mais data_loader.py fait déjà le travail.
        
        return df

    def run(self):
        """Boucle principale de simulation (The Event Loop)."""
        print(f"--- Démarrage du Backtest sur {self.data_path} ---")
        df = self.load_data()
        
        total_steps = len(df)
        print(f"Simulation de {total_steps} barres de données...")

        for i, (timestamp, row) in enumerate(df.iterrows()):
            # 1. Création de l'événement de marché
            event = MarketEvent(
                timestamp=timestamp,
                symbol_y='Asset_Y', price_y=row['asset_y'],
                symbol_x='Asset_X', price_x=row['asset_x']
            )

            # 2. Mise à jour de la valeur du portefeuille (Mark-to-Market)
            portfolio_value = self.current_capital + \
                              (self.pos_y * event.price_y) + \
                              (self.pos_x * event.price_x)
            self.equity_curve.append({'timestamp': timestamp, 'equity': portfolio_value})

            # 3. Interrogation de la stratégie
            signal = self.strategy.calculate_signal(event)

            # 4. Exécution (si signal)
            if signal:
                self._execute_signal(signal, event)

        self._generate_report()

    def _execute_signal(self, signal, market: MarketEvent):
        """Simule l'exécution des ordres."""
        
        # ---  ALLOCATION DYNAMIQUE ---
        # on engage le capital.
        # On utilise 45% du capital actuel par jambe (45% Long + 45% Short = 90% Investi).
        # On garde 10% de cash buffer pour les appels de marge ou frais.
        target_exposure = self.current_capital * 0.45 
        # ------------------------------------------
        
        # Calcul des quantités théoriques basées sur le Beta estimé
        qty_y = int(target_exposure / market.price_y)
        qty_x = int((target_exposure * signal.estimated_beta) / market.price_x)

        
        if signal.signal_type == SignalType.LONG_SPREAD:
            # Achat Y, Vente X
            self._rebalance(qty_y, -qty_x, market)
            self.trades.append({'time': market.timestamp, 'type': 'LONG', 'price': market.price_y})

        elif signal.signal_type == SignalType.SHORT_SPREAD:
            # Vente Y, Achat X
            self._rebalance(-qty_y, qty_x, market)
            self.trades.append({'time': market.timestamp, 'type': 'SHORT', 'price': market.price_y})

        elif signal.signal_type == SignalType.EXIT:
            # Fermeture totale
            self._rebalance(0, 0, market)
            self.trades.append({'time': market.timestamp, 'type': 'EXIT', 'price': market.price_y})

    def _rebalance(self, target_y, target_x, market):
        """Ajuste les positions et met à jour le cash."""
        diff_y = target_y - self.pos_y
        diff_x = target_x - self.pos_x
        
        # Coût de transaction (Cash impact)
        cost_y = diff_y * market.price_y
        cost_x = diff_x * market.price_x
        
        self.current_capital -= (cost_y + cost_x)
        
        # Mise à jour positions
        self.pos_y = target_y
        self.pos_x = target_x

    def _generate_report(self):
        """Analyse finale de la performance."""
        equity_df = pd.DataFrame(self.equity_curve).set_index('timestamp')
        
        # Calcul des rendements
        returns = equity_df['equity'].pct_change().dropna()
        total_return = (equity_df['equity'].iloc[-1] / self.initial_capital) - 1
        sharpe = (returns.mean() / returns.std()) * np.sqrt(252) if returns.std() != 0 else 0
        
        print("\n--- RÉSULTATS DU BACKTEST ---")
        print(f"Capital Initial : ${self.initial_capital:.2f}")
        print(f"Capital Final   : ${equity_df['equity'].iloc[-1]:.2f}")
        print(f"Rendement Total : {total_return*100:.2f}%")
        print(f"Ratio de Sharpe : {sharpe:.2f}")
        print(f"Nombre de Trades: {len(self.trades)}")
        
        # Plot
        plt.figure(figsize=(12, 6))
        plt.plot(equity_df['equity'], label='Stratégie Kalman')
        plt.title(f"Equity Curve (Sharpe: {sharpe:.2f})")
        plt.grid(True)
        plt.legend()
        plt.show()
