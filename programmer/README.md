# APC220 Programmer
An Arduino program and schematic for a hardware programmer for APC220 radios. See a quick demo at https://www.youtube.com/watch?v=wyKSJeXSkzc (the video was intended for a few people involved in organising the local [CanSat](https://esero.ie/cansat/) competition: that's what the references to "launchers" are all about).

**NB: Schematic is a work in progress: I'm not sure it is 100% accurate. I need to reverse-engineer my working unit to verify some details. I will add the Kicad file here when complete.**

<ins>UPDATE:</ins> There is a whole new version on the way. What started out as a quick task to draw up the schematic for the existing unit in Kicad has grown a bit. I have the schematic done and a PCB layout designed, dimensioned to fit into a standard Hammond Manufacturing project box (widely available). I amd currently awaiting delivery of PCBs from the fab. Once I have those and have tested them, I'll post the updated software, the Kicad files and a BoM here (along with some pictures). Stay tuned !

Here are 3D renders of the front and back of the current iteration of the PCB from KiCad:-

![alt text](https://github.com/emcgon/APC220-Tools/blob/main/programmer/APC220Programmer/APC220Programmer-back.jpg?raw=true)
![alt text](https://github.com/emcgon/APC220-Tools/blob/main/programmer/APC220Programmer/APC220Programmer-front.jpg?raw=true)

## Bill of Materials

|Part|Description|Notes|
|----|-----------|-----|
|A2|Arduino_Nano_v3.x|	
|BT1|Pin Header||
|D1|1N4001|	~||
|DS1|WC1602A|16x2 LCD Display with "standard" interface|
|Q1|2N7000 MOSFET||
|R1|10K resistor|1/4W axial|
|R2,R3|1K resistor|1/4W axial|
|RV1|10K trimpot||
|SW1|Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical|
|SW2|Rotary Encoder with Switch|
|U1|Adafruit BMP390 Module|Optional|
|U2|MPXH6115A6U/T1|Optional|
|U3|Socket for APC220: 7x1 pin-header and 2x1 pin-header|
|PCB|
|Case|PCB dimensioned to fit in a Hammond Manufacturing 1591CS box|

