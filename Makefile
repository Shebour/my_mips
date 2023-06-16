CC=gcc
CFLAGS= -std=c99 -Wall -Werror -Wextra
CFLAGS_DEBUG= -g -fsanitize=address
TARGET= my_mips
BUILD_DIR= build
C_SOURCES= src/main.c \
					 src/registers.c
C_INCLUDES= -Iinclude
LDLIBS= -lasan
OBJECTS=$(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))


all: $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	$(CC) -c $(CFLAGS) $(CFLAGS_DEBUG) $(C_INCLUDES) $< -o $@

$(BUILD_DIR)/$(TARGET): $(OBJECTS) Makefile
	$(CC) $(LDLIBS) $(OBJECTS) -o $@


$(BUILD_DIR):
	mkdir $@

clean:
	rm -rf $(BUILD_DIR)
