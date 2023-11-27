CC=gcc
MKDIR=mkdir
RM=rm -f
BIN:=bin

CFLAGS:=-g -O0 -Wall -Wextra -pedantic -std=c99 -fsanitize=address -fsanitize=undefined

ifdef release
CFLAGS:=-O2 -Wall -Wextra -pedantic -std=c99 -DNDEBUG
endif

ifndef verbose
SILENT=@
endif

SOURCES:=$(wildcard */main.c)

$(BIN)/day%: $(SOURCES) | $(BIN) 
	$(SILENT) $(CC) $(CFLAGS) -o $@ $(subst $(BIN)/,,$@)/main.c -laocaux -lm

$(BIN):
	$(SILENT) $(MKDIR) -p $(BIN)

clean:
	$(SILENT) $(RM) $(BIN)/*

.PHONY: clean 
