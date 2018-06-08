# sources
SOURCES	= src/f6-split-tool.c
OBJECTS	= $(SOURCES:.c=.o)
TARGET	= f6-split-tool

# compiler
CC	= gcc
CFLAGS	= -O2 -Wall
LDFLAGS	= 

# Make rules
all: $(OBJECTS)
	$(CC) $(LDFLAGS) -o $(TARGET) $(OBJECTS)

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f *.[oa] *~ $(TARGET) $(OBJECTS)

