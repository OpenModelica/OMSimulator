## status: correct
## linux: yes
## ucrt64: no
## win: no
## mac: no

## Calibrates a and x_start of HelloWorld (der(x) = a*x, x(0) = x_start)
## against reference data x(t) = exp(-t), i.e., a = -1 and x_start = 1,
## described by Calibration activities of the UQ layered standard:
## cal_01 compares all samples, cal_02 only those from t = 0.5 on, while the
## model still starts at t = 0.

import math
import zipfile

from OMSimulator import SSP, CRef, Capi
from OMSimulator.calibration import calibrateSSP, findUQFiles

Capi.setLogFile("HelloWorldCalibration.log")

with open("reference.csv", "w") as f:
  f.write("time,x|1\n")
  for i in range(11):
    f.write(f"{0.1*i},{math.exp(-0.1*i)}\n")

with open("setup.xml", "w") as f:
  f.write('''<?xml version="1.0" encoding="UTF-8"?>
<uq:UncertaintyQuantification name="HelloWorld"
    xmlns:uq="http://openscaling.org/UQ1/UncertaintyQuantification">
  <uq:Activities>
    <uq:Calibration id="cal_01">
      <uq:ObservedVariables>
        <uq:ObservedVariable name="HelloWorld.x"/>
      </uq:ObservedVariables>
      <uq:SimulationSetting stopTime="1.0" tolerance="1e-8">
        <uq:KeyValues type="org.openmodelica.omsimulator.calibration">
          <uq:KeyValue key="dataName:HelloWorld.x" value="x|1"/>
        </uq:KeyValues>
      </uq:SimulationSetting>
      <uq:ReferenceData>
        <uq:DataSource name="exact" file="reference.csv" type="text/csv"/>
      </uq:ReferenceData>
      <uq:CalibrationTargets>
        <uq:Target parameterName="HelloWorld.a" initialValue="-0.5" lowerBound="-5" upperBound="0"/>
        <uq:Target parameterName="HelloWorld.x_start" initialValue="0.5" lowerBound="0" upperBound="2"/>
      </uq:CalibrationTargets>
    </uq:Calibration>
    <uq:Calibration id="cal_02">
      <uq:ObservedVariables>
        <uq:ObservedVariable name="HelloWorld.x"/>
      </uq:ObservedVariables>
      <uq:SimulationSetting stopTime="1.0" tolerance="1e-8">
        <uq:KeyValues type="org.openmodelica.omsimulator.calibration">
          <uq:KeyValue key="dataName:HelloWorld.x" value="x|1"/>
          <uq:KeyValue key="startTime" value="0.5"/>
        </uq:KeyValues>
      </uq:SimulationSetting>
      <uq:ReferenceData>
        <uq:DataSource name="exact" file="reference.csv" type="text/csv"/>
      </uq:ReferenceData>
      <uq:CalibrationTargets>
        <uq:Target parameterName="HelloWorld.a" initialValue="-0.5" lowerBound="-5" upperBound="0"/>
        <uq:Target parameterName="HelloWorld.x_start" initialValue="0.5" lowerBound="0" upperBound="2"/>
      </uq:CalibrationTargets>
    </uq:Calibration>
  </uq:Activities>
</uq:UncertaintyQuantification>
''')

model = SSP()
model.addResource("../resources/HelloWorld.fmu", new_name="resources/HelloWorld.fmu")
model.addComponent(CRef("default", "HelloWorld"), "resources/HelloWorld.fmu")
model.getElement(CRef("default", "HelloWorld")).implementation = "me"
model.addResource("setup.xml", new_name="extra/uq/setup.xml")
model.addResource("reference.csv", new_name="extra/uq/reference.csv")
model.addMetaDataReference(None, "extra/uq/setup.xml", "quality", "application/x-uncertainty-quantification")

for activity in ("cal_01", "cal_02"):
  result = calibrateSSP(model, activity, output="HelloWorldCalibration.ssp")
  print(f"{activity}: success: {result.success}, samples: {len(result.time)}, from t = {result.time[0]:.1f}")
  for name, value in result.parameters.items():
    print(f"{name}: {value:.4f}")
  for name, value in result.rmse.items():
    print(f"RMSE {name} < 1e-5: {value < 1e-5}")

with zipfile.ZipFile("HelloWorldCalibration.ssp") as ssp:
  print(sorted(n for n in ssp.namelist() if n.startswith("extra/")))
  with ssp.open("extra/uq/cal_02_calibrated.ssv") as ssv:
    print(b'name="HelloWorld.a"' in ssv.read())
  with ssp.open("extra/uq/setup.xml") as uq:
    print(b'<uq:ErrorMetric type="RMSE"' in uq.read())

## the calibrated SSP still refers to its UQ file
print(findUQFiles(SSP("HelloWorldCalibration.ssp")))

## Result:
## cal_01: success: True, samples: 11, from t = 0.0
## HelloWorld.a: -1.0000
## HelloWorld.x_start: 1.0000
## RMSE HelloWorld.x < 1e-5: True
## cal_02: success: True, samples: 6, from t = 0.5
## HelloWorld.a: -1.0000
## HelloWorld.x_start: 1.0000
## RMSE HelloWorld.x < 1e-5: True
## ['extra/uq/cal_01_calibrated.ssv', 'extra/uq/cal_02_calibrated.ssv', 'extra/uq/reference.csv', 'extra/uq/setup.xml']
## True
## True
## ['extra/uq/setup.xml']
## info:    Logging information has been saved to "HelloWorldCalibration.log"
## endResult
