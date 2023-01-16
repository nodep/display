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
    #include "watch.h"

    using led = IoPin<'C', 6>;
    using btn = IoPin<'C', 7>;
    using Watch = RTWatch<32>;  // 1024Hz clock

#endif

// the pins and HW we use here
using mosi	= IoPin<'A', 4>;
using miso	= IoPin<'A', 5>;
using sck	= IoPin<'A', 6>;
using ss	= IoPin<'A', 7>;
using rst	= IoPin<'B', 0>;
using dc	= IoPin<'B', 1>;

using tirq  = IoPin<'B', 2>;
using tss   = IoPin<'B', 3>;

using spi	= SpiMaster<0, 5>;

void init_hw();