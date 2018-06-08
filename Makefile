# f6-split-tool Makefile -- Version 0.0.2
# Simple Makefile to build the f6-split tool
#
# Roland Hostettler <r.hostettler@gmx.ch>
# 2008-01-31
# 2008-05-07

# sources
SOURCES	= src/f6-split-tool.c
OBJECTS	= $(SOURCES:.c=.o)
TARGET	= f6-split-tool
# TARGET	= f6-split-tool.exe

# compiler
CC	= gcc
# CC	= i586-mingw32msvc-gcc
CFLAGS	= -O2 -Wall
LDFLAGS	= 

# Make rules
all: $(OBJECTS)
	$(CC) $(LDFLAGS) -o $(TARGET) $(OBJECTS)

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f *.[oa] *~ $(TARGET) $(OBJECTS)

