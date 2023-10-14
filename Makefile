CC=gcc
CFLAGS= -std=c99 -Wall -Wextra -pedantic
TARGET= my_mips
BUILD_DIR= build/bin
DEBUG_DIR= build/debug
ASAN_DIR= build/asan
C_SOURCES= src/main.c \
					 src/functions.c \
					 src/utils.c \
					 src/syscalls.c \
					 src/cpu.c \
					 src/logger.c \
					 src/dbg.c \
					 src/cstream.c \
					 src/cstream_file.c \
					 src/cstream_readline.c \
					 src/cstream_string.c \
					 src/error.c \
					 src/interruptible_readline.c \
					 src/vec.c \
					 src/readelf.c

C_INCLUDES= -Iinclude
LDLIBS= -lreadline
OBJECTS=$(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
OBJECTS_DEBUG=$(addprefix $(DEBUG_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
OBJECTS_ASAN=$(addprefix $(ASAN_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))


.PHONY: clean test release debug coverage asan

all: release debug test asan

release: $(BUILD_DIR)/$(TARGET)

debug: CFLAGS += -g
debug: $(DEBUG_DIR)/$(TARGET)

asan: CFLAGS += -g -fsanitize=address
asan: LDLIBS = -lasan -lreadline
asan: $(ASAN_DIR)/$(TARGET)

coverage: CFLAGS += --coverage
coverage: LDLIBS += --coverage
coverage: clean release

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(C_INCLUDES) -c $< -o $@

$(BUILD_DIR)/$(TARGET): $(OBJECTS) Makefile
	$(CC) $(OBJECTS) -o $@ $(LDLIBS)

$(DEBUG_DIR)/%.o: %.c Makefile | $(DEBUG_DIR)
	$(CC) $(CFLAGS) $(C_INCLUDES) -c $< -o $@

$(DEBUG_DIR)/$(TARGET): $(OBJECTS_DEBUG) Makefile
	$(CC) $(OBJECTS_DEBUG) -o $@ $(LDLIBS)

$(ASAN_DIR)/%.o: %.c Makefile | $(ASAN_DIR)
	$(CC) $(CFLAGS) $(C_INCLUDES) -c $< -o $@

$(ASAN_DIR)/$(TARGET): $(OBJECTS_ASAN) Makefile
	$(CC) $(OBJECTS_ASAN) -o $@ $(LDLIBS)


$(BUILD_DIR):
	@mkdir -p $@

$(DEBUG_DIR):
	@mkdir -p $@

$(ASAN_DIR):
	@mkdir -p $@

test:
	@$(MAKE) -C test
	@$(MAKE) -C test/c

clean:
	@rm -rf build *.gcov *.html *.css
	@$(MAKE) -C test clean
	@$(MAKE) -C test/c clean distclean

