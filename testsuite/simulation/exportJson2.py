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
##             "SuT",
##             "edrive_mass"
##           ],
##           "type": "me",
##           "path": "resources/edrive_mass.fmu"
##         },
##         {
##           "name": [
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
##           "start element": [
##             "DC-Motor",
##             "SuT",
##             "edrive_mass"
##           ],
##           "start connector": "wB",
##           "end element": [
##             "DC-Motor",
##             "SuT"
##           ],
##           "end connector": "wB"
##         },
##         {
##           "start element": [
##             "DC-Motor",
##             "SuT"
##           ],
##           "start connector": "M_load",
##           "end element": [
##             "DC-Motor",
##             "SuT",
##             "edrive_mass"
##           ],
##           "end connector": "M_B"
##         },
##         {
##           "start element": [
##             "DC-Motor",
##             "SuT",
##             "edrive_mass"
##           ],
##           "start connector": "phiB",
##           "end element": [
##             "DC-Motor",
##             "SuT"
##           ],
##           "end connector": "phiB"
##         },
##         {
##           "start element": [
##             "DC-Motor",
##             "SuT"
##           ],
##           "start connector": "U",
##           "end element": [
##             "DC-Motor",
##             "SuT",
##             "emachine_model"
##           ],
##           "end connector": "U"
##         },
##         {
##           "start element": [
##             "DC-Motor",
##             "SuT",
##             "emachine_model"
##           ],
##           "start connector": "M",
##           "end element": [
##             "DC-Motor",
##             "SuT"
##           ],
##           "end connector": "M_mot"
##         },
##         {
##           "start element": [
##             "DC-Motor",
##             "SuT",
##             "emachine_model"
##           ],
##           "start connector": "I",
##           "end element": [
##             "DC-Motor",
##             "SuT"
##           ],
##           "end connector": "I"
##         },
##         {
##           "start element": [
##             "DC-Motor",
##             "SuT",
##             "emachine_model"
##           ],
##           "start connector": "M",
##           "end element": [
##             "DC-Motor",
##             "SuT",
##             "edrive_mass"
##           ],
##           "end connector": "M_A"
##         },
##         {
##           "start element": [
##             "DC-Motor",
##             "SuT",
##             "edrive_mass"
##           ],
##           "start connector": "wA",
##           "end element": [
##             "DC-Motor",
##             "SuT",
##             "emachine_model"
##           ],
##           "end connector": "w"
##         },
##         {
##           "start element": [
##             "DC-Motor",
##             "SuT",
##             "edrive_mass"
##           ],
##           "start connector": "phiA",
##           "end element": [
##             "DC-Motor",
##             "SuT",
##             "emachine_model"
##           ],
##           "end connector": "phi"
##         },
##         {
##           "start element": [
##             "DC-Motor",
##             "SuT"
##           ],
##           "start connector": "M_mot",
##           "end element": [
##             "DC-Motor"
##           ],
##           "end connector": "M_mot"
##         },
##         {
##           "start element": [
##             "DC-Motor",
##             "SuT"
##           ],
##           "start connector": "wB",
##           "end element": [
##             "DC-Motor"
##           ],
##           "end connector": "wB"
##         },
##         {
##           "start element": [
##             "DC-Motor",
##             "stimuli_model"
##           ],
##           "start connector": "U",
##           "end element": [
##             "DC-Motor"
##           ],
##           "end connector": "U"
##         },
##         {
##           "start element": [
##             "DC-Motor",
##             "SuT"
##           ],
##           "start connector": "I",
##           "end element": [
##             "DC-Motor"
##           ],
##           "end connector": "I"
##         },
##         {
##           "start element": [
##             "DC-Motor",
##             "stimuli_model"
##           ],
##           "start connector": "M_load",
##           "end element": [
##             "DC-Motor",
##             "SuT"
##           ],
##           "end connector": "M_load"
##         },
##         {
##           "start element": [
##             "DC-Motor",
##             "stimuli_model"
##           ],
##           "start connector": "U",
##           "end element": [
##             "DC-Motor",
##             "SuT"
##           ],
##           "end connector": "U"
##         },
##         {
##           "start element": [
##             "DC-Motor"
##           ],
##           "start connector": "M_load",
##           "end element": [
##             "DC-Motor",
##             "stimuli_model"
##           ],
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
## Status.ok
## endResult
