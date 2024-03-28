# APC220 Programmer
An Arduino program and schematic for a hardware programmer for APC220 radios. See a quick demo (of an older prototype) at https://www.youtube.com/watch?v=wyKSJeXSkzc (the video was intended for a few people involved in organising the local [CanSat](https://esero.ie/cansat/) competition: that's what the references to "launchers" are all about).

Here are 3D renders of the front and back of the current iteration of the PCB from KiCad:-

![alt text](https://github.com/emcgon/APC220-Tools/blob/main/programmer/APC220Programmer/APC220Programmer-back.jpg?raw=true)
![alt text](https://github.com/emcgon/APC220-Tools/blob/main/programmer/APC220Programmer/APC220Programmer-front.jpg?raw=true)

## Bill of Materials

|Part|Description|Notes|
|----|-----------|-----|
|A2|Arduino Nano v3.x|	
|BT1|Molex KK254 (P/N 22232021) power connector or  similar _or_ 2x1 2.54mm pin Header|For connection to an off-board 9V PP3 battery clip|
|D1|1N4001|For reverse-polarity protection: almost any diode would do. If you trust yourself not to connect the batter the wrong way around, you could omit this and just fit a wire jumper 😉|
|DS1|16x2 LCD Display with "standard" Hitachi HD44780 16-pin interface||
|Q1|2N7000 MOSFET|TO92 case|
|R1|10K resistor|6.5mm, 1/4W axial|
|R2,R3|1K resistor|6.5mm, 1/4W axial|
|R4|220R resistor|6.5mm, 1/4W axial. Only required if your display doesn't have an on-board current-limiting resistor for the backlight (some do, some don't). Can be replaced with a wire link otherwise.|
|RV1|10K trimpot|Bourns 3296W vertical|
|SW1|2x1 2.54mm pin Header (off-board SPST power switch)|
|SW2|Rotary Encoder with Switch|ALPS EC11E series or equivalent|
|U1|Adafruit BMP390 Module (https://www.adafruit.com/product/4816)|Optional...see note below|
|U2|MPXH6115A6U/T1|Optional...see note below|
|U3|Socket for APC220: 7x1 and 2x1 pin-header sockets|
|PCB||
|Case|PCB dimensioned to fit in a Hammond Manufacturing 1591CS box|
|9V PP3 Batter Clip|Connects to BT1 connector listed above|
|Panel-mounted SPST power switch|Connects to SW1 listed above|

U1 and U2 are barometric pressure sensors and are both optional. If you don't want a barometer function, you can omit both. If you _do_ want the barometer function, you only need one or the other. U1 (the Adafruit one) is probably preferable: it is cheaper, easier to obtain and more accurate (and we love Adafruit because they are wonderful people :heart:!)

## Assembly Tips
- It is a tight fit into a Hammond Manufacturing 1591CS box, but it does fit. In my prototype build, I had to secure the battery to the lid with a Velco cable-tie to fit it in. 
- The PCB is attached to the case by M3 bolts which go through the display and the PCB and the rotary encoder. Use spacers between the display and PCB mounting holes to keep the display parallel to the PCB. Make those spacers large enough so that the front surface of the display PCB is in the same plane as the top surface of the rotary encoder. This should ensure that the entire assembly is securly attached to the case and should leave room under the display so that it doesn't come into contact with the Arduino pins
- Mount the Arduino Nano high enough off the surface of the PCB so that there is only a minimial amount of pin length protruding below. Again, the objective is to ensure that Arduino pins sticking out underneath the display don't touch any part of the display
- When mounting the sockets for the APC220, temporary put an APC220 into the two sockets to ensure perfect alignment
- Mount the display last.  It won't be possible to solder the Arduino in if you have mounted the display first.
