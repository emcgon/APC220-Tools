# APC220-Tools
A collection of tools for using and configuring DFRobot APC220 radio modules

## apc220monitor.py
Listens on the specified serial port (presumably with an APC220 radio connected to it) and outputs any data received to the console, also optionally logging it to a file.

What makes this more useful than just using PuTTY or similar is that this script forces the RTS pin high. In the USB-to-APC220 adapter than is ususally used to connect the APC220 radio to a PC, RTS is wired to the EN pin. Forcing RTS high thus enables the radio (which is necessary for it to send or receive anything) without having to resort to hacking on jumper wires.

### Instructions for use
```
$ python3 apc220monitor.py  -h
usage: apc220monitor.py [-h] comPort {2400,4800,9600,19200,38400} outFile

Monitor data from a serial port (an APC220 radio connected via a USB adapter). Displays the output to the console and optionally writes it to a file.

positional arguments:
  comPort               "COMx" should be the COM port that the APC220 radio is connected to (typically COM2, but check this in Device Manager to be sure)
  {2400,4800,9600,19200,38400}
                        "SPEED" is the baud rate. It is typically 9600 unless you have set your APC220 radio to something different.
  outFile               "output-file" is the name of the file to save output to. You can specify a value of "nul" here if you don't want data saved to a file.

options:
  -h, --help            show this help message and exit
```

#### Windows:
```
C:\APC220-Tools>python3 apc220monitor.py com11 9600 a.out
Logging received data to a.out
Hello from an APC220 connected to a Linux machine!
Bye!

C:\APC220-Tools>type a.out
================================================================================
Logging started at 2024-02-21 22:16:38.862246
================================================================================
Hello from an APC220 connected to a Linux machine!
```

#### Linux:
```
$ sudo python3 apc220monitor.py  /dev/ttyUSB0 9600 a.out
Logging received data to a.out
hello, world!
^CBye!
$ cat a.out 
================================================================================
Logging started at 2024-02-21 22:14:43.543053
================================================================================
hello, world!
$
```

## apc220send.py 
Reads input from standard input and transmits it via the specified serial port.

Again, this script forces RTS high, enabling the radio without having to hard-wire the EN pin.

### Instructions for use
```
$ python3 apc220send.py  -h
usage: apc220send.py [-h] [-v] comPort {2400,4800,9600,19200,38400}

Read data from standard input and transmit it via an APC220 radio connected to the specified serial port.

positional arguments:
  comPort               "COMx" should be the COM port that the APC220 radio is connected to (typically COM2, but check this in Device Manager to be sure)
  {2400,4800,9600,19200,38400}
                        "SPEED" is the baud rate. It is typically 9600 unless you have set your APC220 radio to something different.

optional arguments:
  -h, --help            show this help message and exit
  -v, --verbose         Produce verbose output
```

#### Linux:
```
$ sudo python3 apc220send.py /dev/ttyUSB0 9600
Reading from standard input: end with CTRL-D on a line on its own
hello, world!
14
```
If you are typing the data to be sent (rather than piping it in from a file or another program), press CTRL-D to terminate.  The number that is displayed is the number of bytes read and transmitted.

#### Windows:
```
C:\APC220-Tools>python3 apc220send.py com11 9600
Reading from standard input: end with CTRL-Z on a line on its own
hello, world!
^Z
14
```
If you are typing the data to be sent (rather than piping it in from a file or another program), press CTRL-Z to terminate.  The number that is displayed is the number of bytes read and transmitted.

Alternatively...
```
C:\APC220-Tools>echo Mary had a little lamb | python3 apc220send.py com11 9600
24
```
...does exactly what you would expect. At the receiving end...
```
$ sudo python3 apc220monitor.py  /dev/ttyUSB0 9600 a.out
Logging received data to a.out
Mary had a little lamb 
^CBye!

$ cat a.out 
================================================================================
Logging started at 2024-02-21 22:26:13.318536
================================================================================
Mary had a little lamb 
```
Interactive-mode (-i, --interactive) reads continuously from standard input and transmits one line at a time. This may be more

## rfmagic
The OG tool for reprogramming APC220 radios is called RF Magic and it is (IMHO) nasty. It is only available on Windows and - in my experience - is very hit-and-miss. This is a reimplementation of RF Magic as a Python command-line tool along with an analysis of the protocol.

## APC220Programmer
Schematic and source-code for a standalone hardware programmer based on an Arduino Nano and a 16x2 LCD screen allowing APC220 radios to be quickly reprogrammed without the need for a laptop or a USB adapter.

