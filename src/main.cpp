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
    gpio_put(PIN_DEBUG_2, 1);
    
    //sleep_us(1); // Give the Logic Analyzer some time to see the signal

    printf("Channels received: %ld\n", instance->_channels_captured);

    //printf("DMX input received (byte counter)\n");
    // Toggle the LED
    gpio_put(PICO_DEFAULT_LED_PIN, !gpio_get(PICO_DEFAULT_LED_PIN));

    gpio_put(PIN_DEBUG_2, 0);
}

int main() {
    // Init the on-board LED
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    // DEBUG/measurement pins
    gpio_init(PIN_DEBUG_0);
    gpio_set_dir(PIN_DEBUG_0, GPIO_OUT);
    gpio_init(PIN_DEBUG_1);
    gpio_set_dir(PIN_DEBUG_1, GPIO_OUT);
    gpio_init(PIN_DEBUG_2);
    gpio_set_dir(PIN_DEBUG_2, GPIO_OUT);
    gpio_init(19);
    gpio_set_dir(19, GPIO_OUT);
    gpio_init(20);
    gpio_set_dir(20, GPIO_OUT);
    //gpio_init(21);
    //gpio_set_dir(21, GPIO_OUT);
    gpio_init(22);
    gpio_set_dir(22, GPIO_OUT);

    stdio_init_all();

    // TODO: Works fine if NUMCHANNELS <= number of channels on wire
    //       DMA state still seems to be reset incorrectly ...

    // Set up a DMX input
#define NUMCHANNELS 600
    DmxInput::return_code retVal = in_0.begin(PIN_DMXIN_0, 0, NUMCHANNELS, pio1, false);
    in_0.read_async(DMXBuffer[0], input_updated_c);

    while (true) {
        sleep_ms(1000);
    }
};
