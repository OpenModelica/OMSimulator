## status: correct
## teardown_command: rm myfile1.ssv myfile2.ssv
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes

from OMSimulator import SSP, CRef, Settings, SSV

Settings.suppressPath = True

# This example creates a new SSV file and set some parameter values and export it to filesystem

ssv1 = SSV()
ssv1.setValue("k1", 2.0, "m")
ssv1.setValue("k2", 3.0)
ssv1.setValue("k3", 3)
ssv1.setValue("k4", False)
ssv1.setValue("param3", "hello")
ssv1.export("myfile1.ssv")


ssv2 = SSV()
ssv2.setValue("k1", 20.0, "m")
ssv2.setValue("k2", 30.0, "kg")
ssv2.setValue("param3", "helloSSP")
ssv2.export("myfile2.ssv")

files = ["myfile1.ssv", "myfile2.ssv"]
for filename in files:
  print(f"\n{filename}:")
  print("============")
  with open(filename, "r", encoding="utf-8") as file:
    print(file.read())

## Result:
##
## myfile1.ssv:
## ============
## <?xml version='1.0' encoding='utf-8'?>
## <ssv:ParameterSet xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues" version="2.0" name="parameters">
##   <ssv:Parameters>
##     <ssv:Parameter name="k1">
##       <ssv:Real value="2.0" unit="m"/>
##     </ssv:Parameter>
##     <ssv:Parameter name="k2">
##       <ssv:Real value="3.0"/>
##     </ssv:Parameter>
##     <ssv:Parameter name="k3">
##       <ssv:Integer value="3"/>
##     </ssv:Parameter>
##     <ssv:Parameter name="k4">
##       <ssv:Boolean value="false"/>
##     </ssv:Parameter>
##     <ssv:Parameter name="param3">
##       <ssv:String value="hello"/>
##     </ssv:Parameter>
##   </ssv:Parameters>
## </ssv:ParameterSet>
##
##
## myfile2.ssv:
## ============
## <?xml version='1.0' encoding='utf-8'?>
## <ssv:ParameterSet xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues" version="2.0" name="parameters">
##   <ssv:Parameters>
##     <ssv:Parameter name="k1">
##       <ssv:Real value="20.0" unit="m"/>
##     </ssv:Parameter>
##     <ssv:Parameter name="k2">
##       <ssv:Real value="30.0" unit="kg"/>
##     </ssv:Parameter>
##     <ssv:Parameter name="param3">
##       <ssv:String value="helloSSP"/>
##     </ssv:Parameter>
##   </ssv:Parameters>
## </ssv:ParameterSet>
##
## endResult
