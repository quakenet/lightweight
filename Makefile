##
# $Id: Makefile,v 1.12 2004/09/21 21:16:14 froo Exp $
##

TARGET = lightweight

CC = gcc
LD = gcc

CFLAGS = -g -O2 -Wall
INCLUDE = -I.

OS := $(shell uname -s | tr A-Z a-z)
ifeq "$(OS)" "linux"
INCLUDE += -I/usr/include/pcre
LIBS = -lz -lpcre
else
ifeq "$(OS)" "freebsd"
INCLUDE += -I/usr/local/include
LIBS = -L/usr/local/lib -lz -lpcre
else
$(error Unknown OS "$(OS)", you need to manually edit the Makefile)
endif
endif

#############################################################################
# OBJECT FILES
#############################################################################

SRC = $(wildcard *.c) $(wildcard servercommands/*.c) $(wildcard clientcommands/*.c)
OBJ = $(SRC:.c=.o)

#############################################################################
# BUILD
#############################################################################

$(TARGET): $(OBJ)
	$(LD) -o $@ $(OBJ) $(LIBS)

#############################################################################
# MISC
#############################################################################

.PHONY: clean indent

.c.o:
	$(CC) $(CFLAGS) $(INCLUDE) -o $*.o -c $*.c

clean:
	@rm -f $(OBJ)
	@rm -f $(TARGET)

indent:
	@find . -name "*.[ch]" -exec indent -i2 -ts2 -l120 -bad -bap -br -brs -cd1 -cbi0 -cdw -ce -cp1 -cs -nhnl -npcs -nprs -npsl -nut -sai -saf -saw -sob \{\} \;

love:
	@echo "Quit fooling around and get back to coding!"

tags: TAGS

TAGS:
	@ctags `find . -name "*.[ch]" -a -type f`
	@cd servercommands && ctags `find .. -name "*.[ch]" -a -type f`
	@cd clientcommands && ctags `find .. -name "*.[ch]" -a -type f`
