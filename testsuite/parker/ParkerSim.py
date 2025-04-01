## status: correct
## teardown_command: rm -rf tmp-ParkerSim/
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes

import logging
logging.basicConfig(level=logging.INFO)

from OMSimulator import SSD, SSP, Settings

Settings.suppressPath = True

model = SSP('../resources/ParkerSimPack.ssp', temp_dir="./tmp-ParkerSim/")

print('\nAvailable variants:')
print(sorted(model.variants.keys()))

print('\nActive variant:')
model.activeVariant.list()

print('\nVariant D16_aero_stimuli:')
model.variants['D16_aero_stimuli'].list()

## Result:
## INFO:OMSimulator.ssp:Temporary directory created: <hidden>
## Traceback (most recent call last):
##   File "/home/lochel/OMSimulator/testsuite/parker/ParkerSim.py", line 15, in <module>
##     model = SSP('../resources/ParkerSimPack.ssp', temp_dir="./tmp-ParkerSim/")
##             ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
##   File "/home/lochel/OMSimulator/install/lib/x86_64-linux-gnu/OMSimulator/ssp.py", line 28, in __init__
##     self._extract_ssp(path)
##   File "/home/lochel/OMSimulator/install/lib/x86_64-linux-gnu/OMSimulator/ssp.py", line 47, in _extract_ssp
##     raise FileNotFoundError(f"SSP file '{path}' not found")
## FileNotFoundError: SSP file '../resources/ParkerSimPack.ssp' not found
## INFO:OMSimulator.ssp:Temporary directory removed: <hidden>
## endResult
