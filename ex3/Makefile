TARGET = scheduling

CC = clang
CFLAGS = -std=gnu11
LDFLAGS = -lpthread

BUILD = build

SRCS = $(wildcard *.c)
OBJS = $(addprefix $(BUILD)/,$(SRCS:.c=.o))

all: $(BUILD)/$(TARGET)

$(BUILD)/$(TARGET): $(OBJS) | $(BUILD)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

$(BUILD)/%.o: %.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD):
	mkdir -p $(BUILD)

clean:
	rm -rf $(BUILD)