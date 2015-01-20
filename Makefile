PROJECT   = mbed_arch_pro
OBJECTS   = ./main.o ./i2c.o ./font8x8.o
TOOLCHAIN = GCC_CS
SDK       = $(HOME)/src/mbed/build
SYS_OBJECTS = $(SDK)/mbed/TARGET_ARCH_PRO/TOOLCHAIN_$(TOOLCHAIN)/*.o

INCLUDE_PATHS = -I$(SDK)/mbed -I$(SDK)/mbed/TARGET_ARCH_PRO -I$(SDK)/mbed/TARGET_ARCH_PRO/TARGET_NXP -I$(SDK)/mbed/TARGET_ARCH_PRO/TARGET_NXP/TARGET_LPC176X -I$(SDK)/mbed/TARGET_ARCH_PRO/TARGET_NXP/TARGET_LPC176X/TARGET_ARCH_PRO
LIBRARY_PATHS = -L$(SDK)/mbed/TARGET_ARCH_PRO/TOOLCHAIN_$(TOOLCHAIN)
LIBRARIES = -lmbed
LINKER_SCRIPT = $(SDK)/mbed/TARGET_ARCH_PRO/TOOLCHAIN_$(TOOLCHAIN)/LPC1768.ld

CC = arm-none-eabi-gcc
CPP = arm-none-eabi-g++
OBJCOPY = arm-none-eabi-objcopy

CC_FLAGS = -MMD -c -g -fno-common -fmessage-length=0 -Wall -fno-exceptions -fno-rtti -mcpu=cortex-m3 -mthumb -ffunction-sections -fdata-sections -fomit-frame-pointer
ONLY_C_FLAGS = -std=gnu99
ONLY_CPP_FLAGS = -std=gnu++11

AS = arm-none-eabi-as
LD = arm-none-eabi-gcc
LD_FLAGS = -mcpu=cortex-m3 -mthumb -Wl,--gc-sections
LD_SYS_LIBS = -lstdc++ -lsupc++ -lm -lc -lgcc

ifeq ($(DEBUG), 1)
  CC_FLAGS += -DDEBUG -O0
else
  CC_FLAGS += -DNDEBUG -Os
endif

all: $(PROJECT).bin

clean:
	rm -f $(PROJECT).bin $(PROJECT).elf $(OBJECTS)

.s.o:
	$(AS)  $(CC_FLAGS) $(CC_SYMBOLS) -o $@ $<

.c.o:
	$(CC)  $(CC_FLAGS) $(CC_SYMBOLS) $(ONLY_C_FLAGS)   $(INCLUDE_PATHS) -o $@ $<

.cpp.o:
	$(CPP) $(CC_FLAGS) $(CC_SYMBOLS) $(ONLY_CPP_FLAGS) $(INCLUDE_PATHS) -o $@ $<


$(PROJECT).elf: $(OBJECTS) $(SYS_OBJECTS)
	$(LD) $(LD_FLAGS) -T$(LINKER_SCRIPT) $(LIBRARY_PATHS) -o $@ $^ $(LIBRARIES) $(LD_SYS_LIBS) $(LIBRARIES) $(LD_SYS_LIBS)

$(PROJECT).bin: $(PROJECT).elf
	$(OBJCOPY) -O binary $< $@

-include *.d
