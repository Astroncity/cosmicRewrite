# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Werror -ggdb -L lib/ -I include/ -lraylib -lm -lflecs
DEPFLAGS = -MMD -MP

# Directories
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin

# Output executable names
OUTPUT_NAME = cosmic-ascent
ENGINE_OUTPUT_NAME = game-engine-interface

# Source files and object files for the main game
GAME_SRC_FILES = $(shell find $(SRC_DIR)/game -name '*.c')
GAME_OBJ_FILES = $(patsubst $(SRC_DIR)/game/%, $(BUILD_DIR)/game/%, $(GAME_SRC_FILES:.c=.o))
GAME_DEP_FILES = $(GAME_OBJ_FILES:.o=.d)

# Source files and object files for the engine interface
ENGINE_SRC_FILES = $(shell find $(SRC_DIR)/interface -name '*.c')
ENGINE_OBJ_FILES = $(patsubst $(SRC_DIR)/interface/%, $(BUILD_DIR)/interface/%, $(ENGINE_SRC_FILES:.c=.o))
ENGINE_DEP_FILES = $(ENGINE_OBJ_FILES:.o=.d)

# Colors for output
RED = \033[0;31m
GREEN = \033[0;32m
BLUE = \033[0;34m
YELLOW = \033[0;33m
RESET = \033[0m
ACTION = $(YELLOW) [ACTION] $(RESET)
INFO = $(BLUE) [INFO] $(RESET)

# Default target builds both executables
all: directories $(BIN_DIR)/$(OUTPUT_NAME) $(BIN_DIR)/$(ENGINE_OUTPUT_NAME)
	@printf "$(INFO) Compilation complete. Executables created in $(BIN_DIR).\n"

# Create directories if they don't exist
directories:
	@mkdir -p $(BUILD_DIR)/game
	@mkdir -p $(BUILD_DIR)/interface
	@mkdir -p $(BIN_DIR)

# Build the main game executable
$(BIN_DIR)/$(OUTPUT_NAME): $(GAME_OBJ_FILES)
	@printf "$(ACTION) Linking object files for $(OUTPUT_NAME)...\n"
	@$(CC) $^ -o $@ $(CFLAGS)

# Build the game engine interface executable
$(BIN_DIR)/$(ENGINE_OUTPUT_NAME): $(ENGINE_OBJ_FILES)
	@printf "$(ACTION) Linking object files for $(ENGINE_OUTPUT_NAME)...\n"
	@$(CC) $^ -o $@ $(CFLAGS)

# Compile each game source file to an object file
$(BUILD_DIR)/game/%.o: $(SRC_DIR)/game/%.c
	@mkdir -p $(dir $@)
	@printf "$(ACTION) Compiling $< to $@...\n"
	@$(CC) -c $< -o $@ $(CFLAGS) $(DEPFLAGS)

# Compile each engine interface source file to an object file
$(BUILD_DIR)/interface/%.o: $(SRC_DIR)/interface/%.c
	@mkdir -p $(dir $@)
	@printf "$(ACTION) Compiling $< to $@...\n"
	@$(CC) -c $< -o $@ $(CFLAGS) $(DEPFLAGS)

# Include dependency files
-include $(GAME_DEP_FILES)
-include $(ENGINE_DEP_FILES)

# Clean the build and bin directories
clean:
	@printf "$(ACTION) Cleaning build and bin directories...\n"
	@rm -rf $(BUILD_DIR)/*
	@rm -rf $(BIN_DIR)/*

# Phony targets
.PHONY: all directories clean
