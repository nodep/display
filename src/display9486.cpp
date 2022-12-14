#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "hw.h"

#include "avrdbg.h"

#include "display9486.h"

#define DELAY7 \
	asm volatile(\
	"rjmp .+0 \n" \
	"rjmp .+0 \n" \
	"rjmp .+0 \n" \
	"nop	\n" \
	::);

enum ILICommands : uint8_t
{
	SOFTRESET			= 0x01,
	SLEEPIN				= 0x10,
	SLEEPOUT			= 0x11,
	NORMALDISP			= 0x13,
	INVERTOFF			= 0x20,
	INVERTON			= 0x21,
	GAMMASET			= 0x26,
	DISPLAYOFF			= 0x28,
	DISPLAYON			= 0x29,
	COLADDRSET			= 0x2A,
	PAGEADDRSET			= 0x2B,
	MEMORYWRITE			= 0x2C,
	MEMCONTROL			= 0x36,
	MADCTL				= 0x36,
	PIXELFORMAT			= 0x3A,
	RGBSIGNAL			= 0xB0,
	FRAMECONTROL		= 0xB1,
	INVERSIONCONRTOL	= 0xB4,
	DISPLAYFUNC			= 0xB6,
	ENTRYMODE			= 0xB7,
	POWERCONTROL1		= 0xC0,
	POWERCONTROL2		= 0xC1,
	VCOMCONTROL1		= 0xC5,
	VCOMCONTROL2		= 0xC7,
	POWERCONTROLB		= 0xCF,
	POWERCONTROLA		= 0xCB,
	PGAMCTRL			= 0xE0,
	NGAMCTRL			= 0xE1,
	DRIVERTIMINGA		= 0xE8,
	DRIVERTIMINGB		= 0xEA,
	POWERONSEQ			= 0xED,
	ENABLE3G			= 0xF2,
	INTERFACECONTROL	= 0xF6,
	RUMPRATIO			= 0xF7,

	MADCTL_MY			= 0x80,
	MADCTL_MX			= 0x40,
	MADCTL_MV			= 0x20,
	MADCTL_ML			= 0x10,
	MADCTL_RGB			= 0x00,
	MADCTL_BGR			= 0x08,
	MADCTL_MH			= 0x04,
};

const uint8_t INIT_DELAY = 0x80;

static const uint8_t ILI9486_regValues[] PROGMEM =
{
	0xF1, 6,				// I don't know what these 4 commands are,
		0x36, 0x04, 0x00,	// but my display does not work without them
		0x3C, 0x0F, 0x8F,	// so I leave them in
	0xF2, 9,
		0x18, 0xA3, 0x12,
		0x02, 0xB2, 0x12,
		0xFF, 0x10, 0x00,
	0xF8, 2,
		0x21, 0x04,
	0xF9, 2,
		0x00, 0x08,

	INVERSIONCONRTOL, 1,
		0x00,
	POWERCONTROL2, 1,
		0x41,
	VCOMCONTROL1, 4,
		0x00, 0x91, 0x80, 0x00,
	PGAMCTRL, 15,
		0x0F, 0x1F, 0x1C, 0x0C, 0x0F,
		0x08, 0x48, 0x98, 0x37, 0x0A,
		0x13, 0x04, 0x11, 0x0D, 0x00,
	NGAMCTRL, 15,
		0x0F, 0x32, 0x2E, 0x0B, 0x0D,
		0x05, 0x47, 0x75, 0x37, 0x06,
		0x10 ,0x03, 0x24, 0x20, 0x00,
	PIXELFORMAT, 1,
		0x55,
	SLEEPOUT, 0,

	MADCTL, 1 | INIT_DELAY,			// Rotation:
		MADCTL_BGR | MADCTL_MV,		// MV: landscape, USB left
									// MX: portrait, USB up
									// MY: portrait, USB down
									// MV | MY | MX: landscape, USB right
		120,
	DISPLAYON, 0,

	0	// EOF
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

	const uint8_t* p = ILI9486_regValues;
	uint8_t cmd;
	while ((cmd = pgm_read_byte(p++)) != 0)
	{
		// number of args to follow
		uint8_t num_args = pgm_read_byte(p++);

		// if the hibit is set, delay time follows args
		const bool has_delay = (num_args & INIT_DELAY) != 0;

		// mask out delay bit
		num_args &= ~INIT_DELAY;

		// send the command
		writeCmd16(cmd);
		for (uint8_t i = 0; i < num_args; i++)
			write8(pgm_read_byte(p++));

		if (has_delay)
		{
			// Read post-command delay time (ms)
			uint8_t ms = pgm_read_byte(p++);
			while (ms--)
				_delay_ms(1);
		}
	}
}

const uint8_t EMASK = 0x38;
const uint8_t GMASK = 0x20;
const uint8_t HMASK = 0x78;

void Display9486::setWriteDir()
{
	DDRH |= HMASK;
	DDRE |= EMASK;
	DDRG |= GMASK;
}

void Display9486::setReadDir()
{
	DDRH &= ~HMASK;
	DDRE &= ~EMASK;
	DDRG &= ~GMASK;
}

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
	writeCmd8(COLADDRSET);
	write16(x);
	write16(x + w - 1);

	writeCmd8(PAGEADDRSET);
	write16(y);
	write16(y + h - 1);

	writeCmd8(MEMORYWRITE);
}
