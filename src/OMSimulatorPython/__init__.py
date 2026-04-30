# This file is part of OpenModelica.
#
# Copyright (c) 1998-2026, Open Source Modelica Consortium (OSMC),
# c/o Linköpings universitet, Department of Computer and Information Science,
# SE-58183 Linköping, Sweden.
#
# All rights reserved.
#
# THIS PROGRAM IS PROVIDED UNDER THE TERMS OF AGPL VERSION 3 LICENSE OR
# THIS OSMC PUBLIC LICENSE (OSMC-PL) VERSION 1.8.
# ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES
# RECIPIENT'S ACCEPTANCE OF THE OSMC PUBLIC LICENSE OR THE GNU AGPL
# VERSION 3, ACCORDING TO RECIPIENTS CHOICE.
#
# The OpenModelica software and the OSMC (Open Source Modelica Consortium)
# Public License (OSMC-PL) are obtained from OSMC, either from the above
# address, from the URLs:
# http://www.openmodelica.org or
# https://github.com/OpenModelica/ or
# http://www.ida.liu.se/projects/OpenModelica,
# and in the OpenModelica distribution.
#
# GNU AGPL version 3 is obtained from:
# https://www.gnu.org/licenses/licenses.html#GPL
#
# This program is distributed WITHOUT ANY WARRANTY; without
# even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
# IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
#
# See the full OSMC Public License conditions for more details.

'''
OMSimulator

OpenModelica FMI & SSP based simulator.
'''

from OMSimulator.capi import Capi
from OMSimulator.connection import Connection
from OMSimulator.connector import Connector
from OMSimulator.cref import CRef
from OMSimulator.fmu import FMU
from OMSimulator.settings import Settings
from OMSimulator.ssd import SSD
from OMSimulator.ssp import SSP
from OMSimulator.ssv import SSV
from OMSimulator.ssm import SSM
from OMSimulator.system import System
from OMSimulator.variable import Causality, SignalType, Variable, Float32, Float64, Int8, Int16, Int32, Int64, UInt8, UInt16, UInt32, UInt64
from OMSimulator.instantiated_model import InstantiatedModel

# Define public API
__all__ = [
    'Capi',
    'Causality',
    'Connection',
    'Connector',
    'CRef',
    'FMU',
    'Float32',
    'Float64',
    'Int8',
    'Int16',
    'Int32',
    'Int64',
    'UInt8',
    'UInt16',
    'UInt32',
    'UInt64',
    'InstantiatedModel',
    'Settings',
    'SignalType',
    'SSD',
    'SSM',
    'SSP',
    'SSV',
    'System',
    'Variable',
  ]

__version__ = '@OMS_SHORT_VERSION_STRING@'
__author__ = 'Open Source Modelica Consortium (OSMC)'
__license__ = 'OSMC-PL (OSMC Public License)'
__copyright__ = '''\
Copyright (c) 2018-CurrentYear, Open Source Modelica Consortium (OSMC),
c/o Linköpings universitet, Department of Computer and Information Science,
SE-58183 Linköping, Sweden.'''

version = Capi.getVersion()
