
set(OM_OMS_TESTSUITE_RESOURCES_FMUS
        ${CMAKE_CURRENT_SOURCE_DIR}/A
        ${CMAKE_CURRENT_SOURCE_DIR}/B
        ${CMAKE_CURRENT_SOURCE_DIR}/AircraftVehicleDemonstrator.Atmosphere_Model_FMU_sf
        ${CMAKE_CURRENT_SOURCE_DIR}/AircraftVehicleDemonstrator.bC
        ${CMAKE_CURRENT_SOURCE_DIR}/AircraftVehicleDemonstrator.cockpit
        ${CMAKE_CURRENT_SOURCE_DIR}/AircraftVehicleDemonstrator.consumer_A
        ${CMAKE_CURRENT_SOURCE_DIR}/AircraftVehicleDemonstrator.consumer_B
        ${CMAKE_CURRENT_SOURCE_DIR}/AircraftVehicleDemonstrator.eCS_Generic_Export
        ${CMAKE_CURRENT_SOURCE_DIR}/AircraftVehicleDemonstrator.eCS_SW
        ${CMAKE_CURRENT_SOURCE_DIR}/AircraftVehicleDemonstrator.engine
        ${CMAKE_CURRENT_SOURCE_DIR}/ASSCExample.Class1
        ${CMAKE_CURRENT_SOURCE_DIR}/ASSCExample.Class2
        ${CMAKE_CURRENT_SOURCE_DIR}/DualMassOscillator.System1
        ${CMAKE_CURRENT_SOURCE_DIR}/DualMassOscillator.System2
        ${CMAKE_CURRENT_SOURCE_DIR}/Enum1
        ${CMAKE_CURRENT_SOURCE_DIR}/equationPair.equation1
        ${CMAKE_CURRENT_SOURCE_DIR}/equationPair.equation2
        ${CMAKE_CURRENT_SOURCE_DIR}/fmidertest
        ${CMAKE_CURRENT_SOURCE_DIR}/fmi_attributes_19
        ${CMAKE_CURRENT_SOURCE_DIR}/HelloWorld
        ${CMAKE_CURRENT_SOURCE_DIR}/HelloWorldWithInput
        ${CMAKE_CURRENT_SOURCE_DIR}/Int1
        ${CMAKE_CURRENT_SOURCE_DIR}/Lin2DimODE
        ${CMAKE_CURRENT_SOURCE_DIR}/Modelica.Blocks.Continuous.Integrator
        ${CMAKE_CURRENT_SOURCE_DIR}/Modelica.Blocks.Math.Add
        ${CMAKE_CURRENT_SOURCE_DIR}/Modelica.Blocks.Math.Add3
        ${CMAKE_CURRENT_SOURCE_DIR}/Modelica.Blocks.Math.Feedback
        ${CMAKE_CURRENT_SOURCE_DIR}/Modelica.Blocks.Math.Gain
        ${CMAKE_CURRENT_SOURCE_DIR}/Modelica.Blocks.Math.Product
        ${CMAKE_CURRENT_SOURCE_DIR}/Modelica.Blocks.Nonlinear.Limiter
        ${CMAKE_CURRENT_SOURCE_DIR}/Modelica.Blocks.Sources.Clock
        ${CMAKE_CURRENT_SOURCE_DIR}/Modelica.Blocks.Sources.Constant
        ${CMAKE_CURRENT_SOURCE_DIR}/Modelica.Blocks.Sources.Sine
        ${CMAKE_CURRENT_SOURCE_DIR}/Modelica.Blocks.Sources.Step
        ${CMAKE_CURRENT_SOURCE_DIR}/Modelica.Electrical.Analog.Examples.CauerLowPassAnalog
        ${CMAKE_CURRENT_SOURCE_DIR}/nls.cs
        ${CMAKE_CURRENT_SOURCE_DIR}/nls.me
        ${CMAKE_CURRENT_SOURCE_DIR}/QuarterCarModel.DisplacementDisplacement.Chassis
        ${CMAKE_CURRENT_SOURCE_DIR}/QuarterCarModel.DisplacementDisplacement.Wheel
        ${CMAKE_CURRENT_SOURCE_DIR}/QuarterCarModel.DisplacementForce.Chassis
        ${CMAKE_CURRENT_SOURCE_DIR}/QuarterCarModel.DisplacementForce.Wheel
        ${CMAKE_CURRENT_SOURCE_DIR}/replaceA
        ${CMAKE_CURRENT_SOURCE_DIR}/replaceB
        ${CMAKE_CURRENT_SOURCE_DIR}/replaceA_extended
        ${CMAKE_CURRENT_SOURCE_DIR}/str_hello_world
        ${CMAKE_CURRENT_SOURCE_DIR}/tank1
        ${CMAKE_CURRENT_SOURCE_DIR}/tank2
        ${CMAKE_CURRENT_SOURCE_DIR}/tank3
        ${CMAKE_CURRENT_SOURCE_DIR}/tlm.adder
        ${CMAKE_CURRENT_SOURCE_DIR}/tlm.CoarseGrained1_1D
        ${CMAKE_CURRENT_SOURCE_DIR}/tlm.CoarseGrained2_1D
        ${CMAKE_CURRENT_SOURCE_DIR}/tlm.FineGrained1_1D
        ${CMAKE_CURRENT_SOURCE_DIR}/tlm.FineGrained2_1D
        ${CMAKE_CURRENT_SOURCE_DIR}/tlm.gain
        ${CMAKE_CURRENT_SOURCE_DIR}/tlm.LowerPendulum
        ${CMAKE_CURRENT_SOURCE_DIR}/tlm.LowerPendulumFG
        ${CMAKE_CURRENT_SOURCE_DIR}/tlm.mass
        ${CMAKE_CURRENT_SOURCE_DIR}/tlm.massSpring
        ${CMAKE_CURRENT_SOURCE_DIR}/tlm.source
        ${CMAKE_CURRENT_SOURCE_DIR}/tlm.UpperPendulum
        ${CMAKE_CURRENT_SOURCE_DIR}/ypipe
)

set(OM_OMS_TESTSUITE_RESOURCES_SSPS
        ${CMAKE_CURRENT_SOURCE_DIR}/embrace
        ${CMAKE_CURRENT_SOURCE_DIR}/embrace_TwoConf
        ${CMAKE_CURRENT_SOURCE_DIR}/import_hierarchical_ssv_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/importParameterMapping
        ${CMAKE_CURRENT_SOURCE_DIR}/importParameterMappingInline
        ${CMAKE_CURRENT_SOURCE_DIR}/importStartValues
        ${CMAKE_CURRENT_SOURCE_DIR}/importStartValues1
        ${CMAKE_CURRENT_SOURCE_DIR}/import_export_parameters1
        ${CMAKE_CURRENT_SOURCE_DIR}/import_export_parameters2
        ${CMAKE_CURRENT_SOURCE_DIR}/import_export_parameters3
        ${CMAKE_CURRENT_SOURCE_DIR}/importExportAllResources
        ${CMAKE_CURRENT_SOURCE_DIR}/invalidSSP
        ${CMAKE_CURRENT_SOURCE_DIR}/replaceSubmodel4
        ${CMAKE_CURRENT_SOURCE_DIR}/replaceSubmodel5
)

## Extract FMUs
foreach(fmu_directory ${OM_OMS_TESTSUITE_RESOURCES_FMUS})
  get_filename_component(fmu_name ${fmu_directory} NAME)
  set(output_file_path ${CMAKE_CURRENT_SOURCE_DIR}/${fmu_name}.fmu)

  add_custom_command(
    # Use the modelDescrition.xml file as a dependency
    DEPENDS ${fmu_directory}/modelDescription.xml
    COMMAND cd ${fmu_directory}
    COMMAND zip -r ${output_file_path} *
    COMMAND cd ..
    OUTPUT ${output_file_path}
    COMMENT "Creating FMU ${output_file_path}"
  )

  set(OM_OMS_TESTSUITE_EXTRACTED_FMU_FILES ${OM_OMS_TESTSUITE_EXTRACTED_FMU_FILES} ${output_file_path})
endforeach()

## Extract SSPs
foreach(fmu_directory ${OM_OMS_TESTSUITE_RESOURCES_SSPS})
  get_filename_component(fmu_name ${fmu_directory} NAME)
  set(output_file_path ${CMAKE_CURRENT_SOURCE_DIR}/${fmu_name}.ssp)

  add_custom_command(
    # Use the SystemStructure.ssd file as a dependency
    DEPENDS ${fmu_directory}/SystemStructure.ssd
    COMMAND cd ${fmu_directory}
    COMMAND zip -r ${output_file_path} *
    COMMAND cd ..
    OUTPUT ${output_file_path}
    COMMENT "Creating SSP ${output_file_path}"
  )

  set(OM_OMS_TESTSUITE_EXTRACTED_SSP_FILES ${OM_OMS_TESTSUITE_EXTRACTED_SSP_FILES} ${output_file_path})
endforeach()

# A custom target the depends on the extracted files.
add_custom_target(testsuite-resources
            DEPENDS ${OM_OMS_TESTSUITE_EXTRACTED_FMU_FILES}
            DEPENDS ${OM_OMS_TESTSUITE_EXTRACTED_SSP_FILES}
            COMMENT "Extracted FMUs and SSPs used by the testsuite."
)
