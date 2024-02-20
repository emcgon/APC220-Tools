# APC220-Tools
A collection of tools for using and configuring DFRobot APC220 radio modules

## apc220monitor.py
Listens on the specified serial port (presumably with an APC220 radio connected to it) and outputs any data received to the console, also optionally logging it to a file.

What makes this more useful than just using PuTTY or similar is that this script forces the RTS pin high. In the USB-to-APC220 adapter than is ususally used to connect the APC220 radio to a PC, RTS is wired to the EN pin. Forcing RTS high thus enables the radio (which is necessary for it to send or receive anything) without having to resort to hacking on jumper wires.

## apc220send.py 
Reads input from standard input and transmits it via the specified serial port.

Again, this script forces RTS high, enabling the radio without having to hard-wire the EN pin.

## rfmagic
The OG tool for reprogramming APC220 radios is called RF Magic and it is (IMHO) nasty. It is only available on Windows and - in my experience - is very hit-and-miss. This is a reimplementation of RF Magic as a Python command-line tool along with an analysis of the protocol.

## APC220Programmer
Schematic and source-code for a standalone hardware programmer based on an Arduino Pro Mini and a 16x2 LCD screen allowing APC220 radios to be quickly reprogrammed.
