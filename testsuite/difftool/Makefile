ifdef OMDEV
	CC = gcc
	EXT=".exe"
endif

# ugly fix for macosx
detected_OS := $(shell uname -s)
OMBUILDDIR =../../install/

$(OMBUILDDIR)/bin/omc-diff$(EXT): lex.yy.o $(OMBUILDDIR)/bin
	$(CC) -o $@ lex.yy.o
lex.yy.c: omc-diff.l
	flex omc-diff.l
$(OMBUILDDIR)/bin:
	mkdir -p "$@"
