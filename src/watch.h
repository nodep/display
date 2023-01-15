#pragma once

template <uint16_t Prescale>
class RTWatch
{
public:
	constexpr static RTC_PRESCALER_t get_prescaler()
	{
		if constexpr(Prescale == 1)
			return RTC_PRESCALER_DIV1_gc;
		else if constexpr(Prescale == 2)
			return RTC_PRESCALER_DIV2_gc;
		else if constexpr(Prescale == 4)
			return RTC_PRESCALER_DIV4_gc;
		else if constexpr(Prescale == 8)
			return RTC_PRESCALER_DIV8_gc;
		else if constexpr(Prescale == 16)
			return RTC_PRESCALER_DIV16_gc;
		else if constexpr(Prescale == 32)
			return RTC_PRESCALER_DIV32_gc;
		else if constexpr(Prescale == 64)
			return RTC_PRESCALER_DIV64_gc;
		else if constexpr(Prescale == 128)
			return RTC_PRESCALER_DIV128_gc;
		else if constexpr(Prescale == 256)
			return RTC_PRESCALER_DIV256_gc;
		else if constexpr(Prescale == 512)
			return RTC_PRESCALER_DIV512_gc;
		else if constexpr(Prescale == 1024)
			return RTC_PRESCALER_DIV1024_gc;
		else if constexpr(Prescale == 2048)
			return RTC_PRESCALER_DIV2048_gc;
		else if constexpr(Prescale == 4096)
			return RTC_PRESCALER_DIV4096_gc;
		else if constexpr(Prescale == 8192)
			return RTC_PRESCALER_DIV8192_gc;
		else if constexpr(Prescale == 16384)
			return RTC_PRESCALER_DIV16384_gc;

		return RTC_PRESCALER_DIV32768_gc;
	}

	static void start()
	{
		//RTC.CLKSEL = RTC_CLKSEL_XOSC32K_gc;
		RTC.CTRLA = get_prescaler() | RTC_RTCEN_bm;
	}

	static uint16_t cnt()
	{
		return RTC.CNT;
	}
};
