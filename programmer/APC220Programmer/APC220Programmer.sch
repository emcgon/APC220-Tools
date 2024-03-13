EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Device:Battery_Cell BT1
U 1 1 65D49170
P 1600 1800
F 0 "BT1" H 1718 1896 50  0000 L CNN
F 1 "9V PP3" H 1718 1805 50  0000 L CNN
F 2 "" V 1600 1860 50  0001 C CNN
F 3 "~" V 1600 1860 50  0001 C CNN
	1    1600 1800
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_SPST SW1
U 1 1 65D4993D
P 2600 1400
F 0 "SW1" H 2600 1635 50  0000 C CNN
F 1 "SW_SPST" H 2600 1544 50  0000 C CNN
F 2 "" H 2600 1400 50  0001 C CNN
F 3 "~" H 2600 1400 50  0001 C CNN
	1    2600 1400
	1    0    0    -1  
$EndComp
$Comp
L Device:R R1
U 1 1 65D4A98E
P 2250 1550
F 0 "R1" H 2320 1596 50  0000 L CNN
F 1 "10K" H 2320 1505 50  0000 L CNN
F 2 "" V 2180 1550 50  0001 C CNN
F 3 "~" H 2250 1550 50  0001 C CNN
	1    2250 1550
	1    0    0    -1  
$EndComp
$Comp
L Device:R R2
U 1 1 65D4B0A8
P 2250 2050
F 0 "R2" H 2320 2096 50  0000 L CNN
F 1 "1K" H 2320 2005 50  0000 L CNN
F 2 "" V 2180 2050 50  0001 C CNN
F 3 "~" H 2250 2050 50  0001 C CNN
	1    2250 2050
	1    0    0    -1  
$EndComp
$Comp
L Regulator_Linear:LM7805_TO220 U1
U 1 1 65D4B5D9
P 3350 1400
F 0 "U1" H 3350 1642 50  0000 C CNN
F 1 "LM7805_TO220" H 3350 1551 50  0000 C CNN
F 2 "Package_TO_SOT_THT:TO-220-3_Vertical" H 3350 1625 50  0001 C CIN
F 3 "https://www.onsemi.cn/PowerSolutions/document/MC7800-D.PDF" H 3350 1350 50  0001 C CNN
	1    3350 1400
	1    0    0    -1  
$EndComp
$Comp
L Device:C C1
U 1 1 65D4C795
P 2900 2050
F 0 "C1" H 3015 2096 50  0000 L CNN
F 1 "100n" H 3015 2005 50  0000 L CNN
F 2 "" H 2938 1900 50  0001 C CNN
F 3 "~" H 2900 2050 50  0001 C CNN
	1    2900 2050
	1    0    0    -1  
$EndComp
$Comp
L Device:CP C2
U 1 1 65D4E45F
P 3800 2050
F 0 "C2" H 3918 2096 50  0000 L CNN
F 1 "CP" H 3918 2005 50  0000 L CNN
F 2 "" H 3838 1900 50  0001 C CNN
F 3 "~" H 3800 2050 50  0001 C CNN
	1    3800 2050
	1    0    0    -1  
$EndComp
Wire Wire Line
	1600 1600 1600 1400
Wire Wire Line
	1600 1400 2250 1400
Wire Wire Line
	2400 1400 2250 1400
Connection ~ 2250 1400
Wire Wire Line
	2250 1700 2250 1900
Wire Wire Line
	2900 2200 2250 2200
Wire Wire Line
	3800 2200 3350 2200
Connection ~ 2900 2200
Wire Wire Line
	2250 2200 2000 2200
Wire Wire Line
	1600 2200 1600 1900
Connection ~ 2250 2200
Wire Wire Line
	2800 1400 2900 1400
Wire Wire Line
	2900 1400 2900 1900
Connection ~ 2900 1400
Wire Wire Line
	2900 1400 3050 1400
Wire Wire Line
	3650 1400 3800 1400
Wire Wire Line
	3800 1400 3800 1900
Wire Wire Line
	3350 1700 3350 2200
Connection ~ 3350 2200
Wire Wire Line
	3350 2200 2900 2200
$Comp
L Device:Rotary_Encoder_Switch SW2
U 1 1 65D4FFC0
P 6650 4650
F 0 "SW2" H 6650 5017 50  0000 C CNN
F 1 "Rotary_Encoder_Switch" H 6650 4926 50  0000 C CNN
F 2 "" H 6500 4810 50  0001 C CNN
F 3 "~" H 6650 4910 50  0001 C CNN
	1    6650 4650
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR04
U 1 1 65D53709
P 4100 1250
F 0 "#PWR04" H 4100 1100 50  0001 C CNN
F 1 "+5V" H 4115 1423 50  0000 C CNN
F 2 "" H 4100 1250 50  0001 C CNN
F 3 "" H 4100 1250 50  0001 C CNN
	1    4100 1250
	1    0    0    -1  
$EndComp
Wire Wire Line
	3800 1400 4100 1400
Wire Wire Line
	4100 1400 4100 1250
Connection ~ 3800 1400
$Comp
L power:GND #PWR01
U 1 1 65D54627
P 2000 2200
F 0 "#PWR01" H 2000 1950 50  0001 C CNN
F 1 "GND" H 2005 2027 50  0000 C CNN
F 2 "" H 2000 2200 50  0001 C CNN
F 3 "" H 2000 2200 50  0001 C CNN
	1    2000 2200
	1    0    0    -1  
$EndComp
Connection ~ 2000 2200
Wire Wire Line
	2000 2200 1600 2200
$Comp
L Display_Character:WC1602A DS1
U 1 1 65D5497D
P 2750 5150
F 0 "DS1" H 2750 6131 50  0000 C CNN
F 1 "WC1602A" H 2750 6040 50  0000 C CNN
F 2 "Display:WC1602A" H 2750 4250 50  0001 C CIN
F 3 "http://www.wincomlcd.com/pdf/WC1602A-SFYLYHTC06.pdf" H 3450 5150 50  0001 C CNN
	1    2750 5150
	1    0    0    -1  
$EndComp
Wire Wire Line
	3150 4850 4150 4850
$Comp
L Transistor_FET:2N7000 Q1
U 1 1 65D5771A
P 4050 5400
F 0 "Q1" H 4254 5446 50  0000 L CNN
F 1 "2N7000" H 4254 5355 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-92_Inline" H 4250 5325 50  0001 L CIN
F 3 "https://www.onsemi.com/pub/Collateral/NDS7002A-D.PDF" H 4050 5400 50  0001 L CNN
	1    4050 5400
	1    0    0    -1  
$EndComp
Wire Wire Line
	3150 4950 4150 4950
Wire Wire Line
	4150 4950 4150 5200
Wire Wire Line
	2750 5950 3550 5950
Wire Wire Line
	4150 5950 4150 5600
$Comp
L power:+5V #PWR05
U 1 1 65D567C3
P 4150 4000
F 0 "#PWR05" H 4150 3850 50  0001 C CNN
F 1 "+5V" H 4165 4173 50  0000 C CNN
F 2 "" H 4150 4000 50  0001 C CNN
F 3 "" H 4150 4000 50  0001 C CNN
	1    4150 4000
	1    0    0    -1  
$EndComp
Wire Wire Line
	4150 4000 4150 4100
$Comp
L power:GND #PWR03
U 1 1 65D5D12B
P 3550 6050
F 0 "#PWR03" H 3550 5800 50  0001 C CNN
F 1 "GND" H 3555 5877 50  0000 C CNN
F 2 "" H 3550 6050 50  0001 C CNN
F 3 "" H 3550 6050 50  0001 C CNN
	1    3550 6050
	1    0    0    -1  
$EndComp
Wire Wire Line
	3550 6050 3550 5950
Connection ~ 3550 5950
Wire Wire Line
	3550 5950 4150 5950
Wire Notes Line
	1400 3550 4700 3550
Wire Notes Line
	4700 3550 4700 6350
Wire Notes Line
	4700 6350 1400 6350
Wire Notes Line
	1400 6350 1400 3550
Text Notes 1450 3700 0    50   ~ 0
Display (16x2 LCD)
$Comp
L Device:R_POT_TRIM RV1
U 1 1 65D65BBD
P 3500 4400
F 0 "RV1" H 3430 4354 50  0000 R CNN
F 1 "R_POT_TRIM" H 3430 4445 50  0000 R CNN
F 2 "" H 3500 4400 50  0001 C CNN
F 3 "~" H 3500 4400 50  0001 C CNN
	1    3500 4400
	-1   0    0    1   
$EndComp
Wire Wire Line
	3500 4250 3500 4100
Wire Wire Line
	3500 4100 4150 4100
Connection ~ 4150 4100
Wire Wire Line
	4150 4100 4150 4850
Wire Wire Line
	3150 4550 3150 4400
Wire Wire Line
	3150 4400 3350 4400
$Comp
L power:GND #PWR02
U 1 1 65D6A206
P 3500 4600
F 0 "#PWR02" H 3500 4350 50  0001 C CNN
F 1 "GND" H 3505 4427 50  0000 C CNN
F 2 "" H 3500 4600 50  0001 C CNN
F 3 "" H 3500 4600 50  0001 C CNN
	1    3500 4600
	1    0    0    -1  
$EndComp
Wire Wire Line
	3500 4600 3500 4550
Wire Notes Line
	1350 2650 1350 750 
Wire Notes Line
	1350 750  4650 750 
Wire Notes Line
	4650 750  4650 2650
Wire Notes Line
	4650 2650 1350 2650
Text Notes 1400 900  0    50   ~ 0
Power
$Comp
L j_ProcessorBoards:ProMini uP1
U 1 1 65D6E61D
P 7700 950
F 0 "uP1" H 7100 747 60  0000 C CNN
F 1 "Arduino Pro Mini" H 7100 641 60  0000 C CNN
F 2 "" H 7100 -1350 60  0000 C CNN
F 3 "" H 7100 -1350 60  0000 C CNN
	1    7700 950 
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x07_Female J?
U 1 1 65D71F67
P 8850 1700
F 0 "J?" H 9050 1800 50  0000 L CNN
F 1 "APC220 Socket" H 9050 1700 50  0000 L CNN
F 2 "" H 8850 1700 50  0001 C CNN
F 3 "~" H 8850 1700 50  0001 C CNN
	1    8850 1700
	1    0    0    -1  
$EndComp
Text Notes 8850 1400 0    50   ~ 0
SET
Text Notes 8850 1500 0    50   ~ 0
AUX
Text Notes 8850 1600 0    50   ~ 0
TXD
Text Notes 8850 1700 0    50   ~ 0
RXD
Text Notes 8850 1800 0    50   ~ 0
EN
Text Notes 8850 1900 0    50   ~ 0
VCC
Text Notes 8850 2000 0    50   ~ 0
GND
$Comp
L power:GND #PWR?
U 1 1 65D74EFB
P 8650 2100
F 0 "#PWR?" H 8650 1850 50  0001 C CNN
F 1 "GND" H 8655 1927 50  0000 C CNN
F 2 "" H 8650 2100 50  0001 C CNN
F 3 "" H 8650 2100 50  0001 C CNN
	1    8650 2100
	1    0    0    -1  
$EndComp
Wire Wire Line
	8650 2000 8650 2100
Wire Wire Line
	8650 1900 8150 1900
Wire Wire Line
	8150 1900 8150 950 
$Comp
L power:+5V #PWR?
U 1 1 65D76577
P 8150 950
F 0 "#PWR?" H 8150 800 50  0001 C CNN
F 1 "+5V" H 8165 1123 50  0000 C CNN
F 2 "" H 8150 950 50  0001 C CNN
F 3 "" H 8150 950 50  0001 C CNN
	1    8150 950 
	1    0    0    -1  
$EndComp
$EndSCHEMATC
