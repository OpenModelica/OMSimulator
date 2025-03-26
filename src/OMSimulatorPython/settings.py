from dataclasses import dataclass

@dataclass
class _Settings:
    suppressPath: bool = False
    debug_mode: bool = False

Settings = _Settings()
