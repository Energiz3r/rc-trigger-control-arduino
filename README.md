# rc-trigger-control-arduino
Use an arduino to control a 'tank style' vehicle with separate ESCs for left and right wheels with a trigger-and-wheel style R/C transmitter.

# How to build

1. Download the Arduino IDE from https://www.arduino.cc/en/Main/Software
2. Open the main source file remote/remote.ino in the IDE
3. Click the 'tick' (verify) button in the IDE to compile
4. Click the 'right arrow' (upload) button in the IDE to upload to your device

# How to use
This guide assumes you have some basic knowledge of electronics and know how to flash your code onto your arduino.

Wiring Receiver:

R/C receivers have 3 pins per channel. Connect as follows:

One of the +ve wires (usually red) to +5V header on arduino
One of the -ve wires (black) to GND on arduino

Channel 1 is usually steering. Connect the signal wire to the relevant pin on the arduino - see lines 49 - 52 of the firmware to configure which pin to suit your arduino.
Channel 2 is usually the throttle. Connect the signal wire to the relevant pin on the arduino.

Wiring ESC (electronic speed controller):

This firmware is modified for the CanaKit UK1122 2amp motor controller. See the documentation for info on how to wire it up corresponding to the code in the arduino file.

Calibrating:

Have a read of the CONFIG section in the firmware file. You will need to put the arduino into serial mode 2 (line 14) in order to see the raw values your receiver puts out. CTRL+Shift+M opens the serial monitor within the Arduino IDE. Use this mode to see the approximate values while your steering and throttle are in neutral positions and also at full limits in either direction (lines 28 - 35).

Don't forget to also get the raw values for when you switch the transmitter off. In order for the arduino to detect when the connection is lost it needs to know what the receiver outputs while in this state - set these values on lines 43-46. [Let me know if you get strange behaviour when doing this - I haven't tested with any other receivers.]

Setting the serial output mode on line 14 to 1 will show what the arduino is outputting (whether it is reversing, going forward, steering left etc). You can use this to check your values are correct and that it responds how it should.
