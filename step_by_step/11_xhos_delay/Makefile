
# toolchain
TOOLCHAIN    = arm-none-eabi-
CC           = $(TOOLCHAIN)gcc
CP           = $(TOOLCHAIN)objcopy
AS           = $(TOOLCHAIN)gcc #-x assembler-with-cpp
HEX          = $(CP) -O ihex
BIN          = $(CP) -O binary -S

# define mcu, specify the target processor
MCU          = cortex-m3

# all the files will be generated with this name (main.elf, main.bin, main.hex, etc)
PROJECT_NAME=xhos

# specify define
DDEFS       =

# define root dir
ROOT_DIR     = .

# define include dir
INCLUDE_DIRS = .


# define os dir
OS_DIR = $(ROOT_DIR)/xhos
3RDPART_DIR = $(ROOT_DIR)/3rdpart

# define user dir
APP_DIR     = $(ROOT_DIR)/app

# link file
LINK_SCRIPT  = $(OS_DIR)/port/xhos_stm32.ld

# user specific
#ASM_SRC   = $(OS_DIR)/start.s
#SRC       = $(OS_DIR)/xhos.c
#SRC      += $(OS_DIR)/printf.c
#SRC      += $(OS_DIR)/xhos_mem.c
#SRC      += $(USER_DIR)/main.c

# user include
#INCLUDE_DIRS  = $(APP_DIR)
#INCLUDE_DIRS  += $(OS_DIR)

# include sub makefiles
include $(3RDPART_DIR)/makefile.mk   # STM32 Standard Peripheral Library
include $(APP_DIR)/makefile.mk   # STM32 Standard Peripheral Library
include $(OS_DIR)/makefile.mk   # STM32 Standard Peripheral Library

INC_DIR  = $(patsubst %, -I%, $(INCLUDE_DIRS))

# run from Flash
DEFS	 = $(DDEFS) #-DRUN_FROM_FLASH=1

OBJECTS  = $(ASM_SRC:.s=.o) $(SRC:.c=.o)


MC_FLAGS = -mcpu=$(MCU)

#AS_FLAGS = $(MC_FLAGS) -g -gdwarf-2 -mthumb  -Wa,-amhls=$(<:.s=.lst)
#CP_FLAGS = $(MC_FLAGS) $(OPT) -g -gdwarf-2 -mthumb -fomit-frame-pointer -Wall -fverbose-asm -Wa,-ahlms=$(<:.c=.lst) $(DEFS)
#LD_FLAGS = $(MC_FLAGS) -g -gdwarf-2 -mthumb -nostartfiles -Xlinker --gc-sections -T$(LINK_SCRIPT) -Wl,-Map=$(PROJECT_NAME).map,--cref,--no-warn-mismatch
AS_FLAGS = $(MC_FLAGS)  -mthumb 
CP_FLAGS = $(MC_FLAGS)  -mthumb -Wall  $(DEFS)
LD_FLAGS = $(MC_FLAGS)  -mthumb -nostartfiles -T$(LINK_SCRIPT) 
#----------------------- C 编译选项-------------------------------

# -g*: 产生调试信息

# -O*: 代码优代级别

# -f...: 用于改变内建一些默认的函数,具体的看avr-libc手册

# -Wall..: 警告的级别

# -Wa,...: 告诉GCC 后面的选项不用管，把它传给汇编选项.

# -Wl,...: 和-Wa选项差不多，告诉GCC 传递后面的选项给liker链接器.

# -a[cdhlmns=file]: 创建汇编列表文件,默认的一般选 -adhlns

# 子选项如下:

# c 省略错误的条件

# d 省略调试的指令

# h 包含最高层的源文件

# l 包含汇编文件

# m 包含宏的展开

# n 省略处理的框架（forms processing）（我不知是什么意思）

# s 包含标号

# =file 设置生成的汇编列表文件的文件名,可以包含目录名用"/"隔开. 

#
# makefile rules
#
all: $(OBJECTS) $(PROJECT_NAME).elf  $(PROJECT_NAME).hex $(PROJECT_NAME).bin
	$(TOOLCHAIN)size $(PROJECT_NAME).elf

%.o: %.c
	$(CC) -c $(CP_FLAGS) -I . $(INC_DIR) $< -o $@

%.o: %.s
	$(AS) -c $(AS_FLAGS) $< -o $@

%.elf: $(OBJECTS)
	$(CC) $(OBJECTS) $(LD_FLAGS) -o $@

%.hex: %.elf
	$(HEX) $< $@

%.bin: %.elf
	$(BIN)  $< $@

flash: $(PROJECT_NAME).bin
	sudo stm32flash -w $(PROJECT_NAME).bin -v -g 0 /dev/ttyUSB0

clean:
	-rm -rf $(OBJECTS)
	-rm -rf $(PROJECT_NAME).elf
	-rm -rf $(PROJECT_NAME).map
	-rm -rf $(PROJECT_NAME).hex
	-rm -rf $(PROJECT_NAME).bin

