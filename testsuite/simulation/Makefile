TEST = ../rtest -v

TESTFILES = \
exportJson1.py \
exportJson2.py \
exportJson4.py \
tankYPipe.py \
SimpleSimulation1.py \
SimpleSimulation2.py \
SimpleSimulation3.py \
SimpleSimulation4.py \
SimpleSimulation5.py \
SimpleSimulation6.py \
SimpleSimulation7.py \
SimpleSimulation8.py \
SimpleSimulation9.py \
SimpleSimulation10.py \
SimpleSimulation11.py \
SimpleSimulation12.py \
SimpleSimulation13.py \
SimpleSimulation14.py \

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
