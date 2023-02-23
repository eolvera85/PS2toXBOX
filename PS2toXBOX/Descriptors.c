#include "Descriptors.h"

const USB_Descriptor_Device_t PROGMEM DeviceDescriptor =
{
	.Header                 = {.Size = sizeof(USB_Descriptor_Device_t), .Type = DTYPE_Device},
	.USBSpecification       = VERSION_BCD(1,1,0),
	.Class                  = USB_CSCP_NoDeviceClass,
	.SubClass               = USB_CSCP_NoDeviceSubclass,
	.Protocol               = USB_CSCP_NoDeviceProtocol,
	.Endpoint0Size          = 0x08,
	.VendorID               = 0x045E,
	.ProductID              = 0x0202,
	.ReleaseNumber          = VERSION_BCD(1,0,0),
	.ManufacturerStrIndex   = 0x00,
	.ProductStrIndex        = 0x00,
	.SerialNumStrIndex      = NO_DESCRIPTOR,
	.NumberOfConfigurations = 0x01
};

const USB_Descriptor_Configuration_t PROGMEM ConfigurationDescriptor =
{
	.Config =
		{
			.Header                 = {.Size = sizeof(USB_Descriptor_Configuration_Header_t), .Type = DTYPE_Configuration},
			.TotalConfigurationSize = 0x20,
			.TotalInterfaces        = 1,
			.ConfigurationNumber    = 1,
			.ConfigurationStrIndex  = NO_DESCRIPTOR,
			.ConfigAttributes       = USB_CONFIG_ATTR_RESERVED,
			.MaxPowerConsumption    = USB_CONFIG_POWER_MA(100)
		},
	.HID_Interface =
		{
			.Header                 = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},
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
			.Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},
			.EndpointAddress        = JOYSTICK_EPADDR_IN,
			.Attributes             = (EP_TYPE_INTERRUPT),
			.EndpointSize           = JOYSTICK_EPSIZE,
			.PollingIntervalMS      = 0x04
		},

	.HID_ReportOUTEndpoint =
		{
			.Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},
			.EndpointAddress        = JOYSTICK_EPADDR_OUT,
			.Attributes             = (EP_TYPE_INTERRUPT),
			.EndpointSize           = 0x08,
			.PollingIntervalMS      = 0x04
		}
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
			Size    = sizeof(USB_Descriptor_Device_t);
			break;
		case DTYPE_Configuration:
			Address = &ConfigurationDescriptor;
			Size    = sizeof(USB_Descriptor_Configuration_t);
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
