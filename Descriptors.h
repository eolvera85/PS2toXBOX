

#ifndef _DESCRIPTORS_H_
#define _DESCRIPTORS_H_

#include "LUFAConfig.h"
#include <LUFA.h>
#include <LUFA/LUFA/Drivers/USB/USB.h>

#include <avr/pgmspace.h>

typedef struct
{
	USB_Descriptor_Configuration_Header_t Config;
	USB_Descriptor_Interface_t            HID_Interface;
  USB_Descriptor_Endpoint_t             HID_ReportINEndpoint;
  USB_Descriptor_Endpoint_t             HID_ReportOUTEndpoint;
} USB_Descriptor_Configuration_t;

#define JOYSTICK_EPADDR_IN        (ENDPOINT_DIR_IN | 2)
#define JOYSTICK_EPADDR_OUT       (ENDPOINT_DIR_OUT | 2)
#define JOYSTICK_EPSIZE           0x20

uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
		                                const uint16_t wIndex,
		                                const void** const DescriptorAddress)
		                                ATTR_WARN_UNUSED_RESULT ATTR_NON_NULL_PTR_ARG(3);

#endif