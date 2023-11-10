#include <PsxControllerBitBang.h>
#include "usbd_xid.h"

#define XBOX_NONE			    0x00
#define XBOX_DPAD_UP		  0x01
#define XBOX_DPAD_DOWN		0x02
#define XBOX_DPAD_LEFT		0x04
#define XBOX_DPAD_RIGHT		0x08
#define XBOX_START			  0x10
#define XBOX_BACK			    0x20
#define XBOX_LEFT_STICK		0x40
#define XBOX_RIGHT_STICK  0x80

#define XBOX_JTK_MAX      32767
#define XBOX_JTK_MIN      -32767

#define ENABLE_SERIAL_DEBUG
#ifdef ENABLE_SERIAL_DEBUG
	#define dstart(spd) do {Serial1.begin (spd); while (!Serial1) {digitalWrite (LED_BUILTIN, (millis () / 500) % 2);}} while (0);
	#define debug(...) Serial1.print (__VA_ARGS__)
	#define debugln(...) Serial1.println (__VA_ARGS__)
#else
	#define dstart(...)
	#define debug(...)
	#define debugln(...)
#endif

const byte PIN_PS2_DAT = 2;
const byte PIN_PS2_CMD = 3;
const byte PIN_PS2_ATT = 4;
const byte PIN_PS2_CLK = 5;

const unsigned long POLLING_INTERVAL = 1000U / 50U;
PsxControllerBitBang<PIN_PS2_ATT, PIN_PS2_CMD, PIN_PS2_DAT, PIN_PS2_CLK> psx;
XID_ usbd_xid;

boolean haveController = false;
byte axisX, axisY;
bool enableAnalogButton;
bool enableAnalogSticks;
bool enableRumble;
usbd_duke_in_t xbox_in;
usbd_duke_out_t xbox_out;

void setup() {
  pinMode (LED_BUILTIN, OUTPUT);
  dstart(115200);

  xbox_in.startByte = 0x00;
  xbox_in.bLength = sizeof(usbd_duke_in_t);

  xbox_out.bLength = sizeof(usbd_duke_out_t);
  xbox_out.hValue = 0xFF;

  debugln(F("Ready!"));
}

void loop() {
  static unsigned long last = 0;

  if (millis() - last >= POLLING_INTERVAL) 
  {
    last = millis();

    if (!haveController) 
    {
		  if (psx.begin()) 
      {
        debugln(F("Controller found!"));

        if (psx.enterConfigMode()) 
          debugln(F("Success enter config mode"));
        else
          debugln(F("Cannot enter config mode"));

        if (psx.enableAnalogSticks())
        {
          debugln(F("Success enable analog sticks"));
          enableAnalogSticks = true;
        }
        else
        {
          debugln(F("Cannot enable analog sticks"));
        }
        
        if (psx.enableAnalogButtons())
        {
          debugln(F("Success enable analog buttons"));
          enableAnalogButton = true;
        }
        else
        {
          debugln(F("Cannot enable analog buttons"));
        }

        if (psx.enableRumble())
        {
          debugln(F("Success enable rumble"));
          enableRumble = true;
        }
        else
        {
            debugln(F("Cannot enable rumble"));
        }

        if (psx.exitConfigMode())
          debugln(F("Success exit config mode"));
        else
          debugln(F("Cannot exit config mode"));

        haveController = true;
		  }
	  }
    else 
    {
		  if (!psx.read())
      {
        debugln(F("Controller lost :("));
			  haveController = false;
      }
      else
      {
        if (enableRumble && xbox_out.bLength == 0x06)
          psx.setRumble(true, xbox_out.hValue);
        else
          psx.setRumble(false, 0x00);

        xbox_in.wButtons = XBOX_NONE;

        if (enableAnalogButton)
        {
          if (psx.getAnalogButton(PSAB_PAD_UP) > 0x00) xbox_in.wButtons |= XBOX_DPAD_UP;
          if (psx.getAnalogButton(PSAB_PAD_DOWN) > 0x00) xbox_in.wButtons |= XBOX_DPAD_DOWN;
          if (psx.getAnalogButton(PSAB_PAD_LEFT) > 0x00) xbox_in.wButtons |= XBOX_DPAD_LEFT;
          if (psx.getAnalogButton(PSAB_PAD_RIGHT) > 0x00) xbox_in.wButtons |= XBOX_DPAD_RIGHT;

          xbox_in.A = psx.getAnalogButton(PSAB_CROSS) > 0x00 ? 0xFF : 0x00;
          xbox_in.B = psx.getAnalogButton(PSAB_CIRCLE) > 0x00 ? 0xFF : 0x00;
          xbox_in.X = psx.getAnalogButton(PSAB_SQUARE) > 0x00 ? 0xFF : 0x00;
          xbox_in.Y = psx.getAnalogButton(PSAB_TRIANGLE) > 0x00 ? 0xFF : 0x00;
          xbox_in.BLACK = psx.getAnalogButton(PSAB_R1) > 0x00 ? 0xFF : 0x00;
          xbox_in.WHITE = psx.getAnalogButton(PSAB_L1) > 0x00 ? 0xFF : 0x00;

          xbox_in.L = psx.getAnalogButton(PSAB_L2);
          xbox_in.R = psx.getAnalogButton(PSAB_R2);
        }
        else
        {
          if (psx.buttonPressed(PSB_PAD_UP)) xbox_in.wButtons |= XBOX_DPAD_UP;
          if (psx.buttonPressed(PSB_PAD_DOWN)) xbox_in.wButtons |= XBOX_DPAD_DOWN;
          if (psx.buttonPressed(PSB_PAD_LEFT)) xbox_in.wButtons |= XBOX_DPAD_LEFT;
          if (psx.buttonPressed(PSB_PAD_RIGHT)) xbox_in.wButtons |= XBOX_DPAD_RIGHT;

          xbox_in.A = psx.buttonPressed(PSB_CROSS) ? 0xFF : 0x00;
          xbox_in.B = psx.buttonPressed(PSB_CIRCLE) ? 0xFF : 0x00;
          xbox_in.X = psx.buttonPressed(PSB_SQUARE) ? 0xFF : 0x00;
          xbox_in.Y = psx.buttonPressed(PSB_TRIANGLE) ? 0xFF : 0x00;
          xbox_in.BLACK = psx.buttonPressed(PSB_R1) ? 0xFF : 0x00;
          xbox_in.WHITE = psx.buttonPressed(PSB_L1) ? 0xFF : 0x00;
          xbox_in.L = psx.buttonPressed(PSB_L2) ? 0xFF : 0x00;
          xbox_in.R = psx.buttonPressed(PSB_R2) ? 0xFF : 0x00;
        }

        if (psx.buttonPressed(PSB_START)) xbox_in.wButtons |= XBOX_START;
        if (psx.buttonPressed(PSB_SELECT)) xbox_in.wButtons |= XBOX_BACK;
        if (psx.buttonPressed(PSB_L3)) xbox_in.wButtons |= XBOX_LEFT_STICK;
        if (psx.buttonPressed(PSB_R3)) xbox_in.wButtons |= XBOX_RIGHT_STICK;

        if (enableAnalogSticks)
        {
	        psx.getLeftAnalog(axisX, axisY);
          xbox_in.leftStickX = map(axisX, ANALOG_MIN_VALUE, ANALOG_MAX_VALUE, XBOX_JTK_MIN, XBOX_JTK_MAX);
          xbox_in.leftStickY = map(axisY, ANALOG_MIN_VALUE, ANALOG_MAX_VALUE, XBOX_JTK_MAX, XBOX_JTK_MIN);

	        psx.getRightAnalog(axisX, axisY);
          xbox_in.rightStickX = map(axisX, ANALOG_MIN_VALUE, ANALOG_MAX_VALUE, XBOX_JTK_MIN, XBOX_JTK_MAX);
          xbox_in.rightStickY = map(axisY, ANALOG_MIN_VALUE, ANALOG_MAX_VALUE, XBOX_JTK_MAX, XBOX_JTK_MIN);
        }
        else
        {
          xbox_in.leftStickX = 0;
          xbox_in.leftStickY = 0;
          xbox_in.rightStickX = 0;
          xbox_in.rightStickY = 0;
        }

        memset((void*)&xbox_out, 0, sizeof(xbox_out));
        usbd_xid.sendReport(&xbox_in, sizeof(usbd_duke_in_t));
        usbd_xid.getReport(&xbox_out, sizeof(usbd_duke_out_t));
		  }
	  }
  }

}
