#pragma once

#include "iopin.h"
#include "spimaster.h"
#include "watch.h"

#ifdef __AVR_ATmega2560__
    #include "iopin_legacy.h"
    #include "timer1_legacy.h"
    #include "usart_legacy.h"

    using led = IoPin<'B', 7>;
    using Watch = Timer1<1024>;

#else
    #include "iopin.h"
    #include "timera.h"
    #include "usart.h"

    using led = IoPin<'C', 6>;
    using btn = IoPin<'C', 7>;
    using Watch = TimerA<1, 1024>;

#endif

// the pins and HW we use here
using miso	= IoPin<'A', 5>;
using mosi	= IoPin<'A', 4>;
using sck	= IoPin<'A', 6>;
using ss	= IoPin<'A', 7>;
using rst	= IoPin<'B', 0>;
using dc	= IoPin<'B', 1>;
using spi	= SpiMaster<0, 6>;

void init_hw();