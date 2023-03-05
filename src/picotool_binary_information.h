#ifndef PICOTOOL_BINARY_INFORMATION_H
#define PICOTOOL_BINARY_INFORMATION_H

#include "pico/binary_info.h"
#include "pins.h"

bi_decl(bi_program_name("rp2040-dmxinput"));
bi_decl(bi_program_description("Sandbox for DMX input tests"));
bi_decl(bi_program_url("https://github.com/kripton/rp2040-dmxinput"));

bi_decl(bi_program_feature("Reboot on baudrate change: 1200 = Bootloader, 2400 = reset"));

bi_decl(bi_4pins_with_names(
    PIN_DMXIN_0, "DMX input, universe 0",
    PIN_DMXIN_1, "DMX input, universe 1",
    PIN_DMXIN_2, "DMX input, universe 2",
    PIN_DMXIN_3, "DMX input, universe 3")
);

#endif // PICOTOOL_BINARY_INFORMATION_H
