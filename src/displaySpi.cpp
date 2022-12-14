#include <stdint.h>
#include <stdlib.h>

#include <avr/pgmspace.h>
#include <util/delay.h>

#include "iopin.h"
#include "spimaster.h"
#include "avrdbg.h"

#include "displaySpi.h"

enum Commands : uint8_t
{
	cmdNOP = 0x00,
	cmdSoftReset = 0x01,
	cmdRDDID = 0x04,
	cmdRDDST = 0x09,

	cmdSleepIn = 0x10,
	cmdSleepOut = 0x11,
	ST77XX_PTLON = 0x12,
	ST77XX_NORON = 0x13,

	cmdInvertOff = 0x20,
	cmdInvertOn = 0x21,
	cmdGammaSet = 0x26,
	cmdDisplayOff = 0x28,
	cmdDisplayOn = 0x29,
	cmdColumnAddressSet = 0x2A,
	cmdRowAddressSet = 0x2B,
	ST77XX_RAMWR = 0x2C,
	ST77XX_RAMRD = 0x2E,

	ST77XX_PTLAR = 0x30,
	ST77XX_TEOFF = 0x34,
	ST77XX_TEON = 0x35,
	ST77XX_MADCTL = 0x36,
	ST77XX_VSCRSADD = 0x37,
	ST77XX_COLMOD = 0x3A,

	ST77XX_MADCTL_MY = 0x80,
	ST77XX_MADCTL_MX = 0x40,
	ST77XX_MADCTL_MV = 0x20,
	ST77XX_MADCTL_ML = 0x10,
	ST77XX_MADCTL_RGB = 0x00,

	ST77XX_RDID1 = 0xDA,
	ST77XX_RDID2 = 0xDB,
	ST77XX_RDID3 = 0xDC,
	ST77XX_RDID4 = 0xDD,

	ST7735_MADCTL_BGR = 0x08,
	ST7735_MADCTL_MH = 0x04,

	cmdFrameControl1 = 0xB1,
	cmdFrameControl2 = 0xB2,
	cmdFrameControl3 = 0xB3,
	cmdInvertControl = 0xB4,
	ST7735_DISSET5 = 0xB6,
	ILI9341_DFUNCTR = 0xB6,

	cmdPowerControl1 = 0xC0,
	cmdPowerControl2 = 0xC1,
	cmdPowerControl3 = 0xC2,
	cmdPowerControl4 = 0xC3,
	cmdPowerControl5 = 0xC4,
	ST7735_VMCTR1 = 0xC5,
	ST7735_VMCTR2 = 0xC7,

	ST7735_PWCTR6 = 0xFC,

	ST7735_GMCTRP1 = 0xE0,
	ST7735_GMCTRN1 = 0xE1,

	ST_CMD_DELAY = 0x80,		// special signifier for command lists
};

#define ILI9341_RDMODE 0x0A     // Read Display Power Mode
#define ILI9341_RDMADCTL 0x0B   // Read Display MADCTL
#define ILI9341_RDPIXFMT 0x0C   // Read Display Pixel Format
#define ILI9341_RDIMGFMT 0x0D   // Read Display Image Format
#define ILI9341_RDSELFDIAG 0x0F // Read Display Self-Diagnostic Result

#define ILI9341_INVOFF 0x20   // Display Inversion OFF
#define ILI9341_INVON 0x21    // Display Inversion ON
#define ILI9341_GAMMASET 0x26 // Gamma Set
#define ILI9341_DISPOFF 0x28  // Display OFF
#define ILI9341_DISPON 0x29   // Display ON

#define ILI9341_CASET 0x2A // Column Address Set
#define ILI9341_PASET 0x2B // Page Address Set
#define ILI9341_RAMWR 0x2C // Memory Write
#define ILI9341_RAMRD 0x2E // Memory Read

#define ILI9341_PTLAR 0x30    // Partial Area
#define ILI9341_VSCRDEF 0x33  // Vertical Scrolling Definition
#define ILI9341_MADCTL 0x36   // Memory Access Control
#define ILI9341_VSCRSADD 0x37 // Vertical Scrolling Start Address
#define ILI9341_PIXFMT 0x3A   // COLMOD: Pixel Format Set

#define ILI9341_FRMCTR1 0xB1 // Frame Rate Control (In Normal Mode/Full Colors)
#define ILI9341_FRMCTR2 0xB2 // Frame Rate Control (In Idle Mode/8 colors)
#define ILI9341_FRMCTR3 0xB3 // Frame Rate control (In Partial Mode/Full Colors)
#define ILI9341_INVCTR 0xB4  // Display Inversion Control
#define ILI9341_DFUNCTR 0xB6 // Display Function Control

#define ILI9341_PWCTR1 0xC0 // Power Control 1
#define ILI9341_PWCTR2 0xC1 // Power Control 2
#define ILI9341_PWCTR3 0xC2 // Power Control 3
#define ILI9341_PWCTR4 0xC3 // Power Control 4
#define ILI9341_PWCTR5 0xC4 // Power Control 5
#define ILI9341_VMCTR1 0xC5 // VCOM Control 1
#define ILI9341_VMCTR2 0xC7 // VCOM Control 2

#define ILI9341_RDID1 0xDA // Read ID 1
#define ILI9341_RDID2 0xDB // Read ID 2
#define ILI9341_RDID3 0xDC // Read ID 3
#define ILI9341_RDID4 0xDD // Read ID 4

#define ILI9341_GMCTRP1 0xE0 // Positive Gamma Correction
#define ILI9341_GMCTRN1 0xE1 // Negative Gamma Correction

static const uint8_t init9341[] PROGMEM =
{
	0xEF, 3,
		0x03, 0x80, 0x02,
	0xCF, 3,
		0x00, 0xC1, 0x30,
	0xED, 4,
		0x64, 0x03, 0x12, 0x81,
	0xE8, 3,
		0x85, 0x00, 0x78,
	0xCB, 5,
		0x39, 0x2C, 0x00, 0x34, 0x02,
	0xF7, 1,
		0x20,
	0xEA, 2,
		0x00, 0x00,
	ILI9341_PWCTR1, 1,
		0x23,             // Power control VRH[5:0]
	ILI9341_PWCTR2, 1,
		0x10,             // Power control SAP[2:0];BT[3:0]
	ILI9341_VMCTR1, 2,
		0x3e, 0x28,       // VCM control
	ILI9341_VMCTR2, 1,
		0x86,             // VCM control2
	ILI9341_MADCTL, 1,
		0x48,             // Memory Access Control
	ILI9341_VSCRSADD, 1,
		0x00,             // Vertical scroll zero
	ILI9341_PIXFMT, 1,
		0x55,
	ILI9341_FRMCTR1, 2,
		0x00, 0x18,
	ILI9341_DFUNCTR, 3,
		0x08, 0x82, 0x27, // Display Function Control
	0xF2, 1,
		0x00,                         // 3Gamma Function Disable
	ILI9341_GAMMASET, 1,
		0x01,             // Gamma curve selected
	ILI9341_GMCTRP1, 15,
		0x0F, 0x31, 0x2B, 0x0C, 0x0E,	// Set Gamma
		0x08, 0x4E, 0xF1, 0x37, 0x07,
		0x10, 0x03, 0x0E, 0x09, 0x00,
	ILI9341_GMCTRN1, 15,
		0x00, 0x0E, 0x14, 0x03, 0x11,	// Set Gamma
		0x07, 0x31, 0xC1, 0x48, 0x08,
		0x0F, 0x0C, 0x31, 0x36, 0x0F,
	cmdSleepOut, ST_CMD_DELAY,                // Exit Sleep
		150,
	ILI9341_DISPON, ST_CMD_DELAY,                // Display on
		150,

	0x00                                   // End of list
};

// 7735R init
static const uint8_t init7735[] PROGMEM =
{										
	cmdSoftReset, ST_CMD_DELAY,			// Software reset, 0 args, w/delay
		150,							// 150 ms delay
	cmdSleepOut, ST_CMD_DELAY,			// Out of sleep mode, 0 args, w/delay
		255,							// 500 ms delay
	cmdFrameControl1, 3,				// Framerate ctrl - normal mode, 3 arg:
		0x01, 0x2C, 0x2D,				// Rate = fosc/(1x2+40) * (LINE+2C+2D)
	cmdFrameControl2, 3,				// Framerate ctrl - idle mode, 3 args:
		0x01, 0x2C, 0x2D,				// Rate = fosc/(1x2+40) * (LINE+2C+2D)
	cmdFrameControl3, 6,				// Framerate - partial mode, 6 args:
		0x01, 0x2C, 0x2D,				// Dot inversion mode
		0x01, 0x2C, 0x2D,				// Line inversion mode
	cmdInvertControl, 1,				// Display inversion ctrl, 1 arg:
		0x07,							// No inversion
	cmdPowerControl1, 3,				// Power control, 3 args, no delay:
		0xA2,
		0x02,							// -4.6V
		0x84,							// AUTO mode
	cmdPowerControl2, 1,				// Power control, 1 arg, no delay:
		0xC5,							// VGH25=2.4C VGSEL=-10 VGH=3 * AVDD
	cmdPowerControl3, 2,				// Power control, 2 args, no delay:
		0x0A,							// Opamp current small
		0x00,							// Boost frequency
	cmdPowerControl4, 2,				// Power control, 2 args, no delay:
		0x8A,							// BCLK/2,
		0x2A,							// opamp current small & medium low
	cmdPowerControl5, 2,				// Power control, 2 args, no delay:
		0x8A, 0xEE,
	ST7735_VMCTR1, 1,					// Power control, 1 arg, no delay:
		0x0E,
	cmdInvertOff, 0,					// Don't invert display, no args
	ST77XX_MADCTL, 1,					// Rotation:
		0x00,							// 0x00: portrait, pins up
										// 0xA0: landscape, pins right
										// 0xC0: portrait, pins down
										// 0x60: landscape, pins left

	ST77XX_COLMOD, 1,					// set color mode, 1 arg, no delay:
		0x05,							// 16-bit color

	ST77XX_NORON, ST_CMD_DELAY,			// Normal display on, no args, w/delay
		10,								// 10 ms delay
	cmdDisplayOn, ST_CMD_DELAY,		// Main screen turn on, no args w/delay
		100,							// 100 ms delay

	0x00,								// EOF
};

void Display::init()
{
	// setup the SPI pins
	mosi::dir_out();
	sck::dir_out();

	rst::low();
	rst::dir_out();

	ss::high();
	ss::dir_out();

	dc::high();
	dc::dir_out();

	_delay_ms(100);
	rst::high();
	_delay_ms(200);

	spi::init();

	const uint8_t* addr = init9341;
	uint8_t cmd;

	while ((cmd = pgm_read_byte(addr++)) != 0)
	{
		// number of args to follow
		uint8_t numArgs = pgm_read_byte(addr++);

		// if the hibit is set, delay time follows args
		uint16_t ms = static_cast<uint8_t>(numArgs & ST_CMD_DELAY);

		// mask out delay bit
		numArgs &= ~ST_CMD_DELAY;
		send_init_command(cmd, addr, numArgs);
		addr += numArgs;

		if (ms)
		{
			// Read post-command delay time (ms)
			ms = pgm_read_byte(addr++);
			if (ms == 255)
				ms = 500;
			while (ms--)
				_delay_ms(1);
		}
	}
}

void Display::on()
{
	Transaction t;
	send_command(cmdDisplayOn);
}

void Display::off()
{
	Transaction t;
	send_command(cmdDisplayOff);
}

void Display::send_init_command(const uint8_t commandByte, const uint8_t* dataBytes, const uint8_t numDataBytes)
{
	Transaction t;

	send_command(commandByte);

	for (int i = 0; i < numDataBytes; i++)
		spi::send(pgm_read_byte(dataBytes++));
}

void Display::set_addr_window(const Coord x, const Coord y, const Coord w, const Coord h)
{
	send_command(cmdColumnAddressSet); // column addr set
	spi::send16(x);
	spi::send16(x + w - 1);

	send_command(cmdRowAddressSet); // row addr set
	spi::send16(y);
	spi::send16(y + h - 1);

	send_command(ST77XX_RAMWR); // write to RAM
}
