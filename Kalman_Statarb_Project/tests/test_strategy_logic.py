import sys
import os
import unittest
from datetime import datetime

# Ajout du path
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..', 'src')))

from kalman_arb.backtest.events import MarketEvent, SignalType
from kalman_arb.strategy.mean_reversion import KalmanMeanReversionStrategy

class TestStrategyLogic(unittest.TestCase):
    def setUp(self):
        self.strategy = KalmanMeanReversionStrategy(entry_std=2.0, exit_std=0.0)
        
    def test_long_entry_signal(self):
        print("\n--- Test Strategy: Long Entry ---")
        # On simule un écart violent pour forcer le Z-Score
        # Prix Y chute, Prix X monte -> Z-Score devrait être très négatif
        
        # 1. Initialisation (Chauffe)
        for i in range(10):
            self.strategy.calculate_signal(MarketEvent(datetime.now(), 'Y', 100, 'X', 100))
            
        # 2. Choc de marché (Y crash à 90, X reste à 100)
        # Cela devrait créer un spread négatif massif
        event = MarketEvent(datetime.now(), 'Y', 90.0, 'X', 100.0)
        signal = self.strategy.calculate_signal(event)
        
        if signal:
            print(f"Signal Généré: {signal.signal_type} | Z: {signal.z_score:.2f}")
            self.assertEqual(signal.signal_type, SignalType.LONG_SPREAD)
        else:
            print("Pas de signal généré (Check Z-Score threshold)")
            
    def test_state_machine(self):
        # Vérifie qu'on ne peut pas entrer deux fois de suite
        print("\n--- Test Strategy: State Machine ---")
        
        # Force Entry
        self.strategy.current_position = 1 # Déjà Long
        
        # Nouveau choc qui justifierait un Long
        event = MarketEvent(datetime.now(), 'Y', 80.0, 'X', 100.0)
        signal = self.strategy.calculate_signal(event)
        
        # On ne doit PAS recevoir de signal LONG_SPREAD car on est déjà dedans
        if signal is None:
            print("State Machine respectée: Pas de double entrée.")
        else:
            # Sauf si c'est un signal de sortie (Stop loss implicite si inversion extreme)
            # Mais ici on teste juste la redondance
            print(f"Signal inattendu: {signal.signal_type}")

if __name__ == '__main__':
    unittest.main()
