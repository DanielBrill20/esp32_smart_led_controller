#ifndef LED_MANAGER_H
#define LED_MANAGER_H

#include <string.h>
#include "esp_err.h"
#include "esp_log.h"
#include "led_strip.h"

enum {
    RED,
    GREEN,
    BLUE
};

typedef enum {
    LED_MODE_LIGHT,
    LED_MODE_BLINKY,
    LED_MODE_MORSE
} led_mode_t;

typedef struct {
    led_mode_t mode;
    bool state;
    uint8_t rgb[3];
    uint32_t blinky_dur;
    char* morse_code;
} led_config_t;

typedef struct led_t led_t; // Forward struct declaration

struct led_t {
    led_strip_handle_t led_handle;
    uint8_t index;
    led_config_t config;
    void (*set_mode)(led_t*, led_mode_t);
    void (*set_rgb)(led_t*, uint8_t, uint8_t, uint8_t);
};

led_t* led_manager_get_led();
void led_manager_init();

#endif // LED_MANAGER_H
