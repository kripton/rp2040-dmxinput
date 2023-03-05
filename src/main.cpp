/**
 * Copyright (c) 2021 Open Lighting Project
 *
 * SPDX-License-Identifier: GPL-3.0
 */

extern "C" {
#include <stdio.h>
#include <hardware/gpio.h>      // To "manually" control the trigger pin

#include <pico/stdlib.h>

#include "pins.h"
#include "picotool_binary_information.h"

#include "DmxInput.h"
}

extern "C" {
    uint8_t DMXBuffer[8][513]; // 8 universes, start code + 512 channels
}

DmxInput in_0;   // The instance of DmxInput for universe 0
DmxInput in_1;   // The instance of DmxInput for universe 1
DmxInput in_2;   // The instance of DmxInput for universe 2
DmxInput in_3;   // The instance of DmxInput for universe 3

void __isr input_updated_c(DmxInput* instance) {
    printf("DMX input received\n");
    // Toggle the LED
    gpio_put(PICO_DEFAULT_LED_PIN, !gpio_get(PICO_DEFAULT_LED_PIN));
}

int main() {
    // Init the on-board LED
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    stdio_init_all();

    // Set up a DMX input
    DmxInput::return_code retVal = in_0.begin(PIN_DMXIN_0, 0, 512, pio1, false);
    in_0.read_async(DMXBuffer[0], input_updated_c);

    while (true) {
        sleep_ms(1000);
        printf("ping\n");
    }
};
