-- status: correct
-- teardown_command: rm -rf BouncingBall-lua/
-- linux: yes
-- ucrt64: yes
-- win: yes
-- mac: yes
-- asan: yes

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./BouncingBall-lua/")
oms_setWorkingDirectory("./BouncingBall-lua/")

oms_newModel("model")
oms_addSystem("model.root", oms_system_wc)

oms_addSubModel("model.root.BouncingBall", "../../resources/BouncingBall.fmu")



oms_setCommandLineOption('--suppressPath=true')
oms_setTempDirectory('./validateSRMD-lua/')
oms_setWorkingDirectory("./validateSRMD-lua/")

model = oms_importFile('../resources/invalidSRMD.ssp')
print(model)
--oms_terminate()
--oms_delete()

