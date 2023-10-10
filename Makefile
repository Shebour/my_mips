CC=gcc
CFLAGS= -std=c99 -Wall -Wextra -pedantic
CFLAGS_DEBUG= -g
TARGET= my_mips
BUILD_DIR= build/bin
DEBUG_DIR= build/debug
C_SOURCES= src/main.c \
					 src/functions.c \
					 src/utils.c \
					 src/syscalls.c \
					 src/cpu.c \
					 src/logger.c \
					 src/dbg.c

C_INCLUDES= -Iinclude
LDLIBS= --coverage
OBJECTS=$(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
OBJECTS_DEBUG=$(addprefix $(DEBUG_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))


.PHONY: clean test release debug coverage

all: release debug test

release: $(BUILD_DIR)/$(TARGET)

debug: $(DEBUG_DIR)/$(TARGET)

coverage: CFLAGS += --coverage
coverage: LDLIBS += --coverage
coverage: release

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	$(CC) -c $(CFLAGS) $(C_INCLUDES) $< -o $@

$(BUILD_DIR)/$(TARGET): $(OBJECTS) Makefile
	$(CC) $(LDLIBS) $(OBJECTS) -o $@

$(DEBUG_DIR)/%.o: %.c Makefile | $(DEBUG_DIR)
	$(CC) -c $(CFLAGS) $(CFLAGS_DEBUG) $(C_INCLUDES) $< -o $@

$(DEBUG_DIR)/$(TARGET): $(OBJECTS_DEBUG) Makefile
	$(CC) $(LDLIBS) $(OBJECTS_DEBUG) -o $@


$(BUILD_DIR):
	mkdir -p $@

$(DEBUG_DIR):
	mkdir -p $@

test:
	$(MAKE) -C test
	$(MAKE) -C test/c

clean:
	rm -rf build *.gcov *.html *.css
	$(MAKE) -C test clean
	$(MAKE) -C test/c clean distclean

