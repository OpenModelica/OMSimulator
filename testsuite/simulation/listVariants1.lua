-- status: correct
-- teardown_command: rm -rf listvariant_01_lua/
-- linux: yes
-- mingw32: no
-- mingw64: yes
-- win: no
-- mac: no

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./listvariant_01_lua/")

oms_newModel("model")

oms_addSystem("model.root", oms_system_wc)

oms_addSubModel("model.root.A", "../resources/Modelica.Blocks.Math.Gain.fmu")
oms_setReal("model.root.A.k", 10)

oms_duplicateVariant("model", "varA")
oms_setReal("model.root.A.u", -10)

oms_duplicateVariant("model", "varB")
oms_setReal("model.root.A.u", -13)
oms_setReal("model.root.A.k", -100)

oms_setResultFile("model", "listVariant1.mat")


oms_export("model", "listVariant1.ssp")

oms_terminate("model")
oms_delete("model")

oms_importFile("listVariant1.ssp")

variants, status = oms_listVariants("model")
print(variants)

oms_terminate("model")
oms_delete("model")

-- Result:
-- <?xml version="1.0"?>
-- <oms:snapshot
--   xmlns:oms="https://raw.githubusercontent.com/OpenModelica/OMSimulator/master/schema/oms.xsd"
--   partial="false">
--   <oms:file
--     name="ssdVariants.xml">
--     <oms:Variants
--       version="1.0">
--       <oms:variant
--         name="model" />
--       <oms:variant
--         name="varA" />
--       <oms:variant
--         name="varB" />
--     </oms:Variants>
--   </oms:file>
-- </oms:snapshot>
--
-- endResult
