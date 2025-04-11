## status: correct
## teardown_command:
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes

from OMSimulator import SSP, SSD, CRef, Settings

Settings.suppressPath = True


# This example creates a new SSP file with an FMU instantiated as a component and sets two differents solver for the components and the system.
# It then exports the SSP file and re-imports it to verify the solver settings.

model = SSD.importFromFile('../resources/LOC/SystemStructure.ssd')

model.instantiate() ## internally generate the json file and also set the model state like virgin,

## Result:
## {
##   "simulation units": [
##     {
##       "components": [
##         {
##           "name": "System",
##           "type": "TODO",
##           "path": "resources/LOC_SYSTEM_linux.fmu"
##         },
##         {
##           "name": "Control",
##           "type": "TODO",
##           "path": "resources/LOC_CNTRL_custom_linux.fmu"
##         }
##       ],
##       "solver": {
##         "type": "co-simulation"
##       },
##       "connections": [
##         [
##           {
##             "start element": "System",
##             "start connector": "OUTPUT_temperature_lube_oil",
##             "end element": "Control",
##             "end connector": "INPUT_temperature_lube_oil"
##           },
##           {
##             "start element": "Control",
##             "start connector": "OUTPUT_control_valve_position",
##             "end element": "System",
##             "end connector": "INPUT_control_valve_position"
##           }
##         ]
##       ]
##     }
##   ]
## }
## endResult
