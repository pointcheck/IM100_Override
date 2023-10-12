#FUSE BITS: 9.6MHz
LFUSE = 0x7a
HFUSE = 0xff

LOCK = 0xff
PROG = avrispmkII 

CROSS=$(CROSS_COMPILE)
CC=$(CROSS)gcc
LD=$(CROSS)gcc
AR=$(CROSS)ar
CP=$(CROSS)objcopy
AS=$(CROSS)as
STRIP=$(CROSS)strip
OBJDUMP=$(CROSS)objdump

BUILD_NUMBER=`cat build_number`

#MCFLAGS =  -Os -std=c99 -Wl,-u,vfprintf   
MCFLAGS =  -Os -std=c99    
INCLUDES = -I./

STATIC_LIBS =

CFLAGS	= $(MCFLAGS) $(INCLUDES) -mmcu=attiny13 -DBUILD_NUMBER=$(BUILD_NUMBER)+1 -gdwarf-2 -DF_CPU=9600000UL -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -Wall -Wstrict-prototypes -fno-move-loop-invariants  
###-DDEBUG=1

#LDFLAGS = -mmcu=atmega16  -Wl,-u,vfprintf -lprintf_flt 
#LDFLAGS = -mmcu=attiny13  -Wl,-u,vfprintf  
LDFLAGS = -mmcu=attiny13  


all: main.hex

main.hex: main.elf
	$(CP) -j .text -j .data -O ihex $^ $@

main.o: main.c 
	$(CC) -c $(CFLAGS) $^ $(LDFLAGS) $(STATIC_LIBS) -o $@

main.elf: main.o  
	@expr $(BUILD_NUMBER) + 1 > build_number 
	$(CC) $^ $(LDFLAGS) $(STATIC_LIBS) -o $@
	$(STRIP) $@

disass:	main.elf
	$(OBJDUMP) --disassemble-all $^

install:
	avrdude -p t13 -c $(PROG) -P usb -e -U flash:w:main.hex 


fuse:
	avrdude -p t13 -c $(PROG) -P usb -U lfuse:w:$(LFUSE):m -U hfuse:w:$(HFUSE):m -U lock:w:$(LOCK):m 


clean:
	rm main.hex main.o

