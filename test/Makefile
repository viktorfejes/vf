CC = clang
BUILD_DIR = build
BIN_DIR = bin
OBJ_DIR = obj

BINARY = tests
EXTENSION = .exe
CFLAGS = -g -MD -Wall
IFLAGS = -Isrc -I../
LFLAGS = 
DEFINES = -D_PLATFORM_WINDOWS -D_CRT_SECURE_NO_WARNINGS

DIR = $(subst /,\,${CURDIR})

# Wildcard function
rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

SRC_FILES = $(call rwildcard,src/,*.c)
DIRECTORIES = \src $(subst $(DIR),,$(shell dir src /S /AD /B | findstr /i src))
OBJ_FILES = $(SRC_FILES:%=$(BUILD_DIR)/$(OBJ_DIR)/%.o)

$(info DIR: $(DIR))
$(info SRC_FILES: $(SRC_FILES))
$(info DIRECTORIES: $(DIRECTORIES))
$(info OBJ_FILES: $(OBJ_FILES))
$(info SCAFFOLD_DIRS: $(addprefix $(BUILD_DIR)\$(OBJ_DIR), $(DIRECTORIES)))

all: scaffold compile link

scaffold:
	@echo Creating folder structure...
	-@setlocal enableextensions enabledelayedexpansion && mkdir $(addprefix $(BUILD_DIR)\$(OBJ_DIR), $(DIRECTORIES)) 2>NUL || cd .
	-@setlocal enableextensions enabledelayedexpansion && mkdir $(BUILD_DIR)\$(BIN_DIR) 2>NUL || cd .
	@echo Scaffolding DONE.

link: scaffold $(OBJ_FILES)
	@echo Linking $(BINARY)...
	@$(CC) $(OBJ_FILES) -o $(BUILD_DIR)\$(BIN_DIR)\$(BINARY)$(EXTENSION) $(LFLAGS)

compile:
	@echo Compiling...

$(BUILD_DIR)/$(OBJ_DIR)/%.c.o: %.c
	@echo $<...
	@$(CC) $< $(CFLAGS) -c -o $@ $(DEFINES) $(IFLAGS)

clean:
	@if exist $(BUILD_DIR)\$(BIN_DIR)\$(BINARY)$(EXTENSION) del $(BUILD_DIR)\$(BIN_DIR)\$(BINARY)$(EXTENSION)
	@rmdir /s /q $(BUILD_DIR)\$(OBJ_DIR)
	@rmdir /s /q $(BUILD_DIR)\$(BIN_DIR)