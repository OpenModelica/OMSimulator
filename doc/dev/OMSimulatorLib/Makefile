RM=rm -rf
MKDIR=mkdir -p

.PHONY: all doc-doxygen clean

all: doc-doxygen

doc-doxygen:
	@echo
	@echo "# make doc-doxygen"
	@echo
	@doxygen Doxyfile

clean:
	@echo
	@echo "# make clean"
	@echo
	$(RM) html
