# APC220 Programmer
An Arduino program and schematic for a hardware programmer for APC220 radios. See a quick demo at https://www.youtube.com/watch?v=wyKSJeXSkzc (the video was intended for a few people involved in organising the local [CanSat](https://esero.ie/cansat/) competition: that's what the references to "launchers" are all about).

**NB: Schematic is a work in progress: I'm not sure it is 100% accurate. I need to reverse-engineer my working unit to verify some details. I will add the Kicad file here when complete.**

<ins>UPDATE:</ins> There is a whole new version on the way. What started out as a quick task to draw up the schematic for the existing unit in Kicad has grown a bit. I have the schematic done and a PCB layout designed, dimensioned to fit into a standard Hammond Manufacturing project box (widely available). I am currently awaiting delivery of PCBs from the fab. Once I have those and have tested them, I'll post the updated software, the Kicad files and a final BoM here (along with some pictures). Stay tuned !

Here are 3D renders of the front and back of the current iteration of the PCB from KiCad:-

![alt text](https://github.com/emcgon/APC220-Tools/blob/main/programmer/APC220Programmer/APC220Programmer-back.jpg?raw=true)
![alt text](https://github.com/emcgon/APC220-Tools/blob/main/programmer/APC220Programmer/APC220Programmer-front.jpg?raw=true)

## Bill of Materials

|Part|Description|Notes|
|----|-----------|-----|
|A2|Arduino Nano v3.x|	
|BT1|2x1 2.54mm pin Header (for 9V PP3 battery)|For connection to an off-board battery clip|
|D1|1N4001|For reverse-polarity protection: almost any diode would do|
|DS1|16x2 LCD Display with "standard" Hitachi HD44780 16-pin interface||
|Q1|2N7000 MOSFET|TO92 case|
|R1|10K resistor|6.5mm, 1/4W axial|
|R2,R3|1K resistor|6.5mm, 1/4W axial|
|RV1|10K trimpot|Bourns 3296W vertical|
|SW1|2x1 2.54mm pin Header (off-board SPST power switch)|
|SW2|Rotary Encoder with Switch|ALPS EC11E series or equivalent|
|U1|Adafruit BMP390 Module|Optional...see note below|
|U2|MPXH6115A6U/T1|Optional...see note below|
|U3|Socket for APC220: 7x1 and 2x1 pin-header sockets|
|PCB||
|Case|PCB dimensioned to fit in a Hammond Manufacturing 1591CS box|
|9V PP3 Batter Clip|Connects to BT1 connector listed above|
|Panel-mounted SPST power switch|Connects to SW1 listed above|

U1 and U2 are barometric pressure sensors and are both optional. If you don't want a barometer function, you can omit both. If you _do_ want the barometer function, you only need one or the other. U1 (the Adafruit one) is probably preferable: it is cheaper, easier to obtain and more accurate (and we love Adafruit because they are wonderful people :heart:!)
