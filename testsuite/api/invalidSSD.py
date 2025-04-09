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

model = SSP('../resources/invalidSSD.ssp')

## Result:
## INFO:OMSimulator.ssp:Temporary directory created: <hidden>
## Traceback (most recent call last):
##   File "C:/OPENMODELICAGIT/OpenModelica/OMSimulator/testsuite/api/invalidSSD.py", line 19, in <module>
##     model = SSP('../resources/invalidSSD.ssp', temp_dir="./invalid-ssd/")
##             ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
##   File "C:/OPENMODELICAGIT/OpenModelica/OMSimulator/install/lib/OMSimulator/ssp.py", line 30, in __init__
##     self._extract_ssp(path)
##   File "C:/OPENMODELICAGIT/OpenModelica/OMSimulator/install/lib/OMSimulator/ssp.py", line 67, in _extract_ssp
##     ssd = SSD.importFromFile(ssd_path, self.resources)
##           ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
##   File "C:/OPENMODELICAGIT/OpenModelica/OMSimulator/install/lib/OMSimulator/ssd.py", line 39, in importFromFile
##     utils.validateSSP(root, filename, "SystemStructureDescription.xsd")
##   File "C:/OPENMODELICAGIT/OpenModelica/OMSimulator/install/lib/OMSimulator/utils.py", line 122, in validateSSP
##     raise Exception(message)
## Exception: Failed to validate SystemStructure.ssd against schemafile SystemStructureDescription.xsd
## ERROR (line 16, column 0): Element '{http://ssp-standard.org/SSP1/SystemStructureDescription}Connector', attribute 'type': The attribute 'type' is not allowed.
## INFO:OMSimulator.ssp:Temporary directory removed: <hidden>
## endResult
