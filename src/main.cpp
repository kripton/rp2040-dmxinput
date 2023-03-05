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
}

extern "C" {

}

int main() {
    // Init the on-board LED
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    while (true) {
        sleep_us(10);
    }
};
