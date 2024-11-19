TEST = ../rtest -v

TESTFILES = \
activateVariant1.lua \
activateVariant2.lua \
activateVariant3.lua \
checkUnits.lua \
deleteConnector.lua \
deleteConnector1.lua \
deleteReferencesAndStartValues.lua \
deleteRootSystem.lua \
deleteStartValues.lua \
deleteStartValuesInSSV.lua \
deleteStartValuesInSSV1.lua \
deleteStartValuesInSSV2.lua \
DualMassOscillator.lua \
duplicateActivateVariant1.lua \
duplicateActivateVariant2.py \
duplicateVariant1.lua \
duplicateVariant2.lua \
duplicateVariant3.lua \
duplicateVariant4.py \
Enumeration.lua \
Enumeration2.lua \
equationPair_fixedpoint.py \
equationPair.py \
EventTest.lua \
exportConnectorsToResultFile.lua \
exportSnapshotSSP.lua \
exportSSMTemplate.lua \
exportSSMTemplate.py \
fmuState01.lua \
fmuState02.py \
import_export_parameters_inline.lua \
import_export_parameters_to_ssv.lua \
import_export_parameters1.lua \
import_export_parameters2.lua \
import_export_parameters3.lua \
import_export_parameters4.lua \
import_export_signalFilter.lua \
import_export_signalFilter2.lua \
import_export_snapshot.lua \
import_hierarchical_ssv_sources.lua \
import_hierarchical_ssv_sources1.lua \
import_parameter_mapping_from_ssm.lua \
import_parameter_mapping_inline.lua \
importExportEnumerationDefinition.lua \
importPartialSnapshot.lua \
importStartValues.lua \
importStartValues1.lua \
importSuppressUnitConversion.lua \
listVariants1.lua \
multipleConnections.lua \
multipleInstance.lua \
nls.py \
partialSnapshot.lua \
PI_Controller.lua \
QuarterCarModel.DisplacementDisplacement.lua \
referenceResources1.lua \
referenceResources2.lua \
rename.lua \
renameModel.lua \
renameNewCref_1.lua \
renameNewCref_2.lua \
renameNewCref.lua \
renameSnapshot.lua \
renameValues1.lua \
renameValues2.lua \
replaceSubmodel1.lua \
replaceSubmodel10.lua \
replaceSubmodel11.py \
replaceSubmodel12.py \
replaceSubmodel13.py \
replaceSubmodel14.py \
replaceSubmodel2.lua \
replaceSubmodel3.lua \
replaceSubmodel4.lua \
replaceSubmodel5.lua \
replaceSubmodel6.py \
replaceSubmodel7.py \
replaceSubmodel8.lua \
replaceSubmodel9.lua \
setExternalInputs.lua \
simulation.lua \
simulation.py \
snapshot.lua \
snapshot.py \
str_hello_world.lua \
str_hello_world.py \
suppressUnitConversion.lua \
table.lua \
tankYPipe.lua \

# Run make failingtest
FAILINGTESTFILES = \

# Dependency files that are not .lua or Makefile
# Add them here or they will be cleaned.
DEPENDENCIES = \
*.lua \
*.py \
Makefile \

CLEAN = `ls | grep -w -v -f deps.tmp`

.PHONY : test clean getdeps failingtest

test:
	@echo
	@echo Running tests...
	@$(TEST) $(TESTFILES)

baseline:
	@echo
	@echo Updating badelines...
	@$(TEST) -b $(TESTFILES)

# Cleans all files that are not listed as dependencies
clean:
	@echo $(DEPENDENCIES) | sed 's/ /\\\|/g' > deps.tmp
	@rm -rf $(CLEAN)

# Run this if you want to list out the files (dependencies).
# do it after cleaning and updating the folder
# then you can get a list of file names (which must be dependencies
# since you got them from repository + your own new files)
# then add them to the DEPENDENCIES. You can find the
# list in deps.txt
getdeps:
	@echo $(DEPENDENCIES) | sed 's/ /\\\|/g' > deps.tmp
	@echo $(CLEAN) | sed -r 's/deps.txt|deps.tmp//g' | sed 's/ / \\\n/g' > deps.txt
	@echo Dependency list saved in deps.txt.
	@echo Copy the list from deps.txt and add it to the Makefile @DEPENDENCIES

failingtest:
	@echo
	@echo Running failing tests...
	@echo
	@$(TEST) $(FAILINGTESTFILES)
