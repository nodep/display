#pragma once

#include "iopin.h"
#include "timera.h"
#include "usart.h"
#include "spimaster.h"
#include "watch.h"

// define the pins and peripherals we use
using mosi	= IoPin<'A', 4>;
using miso	= IoPin<'A', 5>;
using sck	= IoPin<'A', 6>;
using ss	= IoPin<'A', 7>;
using rst	= IoPin<'B', 0>;
using dc	= IoPin<'B', 1>;
using tss	= IoPin<'B', 3>;

using led   = IoPin<'C', 6>;
using btn   = IoPin<'C', 7>;

using watch = RTWatch<32>;  // 1024Hz clock
using spi	= SpiMaster<0, 5>;

void init_hw();