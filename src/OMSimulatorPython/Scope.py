from OMSimulator.capi import capi

def _init():
  global _capi, _Scope
  _capi = capi()
  _Scope = list()
