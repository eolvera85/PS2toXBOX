#ifndef _XBOXPAD_H_
#define _XBOXPAD_H_

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <string.h>
#include "Descriptors.h"

typedef struct {
	uint8_t rid;
	uint8_t rsize;
	uint8_t digital_buttons;
	uint8_t reserved_1;
	uint8_t a;
	uint8_t b;
	uint8_t x;
	uint8_t y;
	uint8_t black;
	uint8_t white;
	uint8_t l;
	uint8_t r;
	int l_x;
	int l_y;
	int r_x;
	int r_y;
	uint8_t reserved_2[8];
	uint8_t reserved_3[8];
} USB_JoystickReport_Data_t;

typedef struct {
  uint8_t rid; 
  uint8_t rsize;     // 0x06
  uint8_t left_padding;
	uint8_t left_rumble;
  uint8_t right_padding;
  uint8_t right_rumble;
} USB_JoystickReport_Rumble_t; 

extern USB_JoystickReport_Data_t gamepad_state;
extern USB_JoystickReport_Rumble_t gamepad_rumble;

void xbox_reset_pad_status(void);
void xbox_send_pad_state(void);
void xbox_reset_watchdog(void);
void xbox_init(bool watchdog);
int xbox_pad_detected(void);
void xbox_set_detected_callback(void (*callbackPtr)(void));
void xbox_set_connect_callback(void (*callbackPtr)(void));
void xbox_set_disconnect_callback(void (*callbackPtr)(void));

#define XBOX_NONE			0x00
#define XBOX_DPAD_UP		0x01
#define XBOX_DPAD_DOWN		0x02
#define XBOX_DPAD_LEFT		0x04
#define XBOX_DPAD_RIGHT		0x08
#define XBOX_START			0x10
#define XBOX_BACK			0x20
#define XBOX_LEFT_STICK		0x40
#define XBOX_RIGHT_STICK	0x80

#endif