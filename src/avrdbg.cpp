#include <stdio.h>

#include <avr/io.h>

#if _DEBUG

#include "hw.h"
#include "avrdbg.h"

#ifdef __AVR_AVR128DA48__
using DebugUsart = Usart<1>;
#else
using DebugUsart = Usart;
#endif

#ifndef _WIN32
static int serial_putchar(char c, FILE*)
{
	DebugUsart::send_byte(c);
	return 0;
}
#endif

void dbgInit()
{
#ifndef _WIN32
	// we can't use FDEV_SETUP_STREAM, so we improvise
	static FILE mydbgout;

	mydbgout.buf = nullptr;
	mydbgout.put = serial_putchar;
	mydbgout.get = nullptr;
	mydbgout.flags = _FDEV_SETUP_WRITE;
	mydbgout.udata = 0;

	stdout = &mydbgout;
#endif

#ifdef __AVR_AVR128DA48__
	// USART1.TX is on C0 on avr128da48,
	// and we have to configure it for output
	IoPin<'C', 0>::dir_out();
#endif

	// baud rate calc
	DebugUsart::set_baud(57600);

	// we only want TX, RX is not needed
	DebugUsart::enable(true, false);

	dprint("\ni live...\n");
}

void printi(uint32_t i)
{
	char buff[11];
	char* start = buff + 9;
	buff[9] = '0';
	buff[10] = '\0';
	for (uint8_t cnt = 9; cnt <= 9  &&  i; --cnt)
	{
		buff[cnt] = '0' + static_cast<char>(i % 10);
		if (buff[cnt] != '0')
			start = buff + cnt;
		i /= 10;
	}
	puts(start);
}

#endif	// _DEBUG
