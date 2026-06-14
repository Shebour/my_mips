CC=gcc
CFLAGS= -std=c99 -Wall -Wextra -pedantic
TARGET= my_mips
BUILD_DIR= build/bin
COV_DIR= build/coverage
DEBUG_DIR= build/debug
ASAN_DIR= build/asan
C_SOURCES= src/main.c \
					 src/functions.c \
					 src/utils.c \
					 src/syscalls.c \
					 src/cpu.c \
					 src/fpu.c \
					 src/jit.c \
					 src/device.c \
					 src/disasm.c \
					 src/logger.c \
					 src/dbg.c \
					 src/cstream.c \
					 src/cstream_file.c \
					 src/cstream_readline.c \
					 src/error.c \
					 src/interruptible_readline.c \
					 src/vec.c \
					 src/map_elf.c

C_INCLUDES= -Iinclude
LDLIBS= -lreadline -lm

# Optional SDL2 backend for the game system's framebuffer. When SDL2 is not
# installed the device falls back to a no-op (the emulator still builds).
SDL_LIBS := $(shell pkg-config --libs sdl2 2>/dev/null)
ifneq ($(SDL_LIBS),)
CFLAGS += -DHAVE_SDL $(shell pkg-config --cflags sdl2 2>/dev/null)
LDLIBS += $(SDL_LIBS)
endif
OBJECTS=$(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
OBJECTS_COV=$(addprefix $(COV_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
OBJECTS_DEBUG=$(addprefix $(DEBUG_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
OBJECTS_ASAN=$(addprefix $(ASAN_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))


.PHONY: clean test release debug coverage asan

all: clean release debug test asan coverage

release: $(BUILD_DIR)/$(TARGET)

debug: CFLAGS += -g
debug: $(DEBUG_DIR)/$(TARGET)

asan: CFLAGS += -g -fsanitize=address
asan: LDLIBS = -lasan -lreadline -lm $(SDL_LIBS)
asan: $(ASAN_DIR)/$(TARGET)

coverage: CFLAGS += --coverage
coverage: LDLIBS += --coverage
coverage: $(COV_DIR)/$(TARGET)

check: release test
	./test_suite.py

check-coverage: coverage test
	./test_suite.py
	gcov -o build/coverage src/*.c
	gcovr --html-details -o coverage/index.html
	firefox coverage/index.html

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(C_INCLUDES) -c $< -o $@

$(BUILD_DIR)/$(TARGET): $(OBJECTS) Makefile
	$(CC) $(OBJECTS) -o $@ $(LDLIBS)

$(COV_DIR)/%.o: %.c Makefile | $(COV_DIR)
	$(CC) $(CFLAGS) $(C_INCLUDES) -c $< -o $@

$(COV_DIR)/$(TARGET): $(OBJECTS_COV) Makefile
	$(CC) $(OBJECTS_COV) -o $@ $(LDLIBS)

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

$(COV_DIR):
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

