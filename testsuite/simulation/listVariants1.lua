-- status: correct
-- teardown_command: rm -rf listvariant_01_lua/
-- linux: yes
-- ucrt64: yes
-- win: no
-- mac: no

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./listvariant_01_lua/")

oms_newModel("model")

oms_addSystem("model.root", oms_system_wc)

oms_addSubModel("model.root.A", "../resources/Modelica.Blocks.Math.Gain.fmu")
oms_setReal("model.root.A.k", 10)

oms_setResultFile("model", "listVariant1.mat")

oms_duplicateVariant("model", "varA")
oms_setReal("varA.root.A.u", -10)

oms_duplicateVariant("varA", "varB")
oms_setReal("varB.root.A.u", -13)
oms_setReal("varB.root.A.k", -100)


print("Info : List Variants before exporting")
variants, status = oms_listVariants("varB")
print(variants)

oms_export("varB", "listVariant1.ssp")

oms_terminate("varB")
oms_delete("varB")

oms_importFile("listVariant1.ssp")

print("Info : List Variants after importing")
variants, status = oms_listVariants("model")
print(variants)

oms_terminate("model")
oms_delete("model")

-- Result:
-- Info : List Variants before exporting
-- <?xml version="1.0"?>
-- <oms:snapshot
--   xmlns:oms="https://raw.githubusercontent.com/OpenModelica/OMSimulator/master/schema/oms.xsd"
--   partial="false">
--   <oms:file
--     name="ssdVariants.xml">
--     <oms:Variants
--       version="2.0">
--       <oms:variant
--         name="varB" />
--       <oms:variant
--         name="varA" />
--       <oms:variant
--         name="model" />
--     </oms:Variants>
--   </oms:file>
-- </oms:snapshot>
--
-- Info : List Variants after importing
-- <?xml version="1.0"?>
-- <oms:snapshot
--   xmlns:oms="https://raw.githubusercontent.com/OpenModelica/OMSimulator/master/schema/oms.xsd"
--   partial="false">
--   <oms:file
--     name="ssdVariants.xml">
--     <oms:Variants
--       version="2.0">
--       <oms:variant
--         name="model" />
--       <oms:variant
--         name="varB" />
--       <oms:variant
--         name="varA" />
--     </oms:Variants>
--   </oms:file>
-- </oms:snapshot>
--
-- endResult
