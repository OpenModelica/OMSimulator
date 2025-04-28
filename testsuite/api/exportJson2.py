## status: correct
## teardown_command:
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes

from OMSimulator import SSP, SSD, CRef, Settings

Settings.suppressPath = True

# This example imports a  SSD file and then generates the internal json structure to be used in the simulation
# The SSD file is located in the resources folder of the repository

model = SSD.importFromFile('../resources/dcmotor/SystemStructure.ssd')

model.instantiate() ## internally generate the json file and also set the model state like virgin,

## Result:
## {
##   "simulation units": [
##     {
##       "components": [
##         {
##           "name": [
##             "DC-Motor",
##             "DC-Motor",
##             "SuT",
##             "edrive_mass"
##           ],
##           "type": "me",
##           "path": "resources/edrive_mass.fmu"
##         },
##         {
##           "name": [
##             "DC-Motor",
##             "DC-Motor",
##             "SuT",
##             "emachine_model"
##           ],
##           "type": "me",
##           "path": "resources/emachine_model.fmu"
##         },
##         {
##           "name": [
##             "DC-Motor",
##             "DC-Motor",
##             "stimuli_model"
##           ],
##           "type": "me",
##           "path": "resources/stimuli_model.fmu"
##         }
##       ],
##       "solver": {
##         "type": "co-simulation",
##         "name": null
##       },
##       "connections": [
##         {
##           "start element": "edrive_mass",
##           "start connector": "wB",
##           "end element": "",
##           "end connector": "wB"
##         },
##         {
##           "start element": "",
##           "start connector": "M_load",
##           "end element": "edrive_mass",
##           "end connector": "M_B"
##         },
##         {
##           "start element": "edrive_mass",
##           "start connector": "phiB",
##           "end element": "",
##           "end connector": "phiB"
##         },
##         {
##           "start element": "",
##           "start connector": "U",
##           "end element": "emachine_model",
##           "end connector": "U"
##         },
##         {
##           "start element": "emachine_model",
##           "start connector": "M",
##           "end element": "",
##           "end connector": "M_mot"
##         },
##         {
##           "start element": "emachine_model",
##           "start connector": "I",
##           "end element": "",
##           "end connector": "I"
##         },
##         {
##           "start element": "emachine_model",
##           "start connector": "M",
##           "end element": "edrive_mass",
##           "end connector": "M_A"
##         },
##         {
##           "start element": "edrive_mass",
##           "start connector": "wA",
##           "end element": "emachine_model",
##           "end connector": "w"
##         },
##         {
##           "start element": "edrive_mass",
##           "start connector": "phiA",
##           "end element": "emachine_model",
##           "end connector": "phi"
##         },
##         {
##           "start element": "SuT",
##           "start connector": "M_mot",
##           "end element": "",
##           "end connector": "M_mot"
##         },
##         {
##           "start element": "SuT",
##           "start connector": "wB",
##           "end element": "",
##           "end connector": "wB"
##         },
##         {
##           "start element": "stimuli_model",
##           "start connector": "U",
##           "end element": "",
##           "end connector": "U"
##         },
##         {
##           "start element": "SuT",
##           "start connector": "I",
##           "end element": "",
##           "end connector": "I"
##         },
##         {
##           "start element": "stimuli_model",
##           "start connector": "M_load",
##           "end element": "SuT",
##           "end connector": "M_load"
##         },
##         {
##           "start element": "stimuli_model",
##           "start connector": "U",
##           "end element": "SuT",
##           "end connector": "U"
##         },
##         {
##           "start element": "",
##           "start connector": "M_load",
##           "end element": "stimuli_model",
##           "end connector": "M_load"
##         }
##       ]
##     }
##   ],
##   "result file": "simulation_result.csv",
##   "simulation settings": {
##     "start time": 0,
##     "stop time": 10,
##     "tolerance": 1e-06
##   }
## }
## endResult
