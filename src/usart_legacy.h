#pragma once

struct Usart
{
private:
	static void set_9600()
	{
		#define BAUD 9600
		#include <util/setbaud.h>

		UBRR0H = UBRRH_VALUE;
		UBRR0L = UBRRL_VALUE;

		#if USE_2X
			UCSR0A |= (1 << U2X0);
		#else
			UCSR0A &= ~(1 << U2X0);
		#endif
	}

	static void set_57600()
	{
		#undef BAUD
		#define BAUD 57600
		#include <util/setbaud.h>

		UBRR0H = UBRRH_VALUE;
		UBRR0L = UBRRL_VALUE;

		#if USE_2X
			UCSR0A |= (1 << U2X0);
		#else
			UCSR0A &= ~(1 << U2X0);
		#endif
	}

public:
	constexpr static void set_baud(const uint32_t baud)
	{
		if (baud == 9600)
			set_9600();
		else if (baud == 57600)
			set_57600();
	}

	static void enable(const bool tx, const bool rx)
	{
	    UCSR0B = (tx ? _BV(TXEN0) : 0) | (rx ? _BV(RXEN0) : 0);
	}

	static void send_byte(const uint8_t b)
	{
	    loop_until_bit_is_set(UCSR0A, UDRE0);
	    UDR0 = b;
	}

	static bool read_byte(uint8_t& b)
	{
		if (UCSR0A & _BV(RXC0))
		{
			b = UDR0;
			return true;
		}

		return false;
	}
};
