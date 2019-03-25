#
# Makefile template for ATMega1284p
#

TARGET      = argon

GLYPHLIB    = ../glyphlib
VPATH       = $(GLYPHLIB)

LIB        = GLYPH.o tilemap-engine.o
BUILD      = argon.o main.o

OBJECTS    =  $(LIB) $(BUILD)

DEVICE     = atmega1284p
DEVDUDE    = m1284p
PROGRAMMER = -c linuxspi -P /dev/spidev0.0

C_FLAGS    = -Wl,--gc-sections -Wl,--relax -ffunction-sections -fdata-sections -fno-inline-small-functions -fpack-struct -fshort-enums

# fuse settings:
# use http://www.engbedded.com/fusecalc
FUSES      = -U lfuse:w:0xfe:m -U hfuse:w:0xd9:m -U efuse:w:0xff:m  # External Xtal

AVRDUDE = sudo avrdude -b 500000 $(PROGRAMMER) -p $(DEVDUDE)
COMPILE = avr-gcc -Wall -O3 -mmcu=$(DEVICE) $(C_FLAGS) -I$(GLYPHLIB)

# symbolic targets:
all:	$(TARGET).hex

.c.o:
	$(COMPILE) -c $< -o $@

flash:	all
	$(AVRDUDE) -U flash:w:$(TARGET).hex:i

fuse:
	$(AVRDUDE) $(FUSES)

clean:
	rm -f $(TARGET).hex $(TARGET).elf $(OBJECTS)

size: $(TARGET).elf
	avr-size --format=avr --mcu=$(DEVICE) $(TARGET).elf
	avr-nm -S -td --size-sort $(TARGET).elf

# file targets:
$(TARGET).elf: $(OBJECTS)
	$(COMPILE) -o $(TARGET).elf $(OBJECTS)
    
$(TARGET).hex: $(TARGET).elf
	rm -f $(TARGET).hex
	avr-objcopy -j .text -j .data -O ihex $(TARGET).elf $(TARGET).hex