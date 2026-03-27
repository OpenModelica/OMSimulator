## status: correct
## teardown_command: rm -rf metadata.ssp
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes

from OMSimulator import SSP, CRef, Settings, SSV, SignalType

Settings.suppressPath = True

## this examples add ssp meta data resources


model = SSP()
model.addResource('../resources/acvs_compositeModel/resources/L90LS_UD_OS_OS.fmu', new_name='resources/L90LS_UD_OS_OS.fmu')
model.addResource('../resources/acvs_compositeModel/resources/od_L90LS_UD_OS_OS.xml', new_name='resources/od_L90LS_UD_OS_OS.xml')

model.addComponent(CRef('default', 'L90LS_UD_OS_OS'), 'resources/L90LS_UD_OS_OS.fmu')
model.addMetaDataReference(CRef('default', 'L90LS_UD_OS_OS'),'resources/od_L90LS_UD_OS_OS.xml', type="application/x-uncertainty-quantification")
model.list()
model.export('metadata.ssp')

# model2 = SSP('metadata.ssp')
# model2.list()


## Result:
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   resources/L90LS_UD_OS_OS.fmu
## |--   resources/od_L90LS_UD_OS_OS.xml
## |-- Active Variant: default
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Variant "default": None
## |-- |-- System: default 'None'
## |-- |-- |-- Connectors:
## |-- |-- |-- Elements:
## |-- |-- |-- |-- FMU: L90LS_UD_OS_OS 'None'
## |-- |-- |-- |-- |-- path: resources/L90LS_UD_OS_OS.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (uA, Causality.input, SignalType.Real, None, 'Valve input A')
## |-- |-- |-- |-- |-- |-- (uB, Causality.input, SignalType.Real, None, 'Valve input B')
## |-- |-- |-- |-- |-- |-- (pP, Causality.input, SignalType.Real, None, 'Pump pressure')
## |-- |-- |-- |-- |-- |-- (pTA, Causality.input, SignalType.Real, None, 'Tank port A pressure')
## |-- |-- |-- |-- |-- |-- (pTB, Causality.input, SignalType.Real, None, 'Tank port B pressure')
## |-- |-- |-- |-- |-- |-- (pA, Causality.input, SignalType.Real, None, 'Port A pressure')
## |-- |-- |-- |-- |-- |-- (pB, Causality.input, SignalType.Real, None, 'Port B pressure')
## |-- |-- |-- |-- |-- |-- (pLin, Causality.input, SignalType.Real, None, 'Input load pressure')
## |-- |-- |-- |-- |-- |-- (pLmax, Causality.input, SignalType.Real, None, 'Max load pressure')
## |-- |-- |-- |-- |-- |-- (qA, Causality.output, SignalType.Real, None, 'Port A flow')
## |-- |-- |-- |-- |-- |-- (qB, Causality.output, SignalType.Real, None, 'Port B flow')
## |-- |-- |-- |-- |-- |-- (qP, Causality.output, SignalType.Real, None, 'Pump flow')
## |-- |-- |-- |-- |-- |-- (qTA, Causality.output, SignalType.Real, None, 'Tankport A flow')
## |-- |-- |-- |-- |-- |-- (qTB, Causality.output, SignalType.Real, None, 'Tankport B flow')
## |-- |-- |-- |-- |-- |-- (pL, Causality.output, SignalType.Real, None, 'Load pressure')
## |-- |-- |-- |-- |-- |-- (p_TP, Causality.input, SignalType.Real, None, 'Pilot tank pressure')
## |-- |-- |-- |-- |-- |-- (p_PS, Causality.input, SignalType.Real, None, 'Pilot pressure supply')
## |-- |-- |-- |-- |-- |-- (d_A, Causality.parameter, SignalType.Real, None, 'Pilot A damping orifice diameter')
## |-- |-- |-- |-- |-- |-- (d_B, Causality.parameter, SignalType.Real, None, 'Pilot B damping orifice diameter')
## |-- |-- |-- |-- |-- |-- (Debug, Causality.output, SignalType.Real, None, 'Debug channel (0 = OK)')
## |-- |-- |-- |-- |-- |-- (KqMO_PABT_gain, Causality.parameter, SignalType.Real, None, 'Flow trim coefficient Meter-out P->A')
## |-- |-- |-- |-- |-- |-- (KqMI_PABT_gain, Causality.parameter, SignalType.Real, None, 'Flow trim coefficient Meter-in P->A')
## |-- |-- |-- |-- |-- |-- (KqMO_PBAT_gain, Causality.parameter, SignalType.Real, None, 'Flow trim coefficient Meter-out P->B')
## |-- |-- |-- |-- |-- |-- (KqMI_PBAT_gain, Causality.parameter, SignalType.Real, None, 'Flow trim coefficient Meter-in P->B')
## |-- |-- |-- |-- |-- MetaData : resources/od_L90LS_UD_OS_OS.xml
## |-- UnitDefinitions:
## |-- |-- Unit: mm
## |-- |-- |-- BaseUnit: m: 1, factor: 1e-3
## |-- |-- Unit: mm2
## |-- |-- |-- BaseUnit: m: 2, factor: 1e-6
## |-- |-- Unit: mA
## |-- |-- |-- BaseUnit: A: 1, factor: 1e-3
## |-- |-- Unit: L/min
## |-- |-- |-- BaseUnit: m: 3, s: -1, factor: 6e-4
## |-- |-- Unit: kg/m3
## |-- |-- |-- BaseUnit: kg: 1, m: -3
## |-- |-- Unit: bar
## |-- |-- |-- BaseUnit: kg: 1, m: -1, s: -2, factor: 1e5, offset: 0
## |-- |-- Unit: deg
## |-- |-- |-- BaseUnit: rad: 1, factor: 0.017453292519943, offset: 0
## |-- |-- Unit: N
## |-- |-- |-- BaseUnit: kg: 1, m: 1, s: -2, factor: 1, offset: 0
## |-- |-- Unit: Hz
## |-- |-- |-- BaseUnit: s: -1, factor: 1, offset: 0
## |-- |-- Unit: -
## |-- |-- |-- BaseUnit: factor: 1, offset: 0
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
## endResult
