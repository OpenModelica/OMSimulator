RM=rm -rf
MKDIR=mkdir -p

.PHONY: all doc-doxygen clean

all: OMSimulatorLib.png doc-doxygen

doc-doxygen:
	@echo
	@echo "# make doc-doxygen"
	@echo
	@doxygen Doxyfile

OMSimulatorLib.png: OMSimulatorLib.tex
	@echo
	@echo "# make OMSimulatorLib.png"
	@echo
	pdflatex OMSimulatorLib.tex
	pdftoppm OMSimulatorLib.pdf -png -r 100 -aaVector yes -aa yes > OMSimulatorLib.png

clean:
	@echo
	@echo "# make clean"
	@echo
	$(RM) OMSimulatorLib.aux OMSimulatorLib-blx.bib OMSimulatorLib.log OMSimulatorLib.run.xml OMSimulatorLib.pdf
	$(RM) html
