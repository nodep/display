#pragma once

#include <avr/io.h>

// 16 timer found in most classic AVRs

template <uint16_t Prescale>
class Timer1
{
private:
	constexpr static uint8_t get_clksel()
	{
		if constexpr(Prescale == 1)
			return _BV(CS10);
		else if constexpr(Prescale == 8)
			return _BV(CS11);
		else if constexpr(Prescale == 64)
			return _BV(CS11) | _BV(CS10);
		else if constexpr(Prescale == 256)
			return _BV(CS12);
		else if constexpr(Prescale == 1024)
			return _BV(CS12) | _BV(CS10);

		return 0;
	}

public:

	static const uint16_t div = Prescale;

	static void start()
	{
		static_assert(get_clksel() != 0, "Unsupported prescale value for Timer1");

		TCCR1B = get_clksel();
	}

	static void stop()
	{
		TCCR1B = 0;
	}

	static uint16_t cnt()
	{
		return TCNT1;
	}
};
