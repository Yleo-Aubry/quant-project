from dataclasses import dataclass
from enum import Enum
from datetime import datetime

class SignalType(Enum):
    """Types de signaux standardisés pour éviter les erreurs de chaînes de caractères."""
    LONG_SPREAD = "LONG_SPREAD"   # Acheter Y, Vendre Beta * X
    SHORT_SPREAD = "SHORT_SPREAD" # Vendre Y, Acheter Beta * X
    EXIT = "EXIT"                 # Fermer toutes les positions

@dataclass
class MarketEvent:
    """
    Représente un tick ou une barre de prix reçue du marché.
    """
    timestamp: datetime
    symbol_y: str
    price_y: float
    symbol_x: str
    price_x: float

@dataclass
class SignalEvent:
    """
    Représente une décision de trading prise par la stratégie.
    """
    timestamp: datetime
    signal_type: SignalType
    z_score: float        # Force du signal (pour analyse post-trade)
    estimated_beta: float # Ratio de couverture à utiliser pour l'exécution
