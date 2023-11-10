extern "C" 
{
  #include "XBOXPad.h"
}
#include <PsxControllerBitBang.h>

#define XBOX_JTK_MAX            32767
#define XBOX_JTK_MIN            -32767

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

boolean haveController = false;
byte axisX, axisY;
bool enableAnalogButton;
bool enableAnalogSticks;
bool enableRumble;
bool firstTime;

void setup() 
{
  pinMode (LED_BUILTIN, OUTPUT);
  dstart(115200);

  firstTime = true;

  xbox_init(true);

  debugln(F("Ready!"));
}

void loop() 
{
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
        xbox_reset_watchdog();

        gamepad_state.digital_buttons = XBOX_NONE;

        if (enableRumble)
        {
          if (firstTime)
            psx.setRumble(true, 0xFF);
          else
            psx.setRumble(true, gamepad_rumble.right_rumble);

            firstTime = false;
        }
        else
        {
          psx.setRumble(false, 0x00);
        }

        if (enableAnalogButton)
        {
          if (psx.getAnalogButton(PSAB_PAD_UP) > 0x00) gamepad_state.digital_buttons |= XBOX_DPAD_UP;
          if (psx.getAnalogButton(PSAB_PAD_DOWN) > 0x00) gamepad_state.digital_buttons |= XBOX_DPAD_DOWN;
          if (psx.getAnalogButton(PSAB_PAD_LEFT) > 0x00) gamepad_state.digital_buttons |= XBOX_DPAD_LEFT;
          if (psx.getAnalogButton(PSAB_PAD_RIGHT) > 0x00) gamepad_state.digital_buttons |= XBOX_DPAD_RIGHT;

          gamepad_state.a = psx.getAnalogButton(PSAB_CROSS) > 0x00 ? 0xFF : 0x00;
          gamepad_state.b = psx.getAnalogButton(PSAB_CIRCLE) > 0x00 ? 0xFF : 0x00;
          gamepad_state.x = psx.getAnalogButton(PSAB_SQUARE) > 0x00 ? 0xFF : 0x00;
          gamepad_state.y = psx.getAnalogButton(PSAB_TRIANGLE) > 0x00 ? 0xFF : 0x00;
          gamepad_state.black = psx.getAnalogButton(PSAB_R1) > 0x00 ? 0xFF : 0x00;
          gamepad_state.white = psx.getAnalogButton(PSAB_L1) > 0x00 ? 0xFF : 0x00;

          gamepad_state.l = psx.getAnalogButton(PSAB_L2);
          gamepad_state.r = psx.getAnalogButton(PSAB_R2);
        }
        else
        {
          if (psx.buttonPressed(PSB_PAD_UP)) gamepad_state.digital_buttons |= XBOX_DPAD_UP;
          if (psx.buttonPressed(PSB_PAD_DOWN)) gamepad_state.digital_buttons |= XBOX_DPAD_DOWN;
          if (psx.buttonPressed(PSB_PAD_LEFT)) gamepad_state.digital_buttons |= XBOX_DPAD_LEFT;
          if (psx.buttonPressed(PSB_PAD_RIGHT)) gamepad_state.digital_buttons |= XBOX_DPAD_RIGHT;

          gamepad_state.a = psx.buttonPressed(PSB_CROSS) ? 0xFF : 0x00;
          gamepad_state.b = psx.buttonPressed(PSB_CIRCLE) ? 0xFF : 0x00;
          gamepad_state.x = psx.buttonPressed(PSB_SQUARE) ? 0xFF : 0x00;
          gamepad_state.y = psx.buttonPressed(PSB_TRIANGLE) ? 0xFF : 0x00;
          gamepad_state.black = psx.buttonPressed(PSB_R1) ? 0xFF : 0x00;
          gamepad_state.white = psx.buttonPressed(PSB_L1) ? 0xFF : 0x00;
          gamepad_state.l = psx.buttonPressed(PSB_L2) ? 0xFF : 0x00;
          gamepad_state.r = psx.buttonPressed(PSB_R2) ? 0xFF : 0x00;
        }

        if (psx.buttonPressed(PSB_START)) gamepad_state.digital_buttons |= XBOX_START;
        if (psx.buttonPressed(PSB_SELECT)) gamepad_state.digital_buttons |= XBOX_BACK;
        if (psx.buttonPressed(PSB_L3)) gamepad_state.digital_buttons |= XBOX_LEFT_STICK;
        if (psx.buttonPressed(PSB_R3)) gamepad_state.digital_buttons |= XBOX_RIGHT_STICK;

        if (enableAnalogSticks)
        {
	        psx.getLeftAnalog(axisX, axisY);
          gamepad_state.l_x = map(axisX, ANALOG_MIN_VALUE, ANALOG_MAX_VALUE, XBOX_JTK_MIN, XBOX_JTK_MAX);
          gamepad_state.l_y = map(axisY, ANALOG_MIN_VALUE, ANALOG_MAX_VALUE, XBOX_JTK_MAX, XBOX_JTK_MIN);

	        psx.getRightAnalog(axisX, axisY);
          gamepad_state.r_x = map(axisX, ANALOG_MIN_VALUE, ANALOG_MAX_VALUE, XBOX_JTK_MIN, XBOX_JTK_MAX);
          gamepad_state.r_y = map(axisY, ANALOG_MIN_VALUE, ANALOG_MAX_VALUE, XBOX_JTK_MAX, XBOX_JTK_MIN);
        }
        else
        {
          gamepad_state.l_x = 0;
          gamepad_state.l_y = 0;
          gamepad_state.r_x = 0;
          gamepad_state.r_y = 0;
        }

        xbox_send_pad_state();
		  }
	  }
  }
}

