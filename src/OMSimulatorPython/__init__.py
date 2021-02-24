'''OMSimulator

OpenModelica FMI & TLM based simulator'''

from OMSimulator import Scope, Types
from OMSimulator.Model import Model
from OMSimulator.NewAPI import *
from OMSimulator.OMSimulator import OMSimulator
from OMSimulator.System import System

__version__ = '@OMS_SHORT_VERSION_STRING@'
__copyright__ = '''\
Copyright (c) 2018-CurrentYear, Open Source Modelica Consortium (OSMC),
c/o Linköpings universitet, Department of Computer and Information Science,
SE-58183 Linköping, Sweden.'''

Scope._init()
version = Scope._capi.getVersion()
