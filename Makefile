# Target file name.
TARGET = pewpewpew

# List C source files here.
CCSOURCES = Game.c config.c WinScreen.c

# Additional local static libs to link against
LIBS = gaming system stm32f4xx fatfs sdcard graphics

# Folder for object files
OBJDIR = obj

# Folder for sourcecode
SRCDIR = src

# Additional defines
DEFINES := -DRGB8Pixels

TARGET_ROOT_DIR := $(SELF_DIR)

-include $(ROOT)/build/firmware.mak
