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
model = SSD.importFromFile('../resources/LOC/SystemStructure.ssd')

model.instantiate() ## internally generate the json file and also set the model state like virgin,

## Result:
## {
##   "simulation units": [
##     {
##       "components": [
##         {
##           "name": [
##             "Root",
##             "System"
##           ],
##           "type": "me_cs",
##           "path": "resources/LOC_SYSTEM_linux.fmu"
##         },
##         {
##           "name": [
##             "Root",
##             "Control"
##           ],
##           "type": "me_cs",
##           "path": "resources/LOC_CNTRL_custom_linux.fmu"
##         }
##       ],
##       "solver": {
##         "type": "co-simulation",
##         "name": null
##       },
##       "connections": [
##         {
##           "start element": [
##             "Root",
##             "System"
##           ],
##           "start connector": "OUTPUT_temperature_lube_oil",
##           "end element": [
##             "Root",
##             "Control"
##           ],
##           "end connector": "INPUT_temperature_lube_oil"
##         },
##         {
##           "start element": [
##             "Root",
##             "Control"
##           ],
##           "start connector": "OUTPUT_control_valve_position",
##           "end element": [
##             "Root",
##             "System"
##           ],
##           "end connector": "INPUT_control_valve_position"
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
