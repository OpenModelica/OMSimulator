'''
OMSimulator

OpenModelica FMI & SSP based simulator.
'''

from OMSimulator import capi
from OMSimulator.cref import CRef
from OMSimulator.fmu import FMU
from OMSimulator.ssd import SSD
from OMSimulator.ssp import SSP
from OMSimulator.system import System
from OMSimulator.variable import Causality, SignalType, Variable

# Define public API
__all__ = ['CRef', 'FMU', 'SSD', 'SSP', 'System', 'Causality', 'SignalType', 'Variable']

__version__ = '@OMS_SHORT_VERSION_STRING@'
__author__ = 'Open Source Modelica Consortium (OSMC)'
__license__ = 'OSMC-PL (OSMC Public License)'
__copyright__ = '''\
Copyright (c) 2018-CurrentYear, Open Source Modelica Consortium (OSMC),
c/o Linköpings universitet, Department of Computer and Information Science,
SE-58183 Linköping, Sweden.'''

version = capi.capi().getVersion()
