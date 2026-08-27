# program executable name
TARGET = build/taska

# compiler flags
CFLAGS = -g -std=gnu11 -I../common

# linker flags
LDFLAGS = -g

# list of sources
SOURCES = $(shell find -name "*.c")

# default rule, to compile everything
all: $(TARGET)

# define object files
OBJECTS = $(SOURCES:.c=.o)

# link programs
$(TARGET): $(OBJECTS)
	@gcc $(LDFLAGS) -o $@ $^

# compile
%.o : %.c
	@gcc $(CFLAGS) -c -o $@ $<

# cleaning
clean:
	rm -f $(TARGET) $(OBJECTS)