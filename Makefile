# Target file name.
TARGET = st

# List C source files here.
CCSOURCES = Game.c

# Additional local static libs to link against
LIBS = gaming stm32f4xx fatfs sdcard graphics 

# Folder for object files
OBJDIR = obj

# Folder for sourcecode
SRCDIR = src

# Additional defines
DEFINES := -DRGB8Pixels

-include $(ROOT)/build/firmware.mak
