from dataclasses import dataclass
from pathlib import Path


@dataclass
class _Settings:
  suppressPath: bool = False
  debug_mode: bool = False

Settings = _Settings()


def suppress_path_to_str(path):
  if path is None:
    return path

  if Settings.suppressPath:
    return f'<hidden>/{Path(path).name}'

  return str(Path(path))
