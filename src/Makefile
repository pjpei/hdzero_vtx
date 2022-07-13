SDCC = sdcc
MAIN_SOURCE = mcu  
SOURCES = isr camera dm6300 global hardware i2c_device i2c lifetime monitor msp_displayport print rom sfr_ext smartaudio_protocol spi uart

CC=sdcc
FAMILY=mcs51
PROC=DS80C323

CFLAGS=-m$(FAMILY) --model-large --opt-code-speed --stack-auto

build: $(MAIN_SOURCE:=.ihx)

$(MAIN_SOURCE:=.ihx): $(MAIN_SOURCE:=.rel) $(SOURCES:=.rel)
	$(SDCC) $(CFLAGS) --xram-size 65536 --iram-size 256 --code-size 65536 --out-fmt-ihx $(MAIN_SOURCE:=.rel) $(SOURCES:=.rel)
	objcopy -Iihex -Obinary mcu.ihx HDZERO_TX.bin

clean:
	rm -f *.ihx *.lk *.lst *.map *.rel *.rst *.sym *.asm

%.rel: %.c
	$(SDCC) -c $(CFLAGS) $<

.PHONY: all clean
