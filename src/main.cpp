#include <stdio.h>
#include <stdlib.h>

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "avrdbg.h"

#include "hw.h"

#include "graph.h"
#include "graphtext.h"

#ifdef __AVR_ATmega2560__
	#include "display9486.h"
	using Display = Display9486;
#else
	#include "displaySpi.h"
#endif

const int DIAL_ARC_POINTS = 85;

struct { uint8_t x, y; } const dial_arc[DIAL_ARC_POINTS] PROGMEM = {
{ 6,34},{ 5,33},{ 4,32},{ 4,31},{ 3,30},{ 3,29},{ 2,28},{ 2,27},{ 2,26},{ 1,25},
{ 1,24},{ 1,23},{ 1,22},{ 1,21},{ 1,20},{ 1,19},{ 1,18},{ 1,17},{ 2,16},{ 2,15},
{ 2,14},{ 3,13},{ 3,12},{ 4,11},{ 4,10},{ 5, 9},{ 6, 8},{ 7, 7},{ 7, 7},{ 8, 6},
{ 9, 5},{10, 4},{11, 4},{12, 3},{13, 3},{14, 2},{15, 2},{16, 2},{17, 1},{18, 1},
{19, 1},{20, 1},{21, 1},{22, 1},{23, 1},{24, 1},{25, 1},{26, 2},{27, 2},{28, 2},
{29, 3},{30, 3},{31, 4},{32, 4},{33, 5},{34, 6},{35, 7},{35, 7},{36, 8},{37, 9},
{38,10},{38,11},{39,12},{39,13},{40,14},{40,15},{40,16},{41,17},{41,18},{41,19},
{41,20},{41,21},{41,22},{41,23},{41,24},{41,25},{40,26},{40,27},{40,28},{39,29},
{39,30},{38,31},{38,32},{37,33},{36,34}
};

template <typename Canvas>
struct ThickBrush
{
	using Transaction = typename Canvas::Transaction;

	Canvas& canvas;

	ThickBrush(Canvas& w)
		: canvas(w)
	{}

	void pixel(Coord x, Coord y, Color col)
	{
		canvas.pixel(x, y, col);
		canvas.pixel(x+1, y, col);
		canvas.pixel(x-1, y, col);
		canvas.pixel(x, y+1, col);
		canvas.pixel(x, y-1, col);
	}
};

template <typename Canvas, typename ColorT>
void draw_dial(Canvas& canvas, Coord x, Coord y, uint8_t position, ColorT col)
{
	static_assert(Canvas::Width >= 43  &&  Canvas::Height >= 36, "Canvas is too small for draw_dial()");

	ThickBrush<Canvas> tb(canvas);

	{
		[[maybe_unused]] typename Canvas::Transaction t;

		// draw the arc
		for (const auto& pt : dial_arc)
			tb.pixel(pgm_read_byte(&pt.x) + x, pgm_read_byte(&pt.y) + y, col);
	}

	// draw the dial line
	const Coord x1 = pgm_read_byte(&dial_arc[position].x) + x;
	const Coord y1 = pgm_read_byte(&dial_arc[position].y) + y;

	draw_line(tb, x + 20, y + 20, x1, y1, col);
}

void draw_dial_at(const char* name, const uint8_t position, Coord x, Coord y, Color col)
{
	{
		Window<43, 36> win(colBlack);
		draw_dial(win, 0, 0, position, col);
		Display::blit(win, x + 11, y);
	}

	Window<64, 8> win(colBlack);
	const uint16_t width = get_text_width_small(name);
	const Coord textx = width > 64 ? 0 : (64 - width) / 2;
	print_small(win, name, textx, 0, col, colBlack);
	Display::blit(win, x, y + 38);
}

void refresh_screen()
{
	Display d;

	// the battery
	const uint8_t battery = rand() & 0x7f;
	fill_rect(d, 0, 0, battery, 3, colGreen);
	fill_rect(d, battery, 0, 128 - battery, 3, colBlack);

	// print some text
	{
		const char* names[] = {"Reverb", "Chorus", "Delay"};
		const uint8_t namendx = rand() % 3;
		const uint16_t width = get_text_width_large(names[namendx]);

		const Coord x = width > 128 ? 0 : (128 - width) / 2;

		Window<128, 19> win(colBlack);
		print_large(win, names[namendx], x, 0, colWhite);
		d.blit(win, 0, 10);
	}

	// draw the knobs
	draw_dial_at("green", rand() % DIAL_ARC_POINTS, 0, 36, colGreen);
	draw_dial_at("blue", rand() % DIAL_ARC_POINTS, 64, 36, colBlue);
	draw_dial_at("red", rand() % DIAL_ARC_POINTS, 0, 95, colRed);
	draw_dial_at("white", rand() % DIAL_ARC_POINTS, 64, 95, colWhite);
}

int main()
{
	init_hw();

	Display d;

	d.init();

	fill(d, colBlack);

	XPT2046_Touchscreen ts;
	ts.begin();

	while (true)
	{
		static uint16_t prev = 0;
		uint16_t cnt = Watch::cnt();

		if (!tirq::in())
		{
			ts.touched();
			char buff[20];
			uint16_t diff = cnt - prev;
			sprintf(buff, "%u", diff);
			Window<100, 18> win(colBlack);
			//fill_rect(d, 0, 0, 100, 20, colBlack);
			print_large(win, buff, 0, 0, colWhite);
			d.blit(win, 0, 0);
		}

		prev = cnt;

		//_delay_ms(1000);
	}
}
