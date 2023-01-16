#pragma once

#include "hw.h"

#include "graph.h"

// The code below is inspired by Adafruit's LCD arduino library
// Thank you, Lady Ada!

class Display
{
public:

	struct Transaction
	{
		Transaction()	{ ss::low(); }
		~Transaction()	{ ss::high(); }
	};

	static const Coord Width = 240;
	static const Coord Height = 320;

	static void init();
	static void off();
	static void on();

	static void pixel(uint8_t x, uint8_t y, Color col)
	{
		set_addr_window(x, y, 1, 1);
		color(col);
	}

	static void pixel(uint8_t x, uint8_t y, ColorRGB col)
	{
		set_addr_window(x, y, 1, 1);
		spi::send16(col);
	}

	static void color(ColorRGB colRGB)
	{
		spi::send16(colRGB);
	}

	static void color(Color col)
	{
		color(color2rgb(col));
	}

	static void colors(Color col, uint16_t len)
	{
		for (uint16_t c = 0; c < len; c++)
			color(col);
	}

	static void hline(uint8_t x, uint8_t y, uint8_t len, Color color)
	{
		set_addr_window(x, y, len, 1);
		colors(color, len);
	}

	static void vline(uint8_t x, uint8_t y, uint8_t len, Color color)
	{
		set_addr_window(x, y, 1, len);
		colors(color, len);
	}

	template <typename Win>
	static void blit(Win& w, Coord x, Coord y) = delete;

	template <Coord WinWidth, Coord WinHeight>
	static void blit(Window<WinWidth, WinHeight>& w, Coord x, Coord y)
	{
		Transaction t;

		set_addr_window(x, y, WinWidth, WinHeight);
		for (Coord y = 0; y < WinHeight; y++)
			for (Coord x = 0; x < WinWidth; x++)
				color(w.get_color(x, y));
	}

	template <Coord WinWidth, Coord WinHeight>
	static void blit(WindowRGB<WinWidth, WinHeight>& w, Coord x, Coord y)
	{
		Transaction t;

		set_addr_window(x, y, WinWidth, WinHeight);
		for (ColorRGB col : w.buffer)
			spi::send16(col);
	}

private:

	friend void fill_rect<Display>(Display& d, Coord x0, Coord y0, Coord w, Coord h, Color color);
	friend void draw_raster<Display>(Display& d, const uint8_t* raster, Coord x, Coord y, Coord w, Coord h, Color color, Color bgcolor);

	static void send_init_command(uint8_t commandByte, const uint8_t* dataBytes, uint8_t numDataBytes);
	static void set_addr_window(Coord x, Coord y, Coord w, Coord h);
	static void send_command(uint8_t cmd)
	{
		dc::low();
		spi::send(cmd);
		dc::high();
	}
};

template <>
inline void fill_rect<Display>(Display&, Coord x, Coord y, Coord w, Coord h, Color color)
{
	typename Display::Transaction t;

	Display::set_addr_window(x, y, w, h);
	for (Coord r = 0; r < h; r++)
		Display::colors(color, w);
}

template <>
inline void draw_raster<Display>(Display&, const uint8_t* raster, Coord x, Coord y, Coord w, Coord h, Color color, Color bgcolor)
{
	typename Display::Transaction t;

	Display::set_addr_window(x, y, w, h);

	Color curr_color = bgcolor;
	while (true)
	{
		const uint8_t num_pixels = pgm_read_byte(raster++);

		if (num_pixels == 0)
			break;

		Display::colors(curr_color, num_pixels);

		if (curr_color == color)
			curr_color = bgcolor;
		else
			curr_color = color;
	}
}


class TS_Point {
public:
	TS_Point() : x(0), y(0), z(0) {}
	TS_Point(int16_t x, int16_t y, int16_t z) : x(x), y(y), z(z) {}
	bool operator==(TS_Point p) { return ((p.x == x) && (p.y == y) && (p.z == z)); }
	bool operator!=(TS_Point p) { return ((p.x != x) || (p.y != y) || (p.z != z)); }
	int16_t x, y, z;
};

class XPT2046_Touchscreen {
public:

	void begin();

	TS_Point getPoint();
	bool tirqTouched()
	{
		return !tirq::in();
	}
	bool touched();

private:
	void update();

	int16_t xraw = 0, yraw = 0, zraw = 0;
	uint16_t msraw = 0x8000;
};