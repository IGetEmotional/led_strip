#include <main.h>
#include <math.h>

#ifndef WS2812_H_

#define WS2812_H_

//--------------------------------------------------

#include "stm32l0xx_hal.h"

#include <string.h>

#include <stdlib.h>

#include <stdint.h>

//--------------------------------------------------
#define DELAY_LEN 48
#define LEN_COUNT 30

#define ARRAY_LEN DELAY_LEN + LEN_COUNT*24

//--------------------------------------------------

#endif /* WS2812_H_ */




void send(int Green, int Red, int Blue);

void set_LED(int LEDnum, int Red, int Green, int Blue);

void led_send (void);

void set_Brightness(int brightness);

uint8_t rainbow_effect(void);

void normal_mode(void);

int change_mode(void);

int fade_mode(void);

void set_rgb_lights(void);

