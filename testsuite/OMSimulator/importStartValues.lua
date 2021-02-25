-- status: correct
-- teardown_command: rm -rf importStartValues/
-- linux: yes
-- mingw: yes
-- win: no
-- mac: no

oms_setCommandLineOption("--suppressPath=true --exportParametersInline=false")

status = oms_setTempDirectory("./importStartValues/")

oms_importFile("../resources/importStartValues.ssp");

src1 = oms_exportSnapshot("importStartValues")
print(src1)

oms_setStopTime("importStartValues", 2)

oms_instantiate("importStartValues")

oms_initialize("importStartValues")
oms_simulate("importStartValues")

oms_terminate("importStartValues")

oms_delete("importStartValues")


-- Result:
-- warning: Wrong/deprecated content detected but successfully loaded. Please re-export the SSP file to avoid this message.
-- warning: Wrong/deprecated content detected but successfully loaded. Please re-export the SSP file to avoid this message.
-- warning: Wrong/deprecated content detected but successfully loaded. Please re-export the SSP file to avoid this message.
-- warning: Wrong/deprecated content detected but successfully loaded. Please re-export the SSP file to avoid this message.
-- warning: Wrong/deprecated content detected but successfully loaded. Please re-export the SSP file to avoid this message.
-- error:   [exportSnapshot] Not implemented
--
-- info:    model doesn't contain any continuous state
-- info:    Result file: importStartValues_res.mat (bufferSize=10)
-- info:    5 warnings
-- info:    1 errors
-- endResult
