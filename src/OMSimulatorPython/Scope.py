from OMSimulator import Types
from OMSimulator.capi import capi


def _loggingCallback(message_type, message):
  global _Logging
  message_type = Types.MessageType(message_type)
  if message_type in (Types.MessageType.WARNING, Types.MessageType.ERROR):
    message = message.decode("utf-8")
    _Logging.append((message_type, message))

def clearLoggingBuffer():
  global _Logging
  _Logging.clear()

def _init():
  global _capi, _Scope, _Logging
  _capi = capi()
  _Scope = list()
  _Logging = list()

  _capi.setLoggingCallback(_loggingCallback)
