extern "C" 
{
  #include "XBOXPad.h"
}
#include <PsxControllerBitBang.h>

#define XBOX_JTK_MAX            32767
#define XBOX_JTK_MIN            -32767
//#define ENABLE_SERIAL_DEBUG

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

void setup() 
{
  dstart(9600);

  //MCUCR |= (1<<JTD);
  //MCUCR |= (1<<JTD);

  xbox_init(true);
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
        
        if (!psx.enterConfigMode())
        {
          debugln (F("Cannot enter config mode"));
        }
        else
        {
          if (!psx.enableAnalogSticks())
          {
            debugln (F("Cannot enable analog sticks"));
          }

          if (!psx.exitConfigMode())
          {
            debugln (F("Cannot exit config mode"));
					}
        }        

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
        
        if (psx.buttonPressed(PSB_PAD_UP)) gamepad_state.digital_buttons |= XBOX_DPAD_UP;
        if (psx.buttonPressed(PSB_PAD_DOWN)) gamepad_state.digital_buttons |= XBOX_DPAD_DOWN;
        if (psx.buttonPressed(PSB_PAD_LEFT)) gamepad_state.digital_buttons |= XBOX_DPAD_LEFT;
        if (psx.buttonPressed(PSB_PAD_RIGHT)) gamepad_state.digital_buttons |= XBOX_DPAD_RIGHT;
        if (psx.buttonPressed(PSB_START)) gamepad_state.digital_buttons |= XBOX_START;
        if (psx.buttonPressed(PSB_SELECT)) gamepad_state.digital_buttons |= XBOX_BACK;
        if (psx.buttonPressed(PSB_L3)) gamepad_state.digital_buttons |= XBOX_LEFT_STICK;
        if (psx.buttonPressed(PSB_R3)) gamepad_state.digital_buttons |= XBOX_RIGHT_STICK;

        gamepad_state.a = psx.buttonPressed(PSB_CROSS) ? 0xFF : 0x00;
        gamepad_state.b = psx.buttonPressed(PSB_CIRCLE) ? 0xFF : 0x00;
        gamepad_state.x = psx.buttonPressed(PSB_SQUARE) ? 0xFF : 0x00;
        gamepad_state.y = psx.buttonPressed(PSB_TRIANGLE) ? 0xFF : 0x00;
        gamepad_state.black = psx.buttonPressed(PSB_R1) ? 0xFF : 0x00;
        gamepad_state.white = psx.buttonPressed(PSB_L1) ? 0xFF : 0x00;
        gamepad_state.l = psx.buttonPressed(PSB_L2) ? 0xFF : 0x00;
        gamepad_state.r = psx.buttonPressed(PSB_R2) ? 0xFF : 0x00;

	      psx.getLeftAnalog(axisX, axisY);
        gamepad_state.l_x = map(axisX, ANALOG_MIN_VALUE, ANALOG_MAX_VALUE, XBOX_JTK_MIN, XBOX_JTK_MAX);
        gamepad_state.l_y = map(axisY, ANALOG_MIN_VALUE, ANALOG_MAX_VALUE, XBOX_JTK_MAX, XBOX_JTK_MIN);

	      psx.getRightAnalog(axisX, axisY);
        gamepad_state.r_x = map(axisX, ANALOG_MIN_VALUE, ANALOG_MAX_VALUE, XBOX_JTK_MIN, XBOX_JTK_MAX);
        gamepad_state.r_y = map(axisY, ANALOG_MIN_VALUE, ANALOG_MAX_VALUE, XBOX_JTK_MAX, XBOX_JTK_MIN);

        xbox_send_pad_state(); 
		  }
	  }
  }
}

