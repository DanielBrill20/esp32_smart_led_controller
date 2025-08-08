#ifndef LED_MANAGER_H
#define LED_MANAGER_H

#include <string.h>
#include "esp_err.h"
#include "esp_log.h"
#include "led_strip.h"
#include "esp_timer.h"

#define ON true
#define OFF false

typedef enum {
    LED_MODE_LIGHT,
    LED_MODE_BLINKY,
    LED_MODE_MORSE
} led_mode_t;

/**
 * @brief   LED pixel struct handling mode, state, blink duration, morse code, and color, along with additional necessary ESP features
 */
typedef struct led_t led_t; // Forward struct declaration (opaque)

/**
 * @brief   Struct to control blink pattern in Morse Code mode
 */
typedef struct {
    led_t* led;         //!< LED pixel to blink
    uint8_t index;      //!< Index of current character in morse code string
    uint8_t str_len;    //!< Length of current morse code string, set internally based on parsed JSON
    bool gap;           //!< Boolean to indicate the division between dots and dashes to provide a pause between morse code characters, set internally
} morse_iterator_t;

/**
 * @brief   Allocates memory for a new led_t object
 * 
 * @param index: The index of the LED pixel within an led strip, used in configuring the pixel. If using a single LED like a DevKit onboard LED, set index to 0
 * 
 * @return
 *      - A pointer to an led_t instance
 *      - NULL: If memory allocation fails
 */
led_t* create_led(uint8_t index);

/**
 * @brief   Frees the memory of an led_t instance
 * 
 * @param led: LED pixel
 * 
 * @return
 *      - ESP_OK: Memory freed successfully
 *      - ESP_FAIL: If led is NULL (already freed or invalid)
 */
esp_err_t destroy_led(led_t* led);

/**
 * @brief   Sets the LED pixel mode, which starts the appropriate blink sequence on the hardware
 * 
 * @note If an invalid mode is passed, an error is logged, and no action is taken.
 * 
 * @param led: LED pixel
 * @param mode: Mode to begin (enum)
 */
void set_led_mode(led_t* led, led_mode_t mode);

/**
 * @brief   Sets the LED pixel state
 * 
 * @note Only changes the internal data stored in led, doesn't actually push change to hardware. To push to hardware, call set_led_mode
 * 
 * @param led: LED pixel
 * @param state: Whether the LED is on or off
 */
void set_led_state(led_t* led, bool state);

/**
 * @brief   Sets the LED pixel blink duration
 * 
 * @note Only changes the internal data stored in led, doesn't actually push change to hardware. To push to hardware, call set_led_mode
 * 
 * @param led: LED pixel
 * @param duration: Time in milliseconds
 */
void set_led_blink_duration(led_t* led, uint32_t duration);

/**
 * @brief   Sets the LED pixel morse code
 * 
 * @note Only changes the internal data stored in led, doesn't actually push change to hardware. To push to hardware, call set_led_mode
 * 
 * @note Frees the old morse code string if it exists before storing the new one
 * 
 * @param led: LED pixel
 * @param morse_code: String to be stored in preparation to blink
 */
void set_led_morse_code(led_t* led, char* morse_code);

/**
 * @brief   Sets the LED pixel color
 * 
 * @note After changing the internal data stored in led, if the LED hardware is on, this function resets it to ON with the new color (effectively pushing the change to the hardware)
 * 
 * @param led: LED pixel
 * @param red: Red part of color
 * @param green: Green part of color
 * @param blue: Blue part of color
 */
void set_led_rgb(led_t* led, uint8_t red, uint8_t green, uint8_t blue);

/**
 * @brief   Prepares 2 timers for the Blinky and Morse Code modes
 * 
 * @param led: LED pixel
 * @param morse_iterator: Struct passed to morse_code_timer to control blink pattern
 */
void led_timers_init(led_t* led, morse_iterator_t* morse_iterator);

/**
 * @brief   Creates LED strip based on RMT TX channel
 *          Essentially initializes the handle to be used in the led_t struct within http_server
 */
void led_manager_init();

#endif // LED_MANAGER_H
