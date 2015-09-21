# rc-trigger-control-arduino
Use an arduino to control a 'tank style' vehicle with separate ESCs for left and right wheels with a trigger-and-wheel style R/C transmitter.

# How to use
This guide assumes you have some basic knowledge of electronics and know how to flash your code onto your arduino.

Wiring Receiver:

R/C receivers have 3 pins per channel. Connect as follows:

One of the +ve wires (usually red) to +5V header on arduino
One of the -ve wires (black) to GND on arduino

Channel 1 is usually steering. Connect the signal wire to the relevant pin on the arduino - see lines 49 - 52 of the firmware to configure which pin to suit your arduino.
Channel 2 is usually the throttle. Connect the signal wire to the relevant pin on the arduino.

Wiring ESC (electronic speed controller):

This firmware assumes you are using a standard R/C type ESC or compatible. If your ESC is designed to work with something other than a regular R/C receiver, it probably won't work with this code.
Connect the left ESC signal wire to the relevant arduino pin.
Same for the right ESC.

If your ESC has an in-built BEC, it can feed power to both the arduino and receiver. Most R/C ESCs have this. Take the GND wires from both ESCs and connect to a GND header on the arduino. To power the arduino and receiver, connect one of the ESC +ve wires to a +5V header. Check to make sure your ESC outputs the correct voltage, otherwise you may need to skip this step and connect power to the barrel connector on the arduino.

Calibrating:

Have a read of the CONFIG section in the firmware file. You will need to put the arduino into serial mode 2 (line 14) in order to see the raw values your receiver puts out. CTRL+Shift+M opens the serial monitor within the Arduino IDE. Use this mode to see the approximate values while your steering and throttle are in neutral positions and also at full limits in either direction (lines 28 - 35).

Don't forget to also get the raw values for when you switch the transmitter off. In order for the arduino to detect when the connection is lost it needs to know what the receiver outputs while in this state - set these values on lines 43-46. [Let me know if you get strange behaviour when doing this - I haven't tested with any other receivers.]

Setting the serial output mode on line 14 to 1 will show what the arduino is outputting (whether it is reversing, going forward, steering left etc). You can use this to check your values are correct and that it responds how it should.

Lines 17 - 19 adjust the neutral position of your ESC and the upper (full throttle) and lower (full reverse) limits respectively. If you find the vehicle moves, or tries to move, while the throttle is neutral, adjust the neutral position to suit. If it won't reach full speed forward or backward, raise or lower the upper and lower limits respectively. The maximum output value is 180 and the minimum is 0.

The steering sensitivity setting is explained in the config and as yet has not been tested at any values other than 1 or 2 (you may encounter unexpected behaviour or it may not work at all)
