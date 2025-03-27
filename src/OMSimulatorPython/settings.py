from dataclasses import dataclass
from pathlib import Path


@dataclass
class _Settings:
  suppressPath: bool = False
  debug_mode: bool = False

Settings = _Settings()


def suppress_path_to_str(path):
  if path is None:
    return str(None)

  if Settings.suppressPath:
    return '<hidden>'

  return str(path)


def suppress_relative_path_to_str(path, temp_directory):
  if path is None:
    return str(None)

  if Settings.suppressPath and temp_directory is not None:
    return f'<hidden>/{Path(path).relative_to(temp_directory)}'

  if Settings.suppressPath:
    return '<hidden>'

  return str(path)
