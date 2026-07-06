MSPM33_SDK_INSTALL_DIR ?= $(abspath ../../../../../..)
include $(MSPM33_SDK_INSTALL_DIR)/imports.mak

CC = $(TICLANG_ARMCOMPILER)/bin/tiarmclang
AR = $(TICLANG_ARMCOMPILER)/bin/tiarmar

# FreeRTOS Source directory
FREERTOS_SRC_DIR = $(MSPM33_SDK_INSTALL_DIR)/kernel/freertos/Source
FREERTOS_PORT_DIR = $(FREERTOS_SRC_DIR)/portable/TI_ARM_CLANG/ARM_CM33_NTZ/non_secure

# Source files
FREERTOS_SOURCES = \
    croutine.c \
    event_groups.c \
    list.c \
    queue.c \
    stream_buffer.c \
    tasks.c \
    timers.c

FREERTOS_PORT_SOURCES = port.c portasm.c

HEAP_SOURCE = heap_4.c

# Object files
FREERTOS_OBJECTS = $(FREERTOS_SOURCES:.c=.obj)
PORT_OBJECTS = $(FREERTOS_PORT_SOURCES:.c=.obj)
HEAP_OBJECTS = $(HEAP_SOURCE:.c=.obj)

ALL_OBJECTS = $(FREERTOS_OBJECTS) $(PORT_OBJECTS) $(HEAP_OBJECTS)

CFLAGS = -D__MSPM33C321A__ \
    -Og \
    -I.. \
    -I$(FREERTOS_SRC_DIR)/include \
    -I$(FREERTOS_PORT_DIR) \
    "-I$(MSPM33_SDK_INSTALL_DIR)/source/third_party/CMSIS/Core/Include" \
    "-I$(MSPM33_SDK_INSTALL_DIR)/source" \
    -gdwarf-3 \
    -mcpu=cortex-m33 \
    -mfloat-abi=hard \
    -mfpu=fpv5-sp-d16 \
    -Wall

OUTPUT = freertos.lib

all: $(OUTPUT)

$(OUTPUT): $(ALL_OBJECTS)
	@echo Creating library $@
	@$(AR) -r $@ $^

# FreeRTOS core source files
$(FREERTOS_OBJECTS): %.obj: $(FREERTOS_SRC_DIR)/%.c
	@echo Compiling $<
	@$(CC) $(CFLAGS) -c $< -o $@

# Port specific files
port.obj: $(FREERTOS_PORT_DIR)/port.c
	@echo Compiling $<
	@$(CC) $(CFLAGS) -c $< -o $@

portasm.obj: $(FREERTOS_PORT_DIR)/portasm.c
	@echo Compiling $<
	@$(CC) $(CFLAGS) -c $< -o $@

# Heap memory file
heap_4.obj: $(FREERTOS_SRC_DIR)/portable/MemMang/heap_4.c
	@echo Compiling $<
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -f $(ALL_OBJECTS) $(OUTPUT)

.PHONY: all clean
