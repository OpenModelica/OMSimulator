## status: correct
## teardown_command: rm -rf parker_simulation_res.mat
## linux: no
## ucrt64: yes
## win: yes
## mac: no

import logging
logging.basicConfig(level=logging.INFO)

from OMSimulator import SSD, SSP, Settings

Settings.suppressPath = True

model = SSP('../resources/ParkerSimPack.ssp')
# model.list()

instantiated_model = model.instantiate() ## internally generate the json file and also set the model state like virgin,
print("", flush=True)
instantiated_model.setResultFile("parker_simulation_res.mat")

instantiated_model.initialize()
instantiated_model.simulate()
instantiated_model.terminate()
instantiated_model.delete()

## Result:
## INFO:OMSimulator.ssp:Temporary directory created: <hidden>
##
## info:    Alg. loop (size 13/57)
##            model.root.valve_model
##            model.root.interface_model
## info:    Result file: parker_simulation_res.mat (bufferSize=1)
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 0
## Setting hidden to 0
## Setting hidden to 0
## Setting hidden to 0
## Setting hidden to 0
## Setting hidden to 0
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## Setting hidden to 1
## INFO:OMSimulator.ssp:Temporary directory removed: <hidden>
## endResult
