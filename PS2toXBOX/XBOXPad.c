#include "XBOXPad.h"

USB_JoystickReport_Data_t gamepad_state;

static int padDetected = 0;
void (*padDetectedCallback)(void) = NULL;
void (*padUSBConnectEventCallback)(void) = NULL;
void (*padUSBDisconnectEventCallback)(void) = NULL;

void EVENT_USB_Device_Connect(void) {
	if(padUSBConnectEventCallback)
		padUSBConnectEventCallback();
}

void EVENT_USB_Device_Disconnect(void) {
	if(padUSBDisconnectEventCallback)
		padUSBDisconnectEventCallback();
}

void EVENT_USB_Device_ConfigurationChanged(void) {
	Endpoint_ConfigureEndpoint(JOYSTICK_EPADDR_IN, EP_TYPE_INTERRUPT, 20, 1);
}

void EVENT_USB_Device_ControlRequest(void) {
	switch (USB_ControlRequest.bRequest) {
		case HID_REQ_GetReport:
			if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE)) {
				Endpoint_ClearSETUP();
				Endpoint_Write_Control_Stream_LE(&gamepad_state, 20);
				Endpoint_ClearOUT();
			}
		break;
		default:
			if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_VENDOR | REQREC_INTERFACE) &&  USB_ControlRequest.bRequest == 0x06) {
				Endpoint_ClearSETUP();
				Endpoint_Write_Control_Stream_LE(gamepad_state.reserved_2, 16);
				Endpoint_ClearOUT();
				padDetected = 1;

				if(padDetectedCallback)
					padDetectedCallback();
			}
		break;
	}
}

void HID_Task(void) {
	if (USB_DeviceState != DEVICE_STATE_Configured)
		return;

	Endpoint_SelectEndpoint(JOYSTICK_EPADDR_IN);

	if (Endpoint_IsINReady()) {
		Endpoint_Write_Stream_LE(&gamepad_state, 20, NULL);

		Endpoint_ClearIN();
	}
}

void xbox_reset_pad_status(void) {
	memset(&gamepad_state, 0x00, sizeof(USB_JoystickReport_Data_t));

	gamepad_state.rsize = 20;

	gamepad_state.reserved_2[0] = 16;
	gamepad_state.reserved_2[1] = 66;
	gamepad_state.reserved_2[2] = 0;
	gamepad_state.reserved_2[3] = 1;
	gamepad_state.reserved_2[4] = 1;
	gamepad_state.reserved_2[5] = 2;
	gamepad_state.reserved_2[6] = 20;
	gamepad_state.reserved_2[7] = 6;

	for (int i = 0; i < 8; i++) 
		gamepad_state.reserved_3[i] = 0xFF;
}

void xbox_send_pad_state(void) {
	HID_Task();
	USB_USBTask();
}

void xbox_reset_watchdog(void) {
	wdt_reset();
}

void xbox_init(bool watchdog) {
	if (watchdog)
		wdt_enable(WDTO_2S);
	else
		wdt_disable();

	xbox_reset_pad_status();

	USB_Init();

	sei();
}

int xbox_pad_detected(void) {
	return padDetected;
}

void xbox_set_detected_callback(void (*callbackPtr)(void)) {
	padDetectedCallback = callbackPtr;
}

void xbox_set_connect_callback(void (*callbackPtr)(void)) {
	padUSBConnectEventCallback = callbackPtr;
}

void xbox_set_disconnect_callback(void (*callbackPtr)(void)) {
	padUSBDisconnectEventCallback = callbackPtr;
}