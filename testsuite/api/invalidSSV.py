## status: correct
## teardown_command:
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes

## This example demonstrates how to export an SSD file from an SSP file.
## the main purpose of this example is to parse all the elements in the SSD file
## and export it to a file.

import logging
logging.basicConfig(level=logging.INFO)

from OMSimulator import SSD, SSP, Settings

Settings.suppressPath = True

try:
  model = SSP('../resources/invalidSSV.ssp')
except Exception as e:
  print(f"Error: {e}")

## Result:
## INFO:OMSimulator.ssp:Temporary directory created: <hidden>
## UserWarning: Failed to validate System1.ssv against schemafile SystemStructureParameterValues.xsd
## ERROR (line 4, column 0): Element '{http://ssp-standard.org/SSP1/SystemStructureParameterValues}Parameter', attribute 'type': The attribute 'type' is not allowed.
## INFO:OMSimulator.ssp:Temporary directory removed: <hidden>
## endResult
