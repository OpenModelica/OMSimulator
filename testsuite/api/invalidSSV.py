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

model = SSP('../resources/invalidSSV.ssp')

## Result:
## INFO:OMSimulator.ssp:Temporary directory created: <hidden>
## Traceback (most recent call last):
##   File "C:/OPENMODELICAGIT/OpenModelica/OMSimulator/testsuite/api/invalidSSV.py", line 19, in <module>
##     model = SSP('../resources/invalidSSV.ssp', temp_dir="./invalid-ssv/")
##             ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
##   File "C:/OPENMODELICAGIT/OpenModelica/OMSimulator/install/lib/OMSimulator/ssp.py", line 30, in __init__
##     self._extract_ssp(path)
##   File "C:/OPENMODELICAGIT/OpenModelica/OMSimulator/install/lib/OMSimulator/ssp.py", line 60, in _extract_ssp
##     self._addResource(path, path.relative_to(self.temp_dir), copy=False)
##   File "C:/OPENMODELICAGIT/OpenModelica/OMSimulator/install/lib/OMSimulator/ssp.py", line 107, in _addResource
##     self.resources[str(new_name)] = SSV(ssv_path = filePath)
##                                     ^^^^^^^^^^^^^^^^^^^^^^^^
##   File "C:/OPENMODELICAGIT/OpenModelica/OMSimulator/install/lib/OMSimulator/ssv.py", line 15, in __init__
##     self.importFromSSV(self.filename)
##   File "C:/OPENMODELICAGIT/OpenModelica/OMSimulator/install/lib/OMSimulator/ssv.py", line 42, in importFromSSV
##     parameterValues = utils.parseSSV(filename)
##                       ^^^^^^^^^^^^^^^^^^^^^^^^
##   File "C:/OPENMODELICAGIT/OpenModelica/OMSimulator/install/lib/OMSimulator/utils.py", line 90, in parseSSV
##     validateSSP(root, filename, "SystemStructureParameterValues.xsd")
##   File "C:/OPENMODELICAGIT/OpenModelica/OMSimulator/install/lib/OMSimulator/utils.py", line 122, in validateSSP
##     raise Exception(message)
## Exception: Failed to validate System1.ssv against schemafile SystemStructureParameterValues.xsd
## ERROR (line 4, column 0): Element '{http://ssp-standard.org/SSP1/SystemStructureParameterValues}Parameter', attribute 'type': The attribute 'type' is not allowed.
## INFO:OMSimulator.ssp:Temporary directory removed: <hidden>
## endResult
