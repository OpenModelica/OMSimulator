TEST = ../rtest -v

TESTFILES = \
addExternalResources1.lua \
addExternalResources2.lua \
addExternalResources3.py \
addExternalResources4.py \
addExternalResources5.lua \
buses.lua \
buses.py \
connections.lua \
connections.py \
deleteReferencesInSSD1.lua \
deleteReferencesInSSD2.lua \
deleteResourcesInSSP1.lua \
deleteResourcesInSSP2.lua \
fmidertest.lua \
getDirectionalDerivative_CS.lua \
getDirectionalDerivative_ME.lua \
getDirectionalDerivative_CS.py \
getDirectionalDerivative_ME.py \
importExportAllResources.lua \
import_export_units1.lua \
import_export_units2.lua \
setUnits1.lua \
setUnits2.lua \
setUnits3.py \
test_omsExport.lua \
test_omsExport.py \
test01.lua \
test01.py \
test02.lua \
test02.py \
test03.lua \
test03.py \

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
