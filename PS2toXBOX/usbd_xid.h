// Copyright 2021, Ryan Wendland, ogx360
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef _XID_h
#define _XID_h

#define _USING_XID

#include <Arduino.h>
#include <PluggableUSB.h>

#define XID_INTERFACECLASS 88
#define XID_INTERFACESUBCLASS 66

#ifndef _USING_HID
#define HID_GET_REPORT 0x01
#define HID_SET_REPORT 0x09
#define HID_REPORT_TYPE_INPUT 1
#define HID_REPORT_TYPE_OUTPUT 2
#endif

#define XID_EP_IN (pluggedEndpoint)
#define XID_EP_OUT (pluggedEndpoint + 1)

static const DeviceDescriptor xid_dev_descriptor PROGMEM =
    D_DEVICE(0x00, 0x00, 0x00, USB_EP_SIZE, USB_VID, USB_PID, 0x0121, 0, 0, 0, 1);

static const uint8_t DUKE_DESC_XID[] PROGMEM = {
    0x10,
    0x42,
    0x00, 0x01,
    0x01,
    0x02,
    0x14,
    0x06,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

static const uint8_t DUKE_CAPABILITIES_IN[] PROGMEM = {
    0x00,
    0x14,
    0xFF,
    0x00,
    0xFF,
    0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF};

static const uint8_t DUKE_CAPABILITIES_OUT[] PROGMEM = {
    0x00,
    0x06,
    0xFF, 0xFF, 0xFF, 0xFF};

typedef struct __attribute__((packed))
{
    InterfaceDescriptor interface;
    EndpointDescriptor in;
    EndpointDescriptor out;
} XIDDescriptor;

typedef struct __attribute__((packed))
{
    uint8_t startByte;
    uint8_t bLength;
    uint16_t wButtons;
    uint8_t A;
    uint8_t B;
    uint8_t X;
    uint8_t Y;
    uint8_t BLACK;
    uint8_t WHITE;
    uint8_t L;
    uint8_t R;
    int16_t leftStickX;
    int16_t leftStickY;
    int16_t rightStickX;
    int16_t rightStickY;
} usbd_duke_in_t;

typedef struct __attribute__((packed))
{
    uint8_t startByte;
    uint8_t bLength;
    uint8_t lValuePadding;
    uint8_t lValue;
    uint8_t hValuePadding;
    uint8_t hValue;
} usbd_duke_out_t;

class XID_ : public PluggableUSBModule
{
public:
    XID_(void);
    int begin(void);
    int sendReport(const void *data, int len);
    int getReport(void *data, int len);

protected:
    int getInterface(uint8_t *interfaceCount);
    int getDescriptor(USBSetup &setup);
    bool setup(USBSetup &setup);

private:
    uint8_t epType[2];
    uint8_t xid_in_data[32];
    uint8_t xid_out_data[32];
    uint32_t xid_out_expired;
};

XID_ &XID();

#endif // _XID_h
