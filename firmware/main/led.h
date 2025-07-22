#ifndef LED_H
#define LED_H

#include "led_strip.h"

typedef struct {
    led_strip_handle_t led_handle;
    int index;
    int red;
    int green;
    int blue;
    bool state;
} led;

#endif // LED_H