MAKE	= /usr/bin/make
F_CPU   = 8000000

# ATTINY2313
# FUSE_H = 0x9b -> EESAVE is programmed (0), EEPROM content is preserved during chip erase
#FUSE_L=0xEF
#FUSE_H=0x9b
FUSE_L=0xE4
FUSE_H=0x9F
CFLAGS= -g -Os -Wall -mcall-prologues -mmcu=attiny2313
CPU=t2313

OBJ2HEX	=	/usr/bin/avr-objcopy 
PROG	=	/usr/bin/avrdude 
TARGET	=	main
VPATH	=	main
SS	=	main
OBJS	=	main.o 
CC	= 	avr-gcc -Wall -Os -DF_CPU=$(F_CPU) $(CFLAGS) -mmcu=attiny2313
AVRDUDE	=	$(PROG) -c usbasp -P usb -p $(CPU) -B8

.PHONY:	$(SS)

all			:	$(TARGET).hex
				
$(SS)			:
				$(MAKE) -C $@
clean 			:
				rm -f *.hex *.obj *.o *.eep
				rm -f */*.o

.S.o			:
				$(CC) -x assembler-with-cpp -c $< -o $@

$(TARGET).hex		:	$(TARGET).obj
				$(OBJ2HEX) -R .eeprom -O ihex $< $@
				avr-objcopy -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 -O ihex $(TARGET).obj  $(TARGET).eep

$(TARGET).obj		:	$(OBJS)
				$(CC) $(CFLAGS) -o $@ $(OBJS)

program 		: 	$(TARGET).hex
				$(AVRDUDE)
				$(AVRDUDE) -U flash:w:$(TARGET).hex:i 
				$(AVRDUDE) -U flash:v:$(TARGET).hex:i 

eeprom			:	$(TARGET).obj
				$(AVRDUDE) -U eeprom:w:$(TARGET).eep:i 
				$(AVRDUDE) -U eeprom:v:$(TARGET).eep:i 

fuse:
	@[ "$(FUSE_H)" != "" -a "$(FUSE_L)" != "" ] || \
		{ echo "*** Edit Makefile and choose values for FUSE_L and FUSE_H!"; exit 1; }
	$(AVRDUDE) -U hfuse:w:$(FUSE_H):m -U lfuse:w:$(FUSE_L):m


