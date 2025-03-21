'''OMSimulator

OpenModelica FMI & SSP based simulator'''

from OMSimulator import types, capi
from OMSimulator.connection import Connection
from OMSimulator.connector import Connector
from OMSimulator.fmu import FMU
from OMSimulator.ssd import SSD
from OMSimulator.ssp import SSP
from OMSimulator.ssv import SSV
from OMSimulator.unit import UnitDefinitions
from OMSimulator.values import Values
from OMSimulator.variables import Variable

# Define what gets imported when using `from OMSimulator import *`
__all__ = [
  "SSP",
  "SSD",
  "SSV",
  "FMU",
  "Values",
  "Variable",
  "Connector",
  "types",
  "importSSP"
]

__version__ = '@OMS_SHORT_VERSION_STRING@'
__copyright__ = '''\
Copyright (c) 2018-CurrentYear, Open Source Modelica Consortium (OSMC),
c/o Linköpings universitet, Department of Computer and Information Science,
SE-58183 Linköping, Sweden.'''

version = capi().getVersion()
