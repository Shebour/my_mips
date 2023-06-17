CC=gcc
CFLAGS= -std=c99 -Wall -Werror -Wextra
CFLAGS_DEBUG= -g -fsanitize=address
TARGET= my_mips
TARGET_DEBUG= my_mips_dbg
BUILD_DIR= build/bin
DEBUG_DIR= build/debug
C_SOURCES= src/main.c \
					 src/registers.c
C_INCLUDES= -Iinclude
LDLIBS= -lasan
OBJECTS=$(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
OBJECTS_DEBUG=$(addprefix $(DEBUG_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))


.PHONY: clean test release debug

all: release debug

release: $(BUILD_DIR)/$(TARGET)

debug: $(DEBUG_DIR)/$(TARGET_DEBUG)

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	$(CC) -c $(CFLAGS) $(C_INCLUDES) $< -o $@

$(BUILD_DIR)/$(TARGET): $(OBJECTS) Makefile
	$(CC) $(OBJECTS) -o $@

$(DEBUG_DIR)/%.o: %.c Makefile | $(DEBUG_DIR)
	$(CC) -c $(CFLAGS) $(CFLAGS_DEBUG) $(C_INCLUDES) $< -o $@

$(DEBUG_DIR)/$(TARGET_DEBUG): $(OBJECTS_DEBUG) Makefile
	$(CC) $(LDLIBS) $(OBJECTS_DEBUG) -o $@


$(BUILD_DIR):
	mkdir -p $@

$(DEBUG_DIR):
	mkdir -p $@

test:
	$(MAKE) -C test
	$(MAKE) -C test/c

clean:
	rm -rf build
	$(MAKE) -C test clean
	$(MAKE) -C test/c clean distclean

