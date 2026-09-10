CC = gcc
CFLAGS = -g -Wall -std=gnu11 -I. -fsanitize=address
LDFLAGS = -g -fsanitize=address

BUILD = build

# one executable per top-level .c file
MAINS = $(wildcard *.c)
TARGETS = $(addprefix $(BUILD)/,$(MAINS:.c=))

# shared sources linked into every executable
COMMON_SOURCES = $(wildcard common/*.c)
COMMON_OBJECTS = $(COMMON_SOURCES:.c=.o)

.PHONY: all clean

all: $(TARGETS)

$(BUILD)/%: %.c $(COMMON_OBJECTS) | $(BUILD)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $< $(COMMON_OBJECTS)

$(BUILD):
	mkdir -p $(BUILD)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(COMMON_OBJECTS)
	rm -rf $(BUILD)
