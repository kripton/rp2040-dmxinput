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

alarm_pool_t* alarmPool;

alarm_id_t in_0_alarm;

int64_t __isr alarmFired(alarm_id_t id, void *user_data) {
    // An alarm fired. That means that 88us passed since the last falling edge
    // => We assume the DMX frame is finished

    if (id == in_0_alarm) {
        // Toggle the LED :)
        gpio_put(PIN_DEBUG_1, 1); gpio_put(PICO_DEFAULT_LED_PIN, 1); sleep_us(2); 
        //printf ("ALARM IN 0 fired!\n");
        gpio_put(PIN_DEBUG_1, 0);
        gpio_put(PICO_DEFAULT_LED_PIN, 0);
    }

    // Do not re-schedule the alarm
    return 0;
}

void __isr input_updated_c(DmxInput* instance) {
    gpio_put(PIN_DEBUG_2, 1);  sleep_us(2); 
    //printf("DMX input received (byte counter)\n");
    // Toggle the LED
    //gpio_put(PICO_DEFAULT_LED_PIN, !gpio_get(PICO_DEFAULT_LED_PIN));
    gpio_put(PIN_DEBUG_2, 0);
}

void __isr gpio_fallingEdge(uint gpio, uint32_t events) {
    gpio_put(PIN_DEBUG_0, 1);
    if (events != GPIO_IRQ_EDGE_FALL) {
        return;
    }
    switch (gpio) {
        case PIN_DMXIN_0:
            //gpio_put(PICO_DEFAULT_LED_PIN, !gpio_get(PICO_DEFAULT_LED_PIN));
            // Stop the currently running BREAK detect timer
            alarm_pool_cancel_alarm(alarmPool, in_0_alarm);
            // Start a new timer RINGing after 88us from NOW
            in_0_alarm = alarm_pool_add_alarm_in_us(alarmPool, 88, alarmFired, nullptr, false);
            break;
        case PIN_DMXIN_1:
            break;
        case PIN_DMXIN_2:
            break;
        case PIN_DMXIN_3:
            break;
    }
    gpio_put(PIN_DEBUG_0, 0);
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

    stdio_init_all();

    // Request one hardware alarm to be used by our alarm pool for ALL DMX IN lines
    // int hardwareAlarm = hardware_alarm_claim_unused(true);

    // Create a new pool with max 18 alarms (16 + 2 reserve) using our freshly claimed hardware alarm

    alarmPool = alarm_pool_create(2, 18);

    // Set up a timer that RINGs after 88us (after the last falling edge)
    in_0_alarm = alarm_pool_add_alarm_in_us(alarmPool, 88, alarmFired, nullptr, false);

    // Set up IRQ on falling edge
    gpio_set_irq_enabled_with_callback(PIN_DMXIN_0, GPIO_IRQ_EDGE_FALL, true, &gpio_fallingEdge);

    // Set up a DMX input
    DmxInput::return_code retVal = in_0.begin(PIN_DMXIN_0, 0, 512, pio1, false);
    in_0.read_async(DMXBuffer[0], input_updated_c);

    while (true) {
        sleep_ms(1000);
        printf("ping\n");
    }
};
