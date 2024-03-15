# rfmagic
The native "RF Magic" (as it is called) application for the APC220 is _nasty_ (IMHO). This is a reimplementation of that tool in Python. Requires Python version 3 and the "pyserial" module.

## Instructions for Use
```
$ python3 rfmagic.py -h
usage: rfmagic.py [-h] {read,write} ...

positional arguments:
  {read,write}
    read        Read settings from APC220
    write       Write settings to APC220

options:
  -h, --help    show this help message and exit
```

```
$ python3 rfmagic.py read -h
usage: rfmagic.py read [-h] [-v] readserialport

positional arguments:
  readserialport  Serial port

options:
  -h, --help      show this help message and exit
  -v              Verbose output (extra parameters)
```

```
$ python3 rfmagic.py write -h
usage: rfmagic.py write [-h] writeserialport frequency {0,1,2,3,4,5,6,7,8,9} {1200,2400,4800,9600,19200}

positional arguments:
  writeserialport       Serial port
  frequency             Frequency (NNN.N : 418.0 - 455.0 in 200KHz steps)
  {0,1,2,3,4,5,6,7,8,9}
                        RF power
  {1200,2400,4800,9600,19200}
                        Baud rate (line and radio)

options:
  -h, --help            show this help message and exit
```
### Reading
Linux example:
```
$ sudo python3 rfmagic.py read /dev/ttyUSB0
Detected APC220 radio.  Reading current settings...
Read():
Frequency        = 450.200
Radio Baud Rate  = 9600
RF Power         = 9
Serial Baud Rate = 9600
Serial Parity    = None

done!
```
*(sudo only required because my user account doesn't have access to /dev/ttyUSB0. A better thing to do would be "sudo chmod 666 /dev/ttyUSB0" first and then the sudo wouldn't be required to run the script)*

"-v" gets you a little additional information (but it's not particularly useful information, which is why it isn't presented by default):-
```
$ sudo python3 rfmagic.py read -v /dev/ttyUSB0
Detected APC220 radio.  Reading current settings...
Read():
Frequency        = 450.200
Radio Baud Rate  = 9600
RF Power         = 9
Serial Baud Rate = 9600
Serial Parity    = None
Network ID       = 0x0001
Node ID          = 0x12345678abcd
Radio ID (fixed) = 0xa6

done!
```

### Writing
Windows example:
```
C:\>python3 rfmagic.py write com11 442.0 9 19200
Detected APC220 radio.  Reading current settings...
Read():
Frequency        = 450.200
Radio Baud Rate  = 9600
RF Power         = 9
Serial Baud Rate = 9600
Serial Parity    = None

Writing new settings to radio...these are the values returned during the "write" process:
Write(): f=442.000MHz, radio baud=19200, power=9, serial baud=19200, parity=None
Frequency        = 442.000
Radio Baud Rate  = 19200
RF Power         = 9
Serial Baud Rate = 19200
Serial Parity    = None

Reading (hopefully) updated settings from radio...
Read():
Frequency        = 442.000
Radio Baud Rate  = 19200
RF Power         = 9
Serial Baud Rate = 19200
Serial Parity    = None

done!
```
