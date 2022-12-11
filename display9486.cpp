#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "hw.h"

#include "avrdbg.h"

#include "display9486.h"
#include "lcd_registers.h"

#define DELAY7 \
  asm volatile(	  \
	"rjmp .+0 \n" \
	"rjmp .+0 \n" \
	"rjmp .+0 \n" \
	"nop      \n" \
	::);

const uint8_t EMASK = 0x38;
const uint8_t GMASK = 0x20;
const uint8_t HMASK = 0x78;

static inline void setWriteDir()
{
	DDRH |= HMASK;
	DDRE |= EMASK;
	DDRG |= GMASK;
}

static inline void setReadDir()
{
	DDRH &= ~HMASK;
	DDRE &= ~EMASK;
	DDRG &= ~GMASK;
}

const uint8_t TFTLCD_DELAY8 = 0x7F;

uint16_t Display9486::read_reg(uint16_t reg, int8_t index)
{
	uint16_t ret;
	csPin::low();
	writeCmd16(reg);
	setReadDir();
	_delay_ms(1); 
	do { 
		ret = read16();
	} while (--index >= 0);   
	csPin::high();
	setWriteDir();

	return ret;
}

uint16_t Display9486::read_ID()
{
	const uint16_t ret = read_reg(0xD3, 1);
	if (ret == 0x9486)
		dprint("lcd OK\n");
	else
		dprint("lcd NOT OK\n");

	return ret;
}

static const uint8_t ILI9486_regValues[] PROGMEM = 
{
	0xF1, 6, 0x36, 0x04, 0x00, 0x3C, 0x0F, 0x8F,
	0xF2, 9, 0x18, 0xA3, 0x12, 0x02, 0xB2, 0x12, 0xFF, 0x10, 0x00, 
	0xF8, 2, 0x21, 0x04,
	0xF9, 2, 0x00, 0x08,
	0x36, 1, 0x08, 
	0xB4, 1, 0x00,
	0xC1, 1, 0x41,
	0xC5, 4, 0x00, 0x91, 0x80, 0x00,
	0xE0, 15, 0x0F, 0x1F, 0x1C, 0x0C, 0x0F, 0x08, 0x48, 0x98, 0x37, 0x0A, 0x13, 0x04, 0x11, 0x0D, 0x00,
	0xE1, 15, 0x0F, 0x32, 0x2E, 0x0B, 0x0D, 0x05, 0x47, 0x75, 0x37, 0x06, 0x10 ,0x03, 0x24, 0x20, 0x00,				
	0x3A, 1, 0x55,
	0x11, 0,
	0x36, 1, 0x28,
	TFTLCD_DELAY8, 120,
	0x29, 0
};

void Display9486::init()
{
    csPin::high();
    rdPin::high();
    wrPin::high();
    rstPin::high();

	rstPin::dir_out();
	csPin::dir_out();
	cdPin::dir_out();
	wrPin::dir_out();
	rdPin::dir_out();

	setWriteDir();

    rstPin::low();
    _delay_ms(2);
    rstPin::high();

    Transaction t;

	const uint8_t MAX_REG_NUM = 24;
    uint8_t dat[MAX_REG_NUM];
	const uint8_t* p = ILI9486_regValues;
    int16_t size = sizeof(ILI9486_regValues);
	while (size > 0) 
	{
		uint8_t cmd = pgm_read_byte(p++);
		uint8_t len = pgm_read_byte(p++);
		if (cmd == TFTLCD_DELAY8) 
		{
			while (len--)
				_delay_ms(1);
			len = 0;
		} 
		else 
		{
			for (uint8_t i = 0; i < len; i++)
				dat[i] = pgm_read_byte(p++);
			send_command(cmd,dat,len);
		}
		size -= len + 2;
	}
}

uint8_t Display9486::read8()
{
	rdPin::low();

	DELAY7;

	uint8_t dst = (PINH & (0x03<<5)) >> 5;
	dst |= (PINH & (0x03<<3)) << 3;
	dst |= (PINE & (0x01<<3)) << 2;
	dst |= (PINE & (0x03<<4)) >> 2;
	dst |= (PING & (0x01<<5)) >> 1;
	
	rdPin::high();

	return dst;
}

uint16_t Display9486::read16()
{
	uint16_t result = read8() << 8;
	result |= read8();
	return result;
}

void Display9486::set_addr_window(Coord x, Coord y, Coord w, Coord h)
{
	writeCmd8(ILI9341_COLADDRSET);
	write16(x);
	write16(x + w - 1);

	writeCmd8(ILI9341_PAGEADDRSET);
	write16(y);
	write16(y + h - 1);

	writeCmd8(ILI9341_MEMORYWRITE);	
}

void Display9486::pixel(Coord x, Coord y, Color col)
{
    set_addr_window(x, y, 1, 1);

    write16(color2rgb(col));
}
