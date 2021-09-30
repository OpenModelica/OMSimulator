TEST = ../rtest -v

TESTFILES = \
deleteConnector.lua \
deleteConnector1.lua \
deleteStartValues.lua \
deleteStartValuesInSSV.lua \
deleteStartValuesInSSV1.lua \
deleteStartValuesInSSV2.lua \
deleteReferencesAndStartValues.lua \
DualMassOscillator.lua \
Enumeration.lua \
Enumeration2.lua \
exportConnectorsToResultFile.lua \
exportSSMTemplate.lua \
exportSSMTemplate.py \
exportSnapshotSSP.lua \
import_export_parameters_inline.lua \
import_export_parameters_to_ssv.lua \
import_export_parameters1.lua \
import_export_parameters2.lua \
import_export_parameters3.lua \
import_export_parameters4.lua \
import_export_signalFilter.lua \
import_export_snapshot.lua \
import_export.lua \
import_export.py \
import_hierarchical_ssv_sources.lua \
import_hierarchical_ssv_sources1.lua \
import_parameter_mapping_from_ssm.lua \
import_parameter_mapping_inline.lua \
importPartialSnapshot.lua \
importStartValues.lua \
importStartValues1.lua \
multipleConnections.lua \
partialSnapshot.lua \
PI_Controller.lua \
QuarterCarModel.DisplacementDisplacement.lua \
referenceResources1.lua \
rename.lua \
renameValues1.lua \
renameValues2.lua \
renameModel.lua \
renameSnapshot.lua \
renameNewCref.lua \
renameNewCref_1.lua \
renameNewCref_2.lua \
setExternalInputs.lua \
simulation.lua \
simulation.py \
snapshot.lua \
snapshot.py \
table.lua \

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
