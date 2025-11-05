-- status: correct
-- teardown_command: rm -rf integratorTest_lua/
-- linux: yes
-- ucrt64: yes
-- win: yes
-- mac: no

function readFile(filename)
  local f = assert(io.open(filename, "r"))
  local content = f:read("*all")
  print(content)
  f:close()
end

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./integratorTest_lua/")
oms_setWorkingDirectory("./integratorTest_lua/")

oms_newModel("IntegratorTest")
oms_addSystem("IntegratorTest.root", oms_system_sc)
oms_addSubModel("IntegratorTest.root.integrator", "../../resources/Modelica.Blocks.Continuous.Integrator.fmu")

-- simulation settings
oms_setSolver("IntegratorTest.root", oms_solver_sc_cvode)
oms_setResultFile("IntegratorTest", "integratorTest_lua.csv")
oms_setStopTime("IntegratorTest", 10.0)
oms_setVariableStepSize("IntegratorTest.root", 1.0, 1e-8, 1.0)

oms_instantiate("IntegratorTest")
oms_initialize("IntegratorTest")

oms_setReal("IntegratorTest.root.integrator.u", 0.0)
print(string.format("t = 0.0: y = %g", oms_getReal("IntegratorTest.root.integrator.y")))
oms_stepUntil("IntegratorTest", 2.5)
oms_setReal("IntegratorTest.root.integrator.u", 10.0)

print(string.format("t = 2.5: y = %g", oms_getReal("IntegratorTest.root.integrator.y")))
oms_stepUntil("IntegratorTest", 5.0)
print(string.format("t = 5.0: y = %g", oms_getReal("IntegratorTest.root.integrator.y")))
oms_stepUntil("IntegratorTest", 10.0)
print(string.format("t = 10.0: y = %g", oms_getReal("IntegratorTest.root.integrator.y")))

oms_delete("IntegratorTest")

readFile("integratorTest_lua.csv")

-- Result:
-- info:    maximum step size for 'IntegratorTest.root': 1.000000
-- info:    Result file: integratorTest_lua.csv (bufferSize=1)
-- t = 0.0: y = 0
-- t = 2.5: y = 0
-- t = 5.0: y = 25
-- t = 10.0: y = 75
-- info:    Final Statistics for 'IntegratorTest.root':
--          NumSteps = 16 NumRhsEvals  = 26 NumLinSolvSetups = 13
--          NumNonlinSolvIters = 24 NumNonlinSolvConvFails = 0 NumErrTestFails = 4
-- time,IntegratorTest.root.integrator._D_outputAlias_y,IntegratorTest.root.integrator.der(_D_outputAlias_y),IntegratorTest.root.integrator.local_set,IntegratorTest.root.integrator.u,IntegratorTest.root.integrator.y,IntegratorTest.root.integrator.initType,IntegratorTest.root.integrator.local_reset,IntegratorTest.root.integrator.use_reset,IntegratorTest.root.integrator.use_set
-- 0, 0, 0, 0, 0, 0, 3, 0, 0, 0
-- 1, 0, 0, 0, 0, 0, 3, 0, 0, 0
-- 2, 0, 0, 0, 0, 0, 3, 0, 0, 0
-- 2.5, 0, 0, 0, 0, 0, 3, 0, 0, 0
-- 2.5, 0, 0, 0, 0, 0, 3, 0, 0, 0
-- 2.5001, 0.001, 10, 0, 10, 0.001, 3, 0, 0, 0
-- 2.5002, 0.002, 10, 0, 10, 0.002, 3, 0, 0, 0
-- 2.5012, 0.012, 10, 0, 10, 0.012, 3, 0, 0, 0
-- 2.5112, 0.112, 10, 0, 10, 0.112, 3, 0, 0, 0
-- 2.6112, 1.112, 10, 0, 10, 1.112, 3, 0, 0, 0
-- 3.6112, 11.112, 10, 0, 10, 11.112, 3, 0, 0, 0
-- 4.6112, 21.112, 10, 0, 10, 21.112, 3, 0, 0, 0
-- 5, 25, 10, 0, 10, 25, 3, 0, 0, 0
-- 5, 25, 10, 0, 10, 25, 3, 0, 0, 0
-- 6, 35, 10, 0, 10, 35, 3, 0, 0, 0
-- 7, 45, 10, 0, 10, 45, 3, 0, 0, 0
-- 8, 55, 10, 0, 10, 55, 3, 0, 0, 0
-- 9, 65, 10, 0, 10, 65, 3, 0, 0, 0
-- 10, 75, 10, 0, 10, 75, 3, 0, 0, 0
-- 10, 75, 10, 0, 10, 75, 3, 0, 0, 0
--
-- endResult
