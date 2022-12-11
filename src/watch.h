#pragma once

template <class Timer>
class TimerUtil : public Timer
{
public:
	static uint32_t ticks2ms_long(const uint32_t ticks)
	{
		return ticks * Timer::div / (F_CPU / 1000);
	}

	static uint16_t ticks2ms(const uint16_t ticks)
	{
		return ticks * Timer::div / (F_CPU / 1000);
	}

	static uint32_t ms2ticks(const uint32_t ms)
	{
		return ms * (F_CPU / 1000) / Timer::div;
	}

	static bool has_ms_passed_since(const uint16_t ms, const uint16_t since)
	{
		return static_cast<uint32_t>(Timer::cnt() - since) >= ms2ticks(ms);
	}

	static uint16_t ms_since(const uint16_t since)
	{
		return ticks2ms(Timer::cnt() - since);
	}
};
