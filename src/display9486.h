#pragma once

#include "graph.h"

// 8 bit parallel display. Shield on Arduino Mega 2560
// Control pins on port F, data pins all over the place: ports 

using rstPin = IoPin<'F', 4>;
using csPin  = IoPin<'F', 3>;
using cdPin  = IoPin<'F', 2>;
using wrPin  = IoPin<'F', 1>;
using rdPin  = IoPin<'F', 0>;

class Display9486
{
public:
    static const Coord Width = 480;
    static const Coord Height = 320;

    struct Transaction
    {
        Transaction()   { csPin::low(); }
        ~Transaction()  { csPin::high(); }
    };

    static void init();

    static void pixel(Coord x, Coord y, Color col)
    {
        set_addr_window(x, y, 1, 1);
        write16(color2rgb(col));
    }

   	static void color(Color col)
	{
		write16(color2rgb(col));
	}

    static void colors(Color col, uint16_t len)
    {
        for (uint16_t c = 0; c < len; c++)
            color(col);
    }

	template <typename Win>
	static void blit(Win& w, Coord x, Coord y) = delete;

	template <Coord WinWidth, Coord WinHeight>
	static void blit(Window<WinWidth, WinHeight>& win, Coord x, Coord y)
	{
		Transaction t;

		set_addr_window(x, y, WinWidth, WinHeight);
		for (Coord wy = 0; wy < WinHeight; wy++)
			for (Coord wx = 0; wx < WinWidth; wx++)
				color(win.get_color(wx, wy));
	}

	template <Coord WinWidth, Coord WinHeight>
	static void blit(WindowRGB<WinWidth, WinHeight>& win, Coord x, Coord y)
	{
		Transaction t;

		set_addr_window(x, y, WinWidth, WinHeight);
		for (ColorRGB col : win.buffer)
			write16(col);
	}

private:

    friend void fill_rect<Display9486>(Display9486&, Coord x, Coord y, Coord w, Coord h, Color color);

    static void wrStrobe()
    {
        wrPin::low();
        wrPin::high();
    }

    static void write8(const uint8_t d)
    {
        PORTE = ((d & (0x03<<2)) << 2) | ((d & (0x01<<5)) >> 2);
        PORTH = ((d & (0x03<<6)) >> 3) | ((d & 0x03) << 5);
        PORTG = (d & (0x01<<4)) << 1;

        wrStrobe();
    }

    static void write16(const uint16_t d)
    {
        write8(static_cast<uint8_t>(d >> 8));
        write8(static_cast<uint8_t>(d));
    }

    static void writeCmd8(const uint8_t x)
    {
        cdPin::low();
        write8(x);
        cdPin::high();
    }

    static void writeCmd16(const uint16_t x)
    {
        cdPin::low();
        write16(x);
        cdPin::high();
    }

    static void set_addr_window(Coord x1, Coord y1, Coord x2, Coord y2);

    static uint8_t read8();
    static uint16_t read16();

    static uint16_t read_reg(uint16_t reg, int8_t index);
    static uint16_t read_ID();

    static void setWriteDir();
    static void setReadDir();
};

template <>
inline void fill_rect<Display9486>(Display9486&, Coord x, Coord y, Coord w, Coord h, Color color)
{
	typename Display9486::Transaction t;

	Display9486::set_addr_window(x, y, w, h);
    for (Coord r = 0; r < h; r++)
        Display9486::colors(color, w);
}
