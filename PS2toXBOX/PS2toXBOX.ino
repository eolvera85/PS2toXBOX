#include <PsxControllerBitBang.h>
#include <OGXBOX.h>

const byte PIN_PS2_DAT = 2;
const byte PIN_PS2_CMD = 3;
const byte PIN_PS2_ATT = 4;
const byte PIN_PS2_CLK = 5;

const unsigned long POLLING_INTERVAL = 1000U / 50U;
PsxControllerBitBang<PIN_PS2_ATT, PIN_PS2_CMD, PIN_PS2_DAT, PIN_PS2_CLK> psx;

boolean haveController;
byte axisX, axisY;
bool enableAnalogButton;
bool enableAnalogSticks;
bool enableRumble;
bool okRumble;
uint8_t lValue;
uint8_t hValue;
OgXbox ogxbox(DUKE);

void setup() {
  haveController = false;
  okRumble = false;
  ogxbox.begin();
}

void loop() {
  static unsigned long last = 0;

  if (millis() - last >= POLLING_INTERVAL) {
    last = millis();

    if (!haveController) {
		  if (psx.begin()) {
        psx.enterConfigMode();
        
        enableAnalogSticks = psx.enableAnalogSticks();
        enableAnalogButton = psx.enableAnalogButtons();
        enableRumble = psx.enableRumble();

        psx.exitConfigMode();

        haveController = true;
		  }
	  }
    else {
		  if (!psx.read()) {
			  haveController = false;
      }
      else {
        if (enableRumble && okRumble)
          psx.setRumble(lValue > 0x00, hValue);
        else 
          psx.setRumble(false, 0x00);
        
        if (enableAnalogButton) {
          ogxbox.setDpad(psx.getAnalogButton(PSAB_PAD_UP) > 0x00,
            psx.getAnalogButton(PSAB_PAD_DOWN) > 0x00,
            psx.getAnalogButton(PSAB_PAD_LEFT) > 0x00,
            psx.getAnalogButton(PSAB_PAD_RIGHT) > 0x00);

          ogxbox.pressAnalogButton(ANA_BTN_A, psx.getAnalogButton(PSAB_CROSS));
          ogxbox.pressAnalogButton(ANA_BTN_B, psx.getAnalogButton(PSAB_CIRCLE));
          ogxbox.pressAnalogButton(ANA_BTN_X, psx.getAnalogButton(PSAB_SQUARE));
          ogxbox.pressAnalogButton(ANA_BTN_Y, psx.getAnalogButton(PSAB_TRIANGLE));
          ogxbox.pressAnalogButton(ANA_BTN_BLACK, psx.getAnalogButton(PSAB_R1));
          ogxbox.pressAnalogButton(ANA_BTN_WHITE, psx.getAnalogButton(PSAB_L1));

          ogxbox.setLeftTrigger(psx.getAnalogButton(PSAB_L2));
          ogxbox.setRightTrigger(psx.getAnalogButton(PSAB_R2));
        }
        else {
          ogxbox.setDpad(psx.buttonPressed(PSB_PAD_UP),
            psx.buttonPressed(PSB_PAD_DOWN),
            psx.buttonPressed(PSB_PAD_LEFT),
            psx.buttonPressed(PSB_PAD_RIGHT));

          ogxbox.setButton(BUTTON_A, psx.buttonPressed(PSB_CROSS));
          ogxbox.setButton(BUTTON_B, psx.buttonPressed(PSB_CIRCLE));
          ogxbox.setButton(BUTTON_X, psx.buttonPressed(PSB_SQUARE));
          ogxbox.setButton(BUTTON_Y, psx.buttonPressed(PSB_TRIANGLE));
          ogxbox.setButton(BUTTON_BLACK, psx.buttonPressed(PSB_R1));
          ogxbox.setButton(BUTTON_WHITE, psx.buttonPressed(PSB_L1));

          ogxbox.setLeftTrigger(psx.buttonPressed(PSB_L2) ? 0xFF : 0x00);
          ogxbox.setRightTrigger(psx.buttonPressed(PSB_R2) ? 0xFF : 0x00);
        }

        ogxbox.setButton(BUTTON_START, psx.buttonPressed(PSB_START));
        ogxbox.setButton(BUTTON_BACK, psx.buttonPressed(PSB_SELECT));
        ogxbox.setButton(BUTTON_LS, psx.buttonPressed(PSB_L3));
        ogxbox.setButton(BUTTON_RS, psx.buttonPressed(PSB_R3));

        if (enableAnalogSticks) {
	        psx.getLeftAnalog(axisX, axisY);
          ogxbox.setLeftJoystick(map(axisX, ANALOG_MIN_VALUE, ANALOG_MAX_VALUE, DUKE_JOYSTICK_MIN, DUKE_JOYSTICK_MAX), 
            map(axisY, ANALOG_MIN_VALUE, ANALOG_MAX_VALUE, DUKE_JOYSTICK_MAX, DUKE_JOYSTICK_MIN));

	        psx.getRightAnalog(axisX, axisY);
          ogxbox.setRightJoystick(map(axisX, ANALOG_MIN_VALUE, ANALOG_MAX_VALUE, DUKE_JOYSTICK_MIN, DUKE_JOYSTICK_MAX), 
            map(axisY, ANALOG_MIN_VALUE, ANALOG_MAX_VALUE, DUKE_JOYSTICK_MAX, DUKE_JOYSTICK_MIN));
        }
        else {
          ogxbox.setLeftJoystick(0, 0);
          ogxbox.setRightJoystick(0, 0);
        }

        ogxbox.sendReport();
        okRumble = ogxbox.getRumble(lValue, hValue);
		  }
	  }
  }
}
