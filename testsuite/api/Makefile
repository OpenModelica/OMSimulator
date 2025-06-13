TEST = ../rtest -v

TESTFILES = \
create_ssp_file.py \
cref.py \
deleteResources1.py \
duplicateVariant1.py \
duplicateVariant2.py \
exportJson1.py \
exportJson2.py \
exportJson4.py \
exportSSVTemplate1.py \
exportSSVTemplate2.py \
exportSSMTemplate1.py \
exportSSMTemplate2.py \
fmuinfo.py \
invalidSSD.py \
invalidSSV.py \
LOC.py \
listSSVReference1.py \
NestedSystem.py \
NewSSP.py \
NewSSP2.py \
NewSSP3.py \
setValue1.py \
setValue2.py \
setValue3.py \
setValue4.py \
setSolver1.py \
setSolver2.py \
setValueSSV1.py \
setValueSSV2.py \
setValueSSV3.py \
setMappingSSM1.py \
setMappingSSM2.py \
setMappingSSM3.py \
swapSSV1.py \
swapSSV2.py \
swapSSV3.py \
swapSSV4.py \

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
	@echo Updating baselines...
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
