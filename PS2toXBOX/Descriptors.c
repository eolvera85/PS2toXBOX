#include "Descriptors.h"

const USB_Descriptor_Device_t PROGMEM DeviceDescriptor2 =
{
	.Header                 = { .Size = sizeof(USB_Descriptor_Device_t), .Type = DTYPE_Device },
	.USBSpecification       = VERSION_BCD(1,1,0),
	.Class                  = USB_CSCP_NoDeviceClass,
	.SubClass               = USB_CSCP_NoDeviceSubclass,
	.Protocol               = USB_CSCP_NoDeviceProtocol,
	.Endpoint0Size          = 0x08, //**
	.VendorID               = 0x045E,
	.ProductID              = 0x0202,
	.ReleaseNumber          = VERSION_BCD(1,0,0),
	.ManufacturerStrIndex   = 0x00,
	.ProductStrIndex        = 0x00,
	.SerialNumStrIndex      = NO_DESCRIPTOR,
	.NumberOfConfigurations = 1
};

const uint8_t PROGMEM DeviceDescriptor[] =
{
	0x12,        // bLength
	0x01,        // bDescriptorType (Device)
	0x10, 0x01,  // bcdUSB 1.10
	0x00,        // bDeviceClass 
	0x00,        // bDeviceSubClass 
	0x00,        // bDeviceProtocol 
	0x08,        // bMaxPacketSize0 8
	0x5E, 0x04,  // idVendor 0x045E
	0x02, 0x02,  // idProduct 0x0202
	0x00, 0x01,  // bcdDevice 1.00
	0x00,        // iManufacturer (String Index)
	0x00,        // iProduct (String Index)
	0x00,        // iSerialNumber (String Index)
	0x01        // bNumConfigurations 1
};

const USB_Descriptor_Configuration_t PROGMEM ConfigurationDescriptor2 =
{
	.Config =
		{
			.Header                 = { .Size = sizeof(USB_Descriptor_Configuration_Header_t), .Type = DTYPE_Configuration },
			.TotalConfigurationSize = 0x20,
			.TotalInterfaces        = 1,
			.ConfigurationNumber    = 1,
			.ConfigurationStrIndex  = NO_DESCRIPTOR,
			.ConfigAttributes       = USB_CONFIG_ATTR_RESERVED,
			.MaxPowerConsumption    = USB_CONFIG_POWER_MA(100)
		},
	.HID_Interface =
		{
			.Header                 = { .Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface },
			.InterfaceNumber        = 0x00,
			.AlternateSetting       = 0x00,
			.TotalEndpoints         = 2,
			.Class                  = 0x58,
			.SubClass               = 0x42,
			.Protocol               = HID_CSCP_NonBootProtocol,
			.InterfaceStrIndex      = NO_DESCRIPTOR
		},
	.HID_ReportINEndpoint =
		{
			.Header                 = { .Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint },
			.EndpointAddress        = JOYSTICK_EPADDR_IN,
			.Attributes             = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
			.EndpointSize           = JOYSTICK_EPSIZE,
			.PollingIntervalMS      = 0x04
		},
	.HID_ReportOUTEndpoint =
		{
			.Header                 = { .Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint },
			.EndpointAddress        = JOYSTICK_EPADDR_OUT,
			.Attributes             = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
			.EndpointSize           = JOYSTICK_EPSIZE, //0x08,
			.PollingIntervalMS      = 0x04
		}
};

const uint8_t PROGMEM ConfigurationDescriptor[] =
{
//Configuration Descriptor:	(Config)
	0x09,        // bLength
	0x02,        // bDescriptorType (Configuration)
	0x20, 0x00,  // wTotalLength 32
	0x01,        // bNumInterfaces 1
	0x01,        // bConfigurationValue
	0x00,        // iConfiguration (String Index)
	0x80,        // bmAttributes Remote Wakeup
	0xFA,        // bMaxPower 500mA
	
//Interface Descriptor:	(HID_Interface)
	0x09,        // bLength
	0x04,        // bDescriptorType (Interface)
	0x00,        // bInterfaceNumber 0
	0x00,        // bAlternateSetting
	0x02,        // bNumEndpoints 2
	0x58,        // bInterfaceClass (USB_CLASS_XID)
	0x42,        // bInterfaceSubClass (USB_DT_XID)
	0x00,        // bInterfaceProtocol
	0x00,        // iInterface (String Index)
//Endpoint Descriptor:	(HID_ReportINEndpoint)	
	0x07,        // bLength
	0x05,        // bDescriptorType (Endpoint)
	0x82,        // bEndpointAddress (IN/D2H)
	0x03,        // bmAttributes (Interrupt)
	0x20, 0x00,  // wMaxPacketSize 32!!!(Although xbox controller marks 32bytes, on VUSB only 8bytes can be sent! But this version is modded to send 20 bytes) change 0x20 to 0x08 for windows support, but no xbox
	0x04,        // bInterval 4 (unit depends on device speed)
	
//Endpoint Descriptor:	(HID_ReportOUTEndpoint)	
	0x07,        // bLength
	0x05,        // bDescriptorType (Endpoint)
	0x02,        // bEndpointAddress (OUT/H2D)
	0x03,        // bmAttributes (Interrupt)
	0x20, 0x00,  // wMaxPacketSize 32 change 0x20 to 0x08 for windows support, but no xbox
	0x04       // bInterval 4 (unit depends on device speed)
};

const USB_Descriptor_String_t PROGMEM LanguageString =
{
	.Header                 = {.Size = USB_STRING_LEN(1), .Type = DTYPE_String},
	.UnicodeString          = {LANGUAGE_ID_ENG}
};

const USB_Descriptor_String_t PROGMEM ManufacturerString =
{
	.Header                 = {.Size = USB_STRING_LEN(20), .Type = DTYPE_String},
	.UnicodeString          = L"eolvera85"
};

const USB_Descriptor_String_t PROGMEM ProductString =
{
	.Header                 = {.Size = USB_STRING_LEN(20), .Type = DTYPE_String},
	.UnicodeString          = L"PSX TO OG XBOX"
};

uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
                                    const uint16_t wIndex,
                                    const void** const DescriptorAddress)
{
	const uint8_t  DescriptorType   = (wValue >> 8);
	const uint8_t  DescriptorNumber = (wValue & 0xFF);
	const void* Address = NULL;
	uint16_t    Size    = NO_DESCRIPTOR;

	switch (DescriptorType)
	{
		case DTYPE_Device:
			Address = &DeviceDescriptor;
			Size    = sizeof(DeviceDescriptor);
			break;
		case DTYPE_Configuration:
			Address = &ConfigurationDescriptor;
			Size    = sizeof(ConfigurationDescriptor);
			break;
		case DTYPE_String:
			switch (DescriptorNumber)
			{
				case 0x00:
					Address = &LanguageString;
					Size    = pgm_read_byte(&LanguageString.Header.Size);
					break;
				case 0x01:
					Address = &ManufacturerString;
					Size    = pgm_read_byte(&ManufacturerString.Header.Size);
					break;
				case 0x02:
					Address = &ProductString;
					Size    = pgm_read_byte(&ProductString.Header.Size);
					break;
			}

			break;
	}

	*DescriptorAddress = Address;
	return Size;
}
