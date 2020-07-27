PRG            = ChargerII
TARGET = $(PRG)

COMMONCODE  = .\CommonCode
#AVRLIB = C:\avrlib

#$(COMMONCODE)/uart.c

#$(COMMONCODE)/timers.c $(COMMONCODE)/lcd.c
#my_lcd.c
#$(COMMONCODE)/timers.c
#BatteryPack_Comms.c
OBJ            =  $(COMMONCODE)/systemtimers.c $(COMMONCODE)/eeprom.c $(COMMONCODE)/lcdrprintf.c Menu.c kbd.c $(COMMONCODE)/lcd.c $(COMMONCODE)/i2c.c  $(COMMONCODE)/rprintf.c $(COMMONCODE)/uart.c  $(COMMONCODE)/time.c  $(COMMONCODE)/io.c chargerII.c  BatteryPack_Comms.c



#MCU_TARGET     = atmega8
MCU_TARGET     = atmega168
MCU = $(MCU_TARGET)
OPTIMIZE       = -O2
#OPTIMIZE       = 
DEFS           =
LIBS           =

# You should not have to change anything below here.

CC             = avr-gcc

# Output format. (can be srec, ihex, binary)
FORMAT = ihex


# Override is only needed by avr-lib build system.

override CFLAGS        = -g -Wall $(OPTIMIZE) -mmcu=$(MCU_TARGET) -I$(COMMONCODE)  -I$(AVRLIB) -I./ $(DEFS) 
override LDFLAGS       = -Wl,-Map,$(PRG).map

OBJCOPY        = avr-objcopy
OBJDUMP        = avr-objdump
SIZE 		   = avr-size
MEMO		   = avr-nm


# 0x13
# Fuse high Byte
# 7  - 1(unprogrammed)
# 6  - 1
# 5  - 1
# 4  RSTDISABLE select PB7 is IO or RESET : 1(unprogrammed)
# 3  SPIEN Enable serial port : 0(spi enabled)
# 2  EESAVE ee memory is preserved :1(eeprom not saved)
# 1  BODLEVEL  brownout detect level : 1(unprogrammed)
# 0  BODEN     brownout enable :0= BOD disabled

# 1Mhz Int RC = 0xE1
# 2Mhz Int RC = 0xE2
# 4Mhz Int RC = 0xE3
# 8Mhz Int RC = 0xE4

#LOW FUSE BYTE
#PLLCLK 7 USE PLL for internal clock
#CKOPT  6 Oscilator options
#SUT1   5 Select Startup time :1(unprogrammed)
#SUT0   4 Select Startup time :0(programmed)
#CKSEL3 3 Select CLock source
#CKSEL2 2 Select CLock source
#CKSEL1 1 Select CLock source
#CKSEL0 0 Select CLock source

#
AVRDUDE_PROGRAMMER = stk500

AVRDUDE_PORT = com3
# programmer connected to serial device
#AVRDUDE_PORT = lpt1	# programmer connected to parallel port

AVRDUDE_WRITE_FLASH = -U flash:w:$(TARGET).hex
#AVRDUDE_WRITE_EEPROM = -U eeprom:w:$(TARGET).eep
#AVRDUDE_WRITE_HFUSE = -U hfuse:w:0x13:m
AVRDUDE_WRITE_HFUSE = -U hfuse:w:0x13:m

#AVRDUDE_WRITE_LOCK = -U lock:w:0xFF:m
AVRDUDE_WRITE_LOCK = 


#AVRDUDE_WRITE_LFUSE = -U lfuse:w:0xe1:m
#AVRDUDE_WRITE_LFUSE = -U lfuse:w:0xe9:m
AVRDUDE_WRITE_LFUSE = -U lfuse:w:0xA9:m


AVRDUDE_FLAGS = -p $(MCU) -P $(AVRDUDE_PORT) -b 19200 -c $(AVRDUDE_PROGRAMMER) -y

# Uncomment the following if you want avrdude's erase cycle counter.
# Note that this counter needs to be initialized first using -Yn,
# see avrdude manual.
#AVRDUDE_ERASE += -y

# Uncomment the following if you do /not/ wish a verification to be
# performed after programming the device.
#AVRDUDE_FLAGS += -V

# Increase verbosity level.  Please use this when submitting bug
# reports about avrdude. See <http://savannah.nongnu.org/projects/avrdude> 
# to submit bug reports.
#AVRDUDE_FLAGS += -v -v


# Programming support using avrdude.
AVRDUDE = avrdude






# Define Messages
# English
MSG_ERRORS_NONE = Errors: none
MSG_BEGIN = -------- begin --------
MSG_END = --------  end  --------
MSG_SIZE_BEFORE = Size before: 
MSG_SIZE_AFTER = Size after:
MSG_COFF = Converting to AVR COFF:
MSG_EXTENDED_COFF = Converting to AVR Extended COFF:
MSG_FLASH = Creating load file for Flash:
MSG_EEPROM = Creating load file for EEPROM:
MSG_EXTENDED_LISTING = Creating Extended Listing:
MSG_SYMBOL_TABLE = Creating Symbol Table:
MSG_LINKING = Linking:
MSG_COMPILING = Compiling:
MSG_ASSEMBLING = Assembling:
MSG_CLEANING = Cleaning project:





all: $(PRG).elf lst text eeprom ram

$(PRG).elf: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)
 


# Program the device.  
program:
	$(AVRDUDE) $(AVRDUDE_FLAGS) -F $(AVRDUDE_WRITE_FLASH) $(AVRDUDE_WRITE_EEPROM) $(AVRDUDE_WRITE_HFUSE) $(AVRDUDE_WRITE_LFUSE) $(AVRDUDE_WRITE_LOCK)
	$(AVRDUDE) $(AVRDUDE_FLAGS) -F $(TARGET).hex $(TARGET)_eeprom.hex 

clean:
	rm  *.o $(PRG).elf *.eps *.png *.pdf *.bak 
	rm  *.lst *.map $(EXTRA_CLEAN_FILES)

lst:  $(PRG).lst

ram: $(PRG).ram


%.eep: %.elf
	@echo
	@echo $(MSG_EEPROM) $@
	-$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" \
	--change-section-lma .eeprom=0 -O $(FORMAT) $< $@


%.lst: %.elf
	$(OBJDUMP) -h -S $< > $@
	
	
%.ram: %.elf
	$(MEMO) -n -S $< > $@	

# Rules for building the .text rom images

text: hex bin srec

hex:  $(PRG).hex
bin:  $(PRG).bin
srec: $(PRG).srec

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

%.srec: %.elf
	$(OBJCOPY) -j .text -j .data -O srec $< $@

%.bin: %.elf
	$(OBJCOPY) -j .text -j .data -O binary $< $@

# Rules for building the .eeprom rom images

eeprom: ehex ebin esrec

ehex:  $(PRG)_eeprom.hex
ebin:  $(PRG)_eeprom.bin
esrec: $(PRG)_eeprom.srec

%_eeprom.hex: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O ihex $< $@

%_eeprom.srec: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O srec $< $@

%_eeprom.bin: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O binary $< $@

# Every thing below here is used by avr-libc's build system and can be ignored
# by the casual user.

FIG2DEV                 = fig2dev
EXTRA_CLEAN_FILES       = *.hex *.bin *.srec

dox: eps png pdf

eps: $(PRG).eps
png: $(PRG).png
pdf: $(PRG).pdf

%.eps: %.fig
	$(FIG2DEV) -L eps $< $@

%.pdf: %.fig
	$(FIG2DEV) -L pdf $< $@

%.png: %.fig
	$(FIG2DEV) -L png $< $@



