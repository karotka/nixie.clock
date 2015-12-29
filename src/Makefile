PRG            = main
OBJ            = main.o twi.o ds3232rtc.o usart.o
MCU_TARGET     = atxmega16d4
OPTIMIZE       = -O2
ISP            = dragon_pdi
F_CPU          = 16000000

# You should not have to change anything below here.

ADDFLAGS = -Wl,-u,vfprintf -lprintf_flt -lm
CC             = avr-gcc

# Override is only needed by avr-lib build system.

override CFLAGS =  -Wall $(OPTIMIZE) -mmcu=$(MCU_TARGET) $(DEFS) -DF_CPU=$(F_CPU) $(ADDFLAGS)

OBJCOPY        = avr-objcopy
OBJDUMP        = avr-objdump

all: hex

$(PRG).elf: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -rf *.o $(PRG).elf $(PRG).hex

hex:  $(PRG).hex

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

install: load

load: $(PRG).hex
	avrdude -P usb -p x16d4 -c $(ISP) -U flash:w:$<

status:
	avrdude -P usb -p x16d4 -c $(ISP) -v
