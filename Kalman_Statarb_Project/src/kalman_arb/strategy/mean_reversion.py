from typing import Optional
from ..core.kalman import KalmanFilter
from ..backtest.events import MarketEvent, SignalEvent, SignalType

class KalmanMeanReversionStrategy:
    """
    Stratégie de retour à la moyenne basée sur le Z-Score du filtre de Kalman.
    
    Logique (White Paper Section 6.2):
    - Entrée Long Spread : Z-Score < -entry_std
    - Entrée Short Spread : Z-Score > +entry_std
    - Sortie (Take Profit/Stop) : |Z-Score| < exit_std
    """

    def __init__(self, entry_std: float = 1.0, exit_std: float = 0.0):
        self.entry_std = entry_std
        self.exit_std = exit_std
        
      
        self.kf = KalmanFilter(R=0.01, delta=1e-5)
        
        self.current_position = 0

    def calculate_signal(self, event: MarketEvent) -> Optional[SignalEvent]:
        """
        Traite un nouvel événement de marché et retourne un SignalEvent si nécessaire.
        """
        # 1. Mise à jour du Filtre avec les nouveaux prix
        metrics = self.kf.update(y=event.price_y, x=event.price_x)
        
        z_score = metrics['z_score']
        beta = metrics['beta']
        timestamp = event.timestamp
        
        signal = None

        # 2. Logique décisionnelle (State Machine)
        
        # CAS A : Nous sommes FLAT (Cash)
        if self.current_position == 0:
            # Le spread est trop bas -> Opportunité d'achat
            if z_score < -self.entry_std:
                signal = SignalEvent(
                    timestamp=timestamp,
                    signal_type=SignalType.LONG_SPREAD,
                    z_score=z_score,
                    estimated_beta=beta
                )
                self.current_position = 1
            
            # Le spread est trop haut -> Opportunité de vente
            elif z_score > self.entry_std:
                signal = SignalEvent(
                    timestamp=timestamp,
                    signal_type=SignalType.SHORT_SPREAD,
                    z_score=z_score,
                    estimated_beta=beta
                )
                self.current_position = -1

        # CAS B : Nous sommes LONG SPREAD
        elif self.current_position == 1:
            # Retour à la normale (ou inversion brutale) -> Sortie
            if z_score > -self.exit_std:
                signal = SignalEvent(
                    timestamp=timestamp,
                    signal_type=SignalType.EXIT,
                    z_score=z_score,
                    estimated_beta=beta
                )
                self.current_position = 0

        # CAS C : Nous sommes SHORT SPREAD
        elif self.current_position == -1:
            # Retour à la normale -> Sortie
            if z_score < self.exit_std:
                signal = SignalEvent(
                    timestamp=timestamp,
                    signal_type=SignalType.EXIT,
                    z_score=z_score,
                    estimated_beta=beta
                )
                self.current_position = 0
                
        return signal
