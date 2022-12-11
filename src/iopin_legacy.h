#pragma once

#include <avr/io.h>

using register8_t = volatile uint8_t;

template <char Port, uint8_t PinNum>
class IoPin
{
protected:

	static_assert(Port >= 'A'  &&  Port <= 'H', "Bad Port in IoPin template argument");
	static_assert(PinNum <= 7, "Bad pin number n IoPin template argument");

	//////////////////
	// helpers
	//////////////////
	constexpr static register8_t& get_port()
	{
		if constexpr (Port == 'A')
			return PORTA;
		else if constexpr (Port == 'B')
			return PORTB;
		else if constexpr (Port == 'C')
			return PORTC;
		else if constexpr (Port == 'D')
			return PORTD;
		else if constexpr (Port == 'E')
			return PORTE;
		else if constexpr (Port == 'F')
			return PORTF;
		else if constexpr (Port == 'G')
			return PORTG;

		return PORTH;
	}

	constexpr static register8_t& get_ddr()
	{
		if constexpr (Port == 'A')
			return DDRA;
		else if constexpr (Port == 'B')
			return DDRB;
		else if constexpr (Port == 'C')
			return DDRC;
		else if constexpr (Port == 'D')
			return DDRD;
		else if constexpr (Port == 'E')
			return DDRE;
		else if constexpr (Port == 'F')
			return DDRF;
		else if constexpr (Port == 'G')
			return DDRG;

		return DDRH;
	}

	constexpr static register8_t& get_pin()
	{
		if constexpr (Port == 'A')
			return PINA;
		else if constexpr (Port == 'B')
			return PINB;
		else if constexpr (Port == 'C')
			return PINC;
		else if constexpr (Port == 'D')
			return PIND;
		else if constexpr (Port == 'E')
			return PINE;
		else if constexpr (Port == 'F')
			return PINF;
		else if constexpr (Port == 'G')
			return PING;

		return PINH;
	}

	constexpr static uint8_t bitmask = 1 << PinNum;

public:

	//////////////////
	// configuration
	//////////////////
	static void dir_out()
	{
		get_ddr() |= bitmask;
	}

	static void dir_in()
	{
		get_ddr() &= ~bitmask;
	}

	//////////////////
	// output
	//////////////////
	static void high()
	{
		get_port() |= bitmask;
	}

	static void low()
	{
		get_port() &= ~bitmask;
	}

	static void toggle()
	{
		get_port() ^= bitmask;
	}

	static void set_value(const bool value)
	{
		if (value)
			high();
		else
			low();
	}

	//////////////////
	// input
	//////////////////
	static bool in()
	{
		return get_pin() & bitmask;
	}

	static void pullup()
	{
		get_port() |= bitmask;
	}

	static void pullup_off()
	{
		get_port() &= ~bitmask;
	}
};
