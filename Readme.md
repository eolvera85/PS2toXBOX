# PS2 to Xbox OG Controller Adapter 
Compatible with boards based on ATMEGA32u4 at 16MHZ 

Features: 
- Native USB on development boards
- Compatible with PS1/PS2 Controllers

Components:
- Board ATMEGA32u4 at 5V/16MHz (Great 3.3v/16MHZ)
- Micro USB to Xbox OG Controller Cable (Depending on the board)
- Regulator 3.3V (Optional)
- Level Shofter 5v/3.3v (Optional)

Notes:
For its correct operation it is necessary that the signals between the board and the controller operate at 3.3v, which is the power supply voltage of the controller, for this the following can be done: (Diagram)
- Remove the diode and add a 3.3v regulator (Based on ProMicro)
- Level Shifter 5v/3.3v

This project is functional with the physical controls of psx/ps2, to combine it with the BlueRetro project it is necessary to modify unofficial functions in BlueRetro (Press Buttons and/or Vibration)

![alt text](Diagram.png?raw=true)

### Pin Connection
| GPIO ATMEGA32u4 | CONTROLLER |
| ----------------- | ----- |
| 2 | PS2_DAT |
| 3 | PS2_CMD |
| 4 | PS2_ATT |
| 5 | PS2_CLK |
| 3.3v | PS2_VCC |
| GND | PS2_GND |

Libraries used:
- [LUFA](https://github.com/Palatis/Arduino-Lufa)
- [PsxNewLib](https://github.com/SukkoPera/PsxNewLib)
- [DigitalIO](https://github.com/greiman/DigitalIO)
- [XBOXPadMicro](https://github.com/bootsector/XBOXPadMicro)