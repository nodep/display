#pragma once

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

void init_hw();