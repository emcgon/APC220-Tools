#include <Wire.h>
#include <LiquidCrystal.h>
#include <Encoder.h>

#define NODEMO

// #define DEBUG

// Pin assignments for APC220
#define APC220_EN_PIN     5
// Pin assignments for LCD
#define LCD_BACKLIGHT_PIN 6    // PWM-capable on an Atmega 328 !!
#define LCD_ENABLE_PIN    7
#define LCD_D4_PIN        8
#define LCD_D5_PIN        9
#define LCD_D6_PIN        10
#define LCD_D7_PIN        11
#define LCD_RS_PIN        12

// Pin assignements for rotary encoder
#define ROTARY_A_PIN       2    // Interrupt 0
#define ROTARY_A_INTERRUPT 0
#define ROTARY_B_PIN       3    // Interrupt 1
#define ROTARY_B_INTERRUPT 1
#define ROTARY_BUTTON_PIN  4

// Battery-sense pin.  10K:1K resistor-divider from battery with junction going to this pin
#define BATTERY_SENSE_PIN  A2

// Various timer values
#define DETECT_TIMEOUT     10000  // How long to continue trying to detect a radio before giving up
#define READ_TIMEOUT       3000   // How long to continue trying to read settings from a radio before giving up
#define DEBOUNCE_TIME      20
#define BACKLIGHT_TIMEOUT  20000  // How long the backlight stays on for
#define ROTARY_SWITCH_INDENTS    4     // How many steps are advanced in each indent on the rotary switch 



struct APC220data
{
    unsigned long frequency;    // Frequency in KHz
    unsigned char serialBaud;
    unsigned char radioBaud;
    unsigned char rfPower;
    unsigned char serialParity;
};

unsigned short int baudRateTable[] = 
{
    1200,    // 0
    2400,    // 1
    4800,    // 2
    9600,    // 3
    19200,   // 4
    38400,   // 5
    57600    // 6
};


/*
** Global variables
*/
LiquidCrystal lcd(LCD_RS_PIN, LCD_ENABLE_PIN, LCD_D4_PIN, LCD_D5_PIN, LCD_D6_PIN, LCD_D7_PIN);
Encoder rotary(ROTARY_B_PIN, ROTARY_A_PIN);
int currentSerialBaudRate=-1;
unsigned long backlightOffTime=0;

#ifdef DEMO
struct APC220data demoRadio;
#endif


void setup() 
{
    // Initialise LCD
    pinMode(LCD_BACKLIGHT_PIN,OUTPUT);
    BacklightOn();
    lcd.begin(16, 2);
    lcd.setCursor(0,0); lcd.print("==APC220 Magic==");
    lcd.setCursor(0,1);
    lcd.print("Eamonn McGonigle");

    // Set up rotary encoder
    pinMode(ROTARY_BUTTON_PIN, INPUT);
    digitalWrite(ROTARY_BUTTON_PIN, HIGH);
        
    // Set up serial interface to APC220 radio
    pinMode(APC220_EN_PIN,OUTPUT);
    digitalWrite(APC220_EN_PIN,LOW);

    byte smiley[] = {
        B00000,
        B10001,
        B00000,
        B00100,
        B00100,
        B00000,
        B10001,
        B01110,
    };
    lcd.createChar(1,smiley);

    byte frowney[] = {
        B00000,
        B10001,
        B00000,
        B00100,
        B00100,
        B00000,
        B01110,
        B10001,
    };
    lcd.createChar(2,frowney);
       
#ifdef DEMO
    // Initialse the demo radio
    demoRadio.frequency = 420000;
    demoRadio.serialBaud = 5;
    demoRadio.radioBaud = 3;
    demoRadio.rfPower = '7';
    demoRadio.serialParity = '1';
#endif

    // Set up battery-sense pin for input and use internal 1.1V reference
    pinMode(A3,INPUT);
    analogReference(INTERNAL);

#ifdef DEBUG
    Serial.begin(115200);
    Serial.println("Debug Mode");
#endif
    // Pause for dramatic effect (also to allow A/D converter to settle to new reference, although it doesn't need this long)
    delay(0618);
    
    // Display battery voltage 
    BatteryVoltage();
    delay(0618);
}



void loop()
{
    static char *mainMenuItems[] = 
    {
        "Read",
        "Write",
        "Ping",
        "Monitor"
    };
    int lastRotaryPosition=0;
    static byte currentMenuItem=0;
    
    // Display main menu items
    lcd.clear();
    lcd.setCursor(1,0); lcd.print(mainMenuItems[0]); 
    lcd.setCursor(16-(strlen(mainMenuItems[1])+1),0); lcd.print(mainMenuItems[1]);    
    lcd.setCursor(1,1); lcd.print(mainMenuItems[2]);
    lcd.setCursor(16-(strlen(mainMenuItems[3])+1),1); lcd.print(mainMenuItems[3]);
    
    // Wait for an event. An event is either the rotary encoder moving 
    // by ROTARY_SWITCH_INDENTS or the button being pressed
    lastRotaryPosition = rotary.read();
    while(1)
    {
                
        // Draw brackets around current menu item
        switch(currentMenuItem)
        {
            case 0:
            lcd.setCursor(0,0); lcd.print('[');
            lcd.setCursor(strlen(mainMenuItems[0])+1,0); lcd.write(']');
            break;

            case 1:
            lcd.setCursor(15,0); lcd.write(']');
            lcd.setCursor(15-(strlen(mainMenuItems[1])+1),0); lcd.write('[');
            break;
                    
            case 2:
            lcd.setCursor(0,1); lcd.write('[');
            lcd.setCursor(strlen(mainMenuItems[2])+1,1); lcd.write(']');
            break;

            case 3:
            lcd.setCursor(15,1); lcd.write(']');
            lcd.setCursor(15-(strlen(mainMenuItems[3])+1),1); lcd.write('[');
            break;
        }

        // See if the rotary encoder has moved
        int rotaryPosition = rotary.read();
        if (abs(rotaryPosition-lastRotaryPosition) >= ROTARY_SWITCH_INDENTS)
        {
            int lastMenuItem = currentMenuItem;
            if ((rotaryPosition < lastRotaryPosition) && (currentMenuItem > 0))
            {
                currentMenuItem--;
            }
            else if ((rotaryPosition > lastRotaryPosition) && (currentMenuItem < 3))
            {
                currentMenuItem++;
            }
            if (currentMenuItem != lastMenuItem)
            {
                // Erase brackets around last
                switch(lastMenuItem)
                {
                    case 0:
                    lcd.setCursor(0,0); lcd.write(' ');
                    lcd.setCursor(strlen(mainMenuItems[0])+1,0); lcd.write(' ');
                    break;

                    case 1:
                    lcd.setCursor(15,0); lcd.write(' ');
                    lcd.setCursor(15-(strlen(mainMenuItems[1])+1),0); lcd.write(' ');
                    break;
                    
                    case 2:
                    lcd.setCursor(0,1); lcd.write(' ');
                    lcd.setCursor(strlen(mainMenuItems[2])+1,1); lcd.write(' ');
                    break;

                    case 3:
                    lcd.setCursor(15,1); lcd.write(' ');
                    lcd.setCursor(15-(strlen(mainMenuItems[3])+1),1); lcd.write(' ');
                    break;
                }
            }
            lastRotaryPosition = rotaryPosition;
            BacklightOn();
        }
        
        // If the button is pressed, follow the currently-selected menu item
        if (DebounceRotaryButton())
        {
            BacklightOn();
            switch(currentMenuItem)
            {
                case 0:
                    Read();
                    break;
                case 1:
                    Write();
                    break;
                case 2:
                    Ping();
                    break;
                case 3:
                    Monitor();
                    break;
            }
            break;
        }
        CheckBacklightTimeout();
    }
}



void Read()
{
    struct APC220data d;

    // Try to detect APC220
    lcd.clear();
    lcd.print("Detecting APC220");
    Serial.begin(115200);
    if (detectAPC220())
    {
        lcd.clear();
        lcd.print("Reading APC220...");
        if (readAPC220(d, false))
        {
            lcd.clear();
                
            // Frequency
            PrintFrequency(d.frequency);
            lcd.print("MHz");
                
            // Power
            lcd.setCursor(11,0);
            lcd.print("Pwr=");
            lcd.write(d.rfPower);

            // Baud
            lcd.setCursor(0,1);
            lcd.print(baudRateTable[d.serialBaud],DEC);
            lcd.write('/');
            lcd.print(baudRateTable[d.radioBaud],DEC);
            
            // Serial Parity
            lcd.setCursor(12,1);
            switch(d.serialParity)
            {
                case '0':
                lcd.print("8N1");
                break;
                
                case '1':
                lcd.print("Odd");
                break;
                
                case '2':
                lcd.print("Even");
                break;
            }
        }
        else
        {
            lcd.setCursor(0,1);
            lcd.print("Failed \002");
        }
    }
    else
    {
        lcd.setCursor(0,1);
        lcd.print("Not found \002");
    }
    Serial.end();
    digitalWrite(APC220_EN_PIN,LOW);
    WaitForClick();
}



void Write()
{
    struct APC220data d;
    int currentMenuItem = 0, scrollY = 0;
    int lastRotaryPosition;
    boolean redraw = 1;
    static char *menuItems[]={ "Freq", "Speed", "Power", "Write", "Cancel" };
    
    lcd.clear();
    lcd.println("Detecting APC220");
    Serial.begin(115200);
    if (detectAPC220())
    {
        lcd.clear();
        lcd.print("Reading APC220...");
        if (readAPC220(d,false))
        {

            lcd.clear();
            // Wait for a rotary encoder event
            lastRotaryPosition = rotary.read();
            while(1)
            {
                int rotaryPosition = rotary.read();
                if (abs(rotaryPosition-lastRotaryPosition) >= ROTARY_SWITCH_INDENTS)
                {
                    BacklightOn();
                    // Update current menu item
                    if ((rotaryPosition < lastRotaryPosition) && (currentMenuItem > 0))
                    {
                        currentMenuItem--;
                        if (currentMenuItem - scrollY < 0) --scrollY;
                    }
                    else if ((rotaryPosition > lastRotaryPosition) && (currentMenuItem < 4))
                    {
                        currentMenuItem++;
                        if (currentMenuItem - scrollY > 1) ++scrollY;
                    }
                    lastRotaryPosition = rotary.read();
                    redraw = 1;
                }
                if (redraw)
                {
                    // Redraw the menu
                    lcd.clear();
                    for (int i=0 ; i <= 1 ; ++i)
                    {
                        lcd.setCursor(1,i); lcd.print(menuItems[scrollY+i]);
                        lcd.setCursor(8,i);
                        switch(scrollY+i)
                        {
                            case 0:
                            PrintFrequency(d.frequency);
                            break;
                        
                            case 1:
                            lcd.print(baudRateTable[d.radioBaud],DEC);
                            break;
    
                            case 2:
                            lcd.write(d.rfPower);
                            break;
                        }
                    }
                    lcd.setCursor(0,currentMenuItem - scrollY); lcd.write('[');
                    lcd.setCursor(strlen(menuItems[currentMenuItem])+1, currentMenuItem - scrollY); lcd.write(']');
                    redraw=0;
                }
                
                if (DebounceRotaryButton())
                {
                    BacklightOn();
                    lcd.setCursor(0,currentMenuItem - scrollY); lcd.write(' ');
                    lcd.setCursor(strlen(menuItems[currentMenuItem])+1, currentMenuItem - scrollY); lcd.write(' ');
                    switch(currentMenuItem)
                    {
                        case 0:    // Frequency
                        EditFrequency(8,currentMenuItem-scrollY,d);
                        redraw=1;
                        break;
                        
                        case 1:    // Speed
                        EditBaud(8,currentMenuItem-scrollY,d);
                        redraw=1;
                        break;
                        
                        case 2:    // RF Power
                        EditRFPower(8,currentMenuItem-scrollY,d);
                        redraw=1;
                        break;
                        
                        case 3:    // Write
                        lcd.clear();
                        lcd.print("Writing.");
                        lcd.setCursor(0,1);
                        d.serialParity = '0';    // TODO: For the moment, only "None" is supported
                        d.serialBaud = d.radioBaud; // TODO: Don't support these being different
                        if (writeAPC220(d))
                        {
                            lcd.print("Success \001");
                        }
                        else
                        {
                            lcd.print("Failed \002");
                        }
                        WaitForClick();
                        return;
                        
                        case 4:    // Cancel
                        return;
                    }
                    lastRotaryPosition = rotary.read();    // Ignore movements that took place during editing
                }
                CheckBacklightTimeout();
            }
        }
        else
        {
            lcd.clear();
            lcd.print("Error reading");
            lcd.setCursor(0,1); lcd.println("from APC220 \002");
            WaitForClick();
        }
    }
    else
    {
        lcd.setCursor(0,1);
        lcd.print("Not found \002");
        WaitForClick();
    }
    Serial.end();
    digitalWrite(APC220_EN_PIN,LOW);
}


void EditFrequency(int x, int y, struct APC220data &d)
{
    unsigned int mhz = d.frequency / 1000;
    unsigned int khz = d.frequency % 1000;
    mhz = EditUnsignedInteger(x, y, 3, mhz, 418, 450, 1);
    lcd.setCursor(x-1,y); lcd.write(' ');
    lcd.setCursor(x,y); PrintFrequency(((unsigned long)mhz)*1000L+(unsigned long)khz);
    khz = EditUnsignedInteger(x+4, y, 3, khz, 0, 800, 200);
    d.frequency = ((unsigned long)mhz)*1000L+(unsigned long)khz;
    lcd.setCursor(x,y); PrintFrequency(d.frequency);
}


unsigned int EditUnsignedInteger(int x, int y, int len, unsigned int i, unsigned int lowerLimit, unsigned int upperLimit, unsigned int stepSize)
{
    int lastRotaryPosition = rotary.read();
    
    // Draw the brackets
    lcd.setCursor(x-1,y); lcd.write('[');
    lcd.setCursor(x+len,y); lcd.write(']');    

    while(1)
    {
        // Has the rotary encoder moved ?
        int rotaryPosition = rotary.read();
        if (abs(rotaryPosition-lastRotaryPosition) >= ROTARY_SWITCH_INDENTS)
        {
            BacklightOn();
            if ((rotaryPosition > lastRotaryPosition) && (i < upperLimit))
            {
                i += stepSize;
            }
            else if ((rotaryPosition < lastRotaryPosition) && (i > lowerLimit))
            {
                i -= stepSize;
            }
            lcd.setCursor(x,y);
            PrintInteger(i,len);
            lastRotaryPosition = rotaryPosition;
        }

        // Has the button been clicked ?
        if (DebounceRotaryButton())
        {
            BacklightOn();
            return(i);
        }
        CheckBacklightTimeout();
    }
}



void EditBaud(int x, int y, struct APC220data &d)
{
    int lastRotaryPosition = rotary.read();
    
    // Draw the brackets
    lcd.setCursor(x-1,y); lcd.write('[');
    lcd.setCursor(x+5,y); lcd.write(']');
    while(1)
    {
        int rotaryPosition = rotary.read();
        if (abs(rotaryPosition-lastRotaryPosition) >= ROTARY_SWITCH_INDENTS)
        {
            BacklightOn();
            if ((rotaryPosition > lastRotaryPosition) && (d.radioBaud < 4))
            {
                d.radioBaud++;
                d.serialBaud = d.radioBaud;    // No good reason for these to be different !
            }
            else if ((rotaryPosition < lastRotaryPosition) && (d.radioBaud > 0))
            {
                d.radioBaud--;
                d.serialBaud = d.radioBaud;
            }
            lcd.setCursor(x,y);
            if (d.radioBaud < 4) lcd.write(' ');    // Right-justify
            lcd.print(baudRateTable[d.radioBaud]);
            lastRotaryPosition = rotaryPosition;
        }
        
        if (DebounceRotaryButton())
        {
            BacklightOn();
            return;
        }
        CheckBacklightTimeout();
    }
}



void EditRFPower(int x, int y, struct APC220data &d)
{
    d.rfPower = EditUnsignedInteger(x,y,1,d.rfPower-'0',0,9,1) + '0';
}



void Ping()
{
    lcd.clear();

    if (currentSerialBaudRate < 0)
    {
        lcd.print("Do \"Read\" first!");
        delay(1000);
    }
    else
    {
        digitalWrite(APC220_EN_PIN, HIGH);
        delay(100);
        Serial.begin(baudRateTable[currentSerialBaudRate]);
        Serial.println("Humpty Dumpty sat on a wall, Humpty Dumpty had a great fall");
        Serial.end();
        lcd.print("Message sent OK!");
    }
    lcd.setCursor(0,1); lcd.print("Click to return");
    WaitForClick();
}



void Monitor()
{
    char buffer[17];
    int bufferContents=0;
    
    lcd.clear();

    if (currentSerialBaudRate < 0)
    {
        lcd.print("Do \"Read\" first!");
        lcd.setCursor(0,1); lcd.print("Click to return");
        WaitForClick();
        return;
    }
    lcd.print("Monitoring...");
    lcd.setCursor(0,1);
    digitalWrite(APC220_EN_PIN, HIGH);
    delay(100);
    Serial.begin(baudRateTable[currentSerialBaudRate]);
    while (!DebounceRotaryButton())
    {
        if (Serial.available())
        {
            int a;
            
            BacklightOn();
            a = Serial.read();
            if ((a == '\r') || (a == '\n'))
            {
                a = 255;
            }
            if ((a == 255) || ((a >= 32) && (a <= 127)))
            {
                if (bufferContents < 16)
                {
                    buffer[bufferContents++] = a;
                    buffer[bufferContents] = 0;
                }
                else
                {
                    memmove(buffer,&buffer[1],15);
                    buffer[15] = a;
                }
                lcd.setCursor(0,1); lcd.print(buffer);
            }
#if 0
            else if ((a == '\r') || (a == '\n'))
            {
                // CR or LF resets the buffer
                buffer[bufferContents=0] = 0;
                lcd.setCursor(0,1); for (int i=0 ; i < 16 ; ++i){ lcd.write(' '); }
            }
#endif                
        }
        CheckBacklightTimeout();
    }
    BacklightOn();
    Serial.end();
}



int DebounceRotaryButton()
{
    unsigned long pressedAt = millis();
    
    // Is the pin low ?
    if (digitalRead(ROTARY_BUTTON_PIN) == LOW)
    {
        delay(DEBOUNCE_TIME);
        // Still low ?
        if (digitalRead(ROTARY_BUTTON_PIN) == LOW)
        {
            // Pressed: wait for it to go high before returning
            while (digitalRead(ROTARY_BUTTON_PIN) == LOW)
                ;
            delay(DEBOUNCE_TIME);
           return(millis() - pressedAt);
        }
    }
    return(0);
}



void WaitForClick()
{
    // First, wait for it to be pressed
    while (!DebounceRotaryButton())
        CheckBacklightTimeout();
    BacklightOn();
}



void PrintFrequency(unsigned long f)
{
    PrintInteger(f/1000, 3);
    f = f % 1000;
    lcd.write('.');
    PrintInteger(f, 3);
}


void PrintInteger(int n, int l)
{
    // Compute 10^(l-1)
    int i;
    for (i=1 ; l > 1 ; l--)
        i *= 10;
    
    // Mask the digits with l, MSB->LSB    
    for (; i > 0 ; i /= 10)
    {
        lcd.write('0'+((n/i) % 10));
    }
}



/*
** Detection involves sending 0xAA 0xFF to the serial port while waiting for a response
** of 0xA7. While detection is in progress, the EN pin on the APC220 should
** alternate between high and low with a duty cycle of 0.326ms high/0.156ms low
*/
boolean detectAPC220()
{
    unsigned long next_en_change=0;
    unsigned long give_up_at;
    boolean apc220_en_state=LOW;
    signed char got_consecutive_a7=0;
    
#ifdef DEMO
    delay(250);
    return(1);
#endif
    give_up_at = millis()+DETECT_TIMEOUT;
    while(1)
    {
        if (millis() > next_en_change)
        {
            if (apc220_en_state)
            {
                // Is HIGH - go low for 156ms
                digitalWrite(APC220_EN_PIN, apc220_en_state = LOW);
                next_en_change = millis() + 156;        // TODO: Tune this with oscilloscope
            }
            else
            {
                // Is LOW - go HIGH for 326ms
                digitalWrite(APC220_EN_PIN, apc220_en_state = HIGH);
                next_en_change = millis() + 326;
            }
        }
        Serial.write(0xAA);
        Serial.write(0xFF);    // Not strictly necessary
        if (Serial.available())
        {
            if (Serial.read() == 0xA7)
            {
                got_consecutive_a7++;
            }
            else
            {
                got_consecutive_a7 = 0;
            }
        }
        if (got_consecutive_a7 >= 3)
        {
            digitalWrite(APC220_EN_PIN, HIGH);
            return(1);
        }
        if (millis() >= give_up_at)
        {
            return(0);
        }
    }
}



boolean readAPC220(struct APC220data& d, boolean skipDetectAndCmd)
{
#ifdef DEMO
    delay(250);
    memcpy(&d, &demoRadio, sizeof(struct APC220data));
    currentSerialBaudRate = d.radioBaud;
    return(1);
#endif
    int n;
    unsigned long give_up_at;
   
    if (skipDetectAndCmd || detectAPC220())
    {
        give_up_at = millis()+READ_TIMEOUT;
        
        // Send "Read" command
        if (!skipDetectAndCmd) Serial.write(0xCC);
        
        // Frequency
        while (Serial.read() != 0x34)    // First byte is always 0x34 (ASCII '4')
            ;
        d.frequency=4;
        for (int i=0 ; i < 5 ; ++i)
        {
            if (!WaitForSerialAvailableUntil(give_up_at))
            {
                return(0);
            }
            n = Serial.read();
            if (!isdigit(n))
            {
                // TODO: Handle error-condition
                return(0);
            }
            d.frequency = d.frequency*10 + (n & 0x0f);
        }

        // Radio baud rate
        if (!WaitForSerialAvailableUntil(give_up_at))
        {
            return(0);
        }
        n = Serial.read() & 0x0f;
        if ((n < 0) || (n > 4))
        {
            // TODO: handle error condition
            return(0);
        }
        currentSerialBaudRate = d.radioBaud = n;
        
        // RF power
        if (!WaitForSerialAvailableUntil(give_up_at))
        {
            return(0);
        }
        n = Serial.read();
        if (!isdigit(n))
        {
            // TODO: handle error condition
            return(0);
        }
        d.rfPower = n;
        
        // Serial baud rate
        if (!WaitForSerialAvailableUntil(give_up_at))
        {
            return(0);
        }
        n = Serial.read() & 0x0f;
        if ((n < 0) || (n > 6))
        {
            // TODO: handle error condition
            return(0);
        }
        d.serialBaud = n;
        
        // Serial parity
        if (!WaitForSerialAvailableUntil(give_up_at))
        {
            return(0);
        }
        n = Serial.read();
        if ((n < '0') || (n > '2'))
        {
            // TODO: handle error condition
            return(0);
        }
        d.serialParity = n;

        // 2x0 followed by 2 x Network ID followed by 6 x Node ID followed by 10x0
        for (int i=0 ; i < 20 ; ++i)        
        {
            if (!WaitForSerialAvailableUntil(give_up_at))
            {
                return(0);
            }
            n = Serial.read();
            if ((i >= 10) && (n != 0))
            {
                // TODO: Bytes 20...29 should be 0
                return(0);
            }
        }

        if (!WaitForSerialAvailableUntil(give_up_at))
        {
            return(0);
        }
        Serial.read();    // Per-radio ID value
        if (!WaitForSerialAvailableUntil(give_up_at))
        {
            return(0);
        }
        if (Serial.read() != 0xD2)
        {
            // TODO: handle error condition
            return(0);
        }
        return(1);
    }
    else
    {
        // Failed to detect APC220
        return(0);
    }
}



boolean writeAPC220(struct APC220data& d)
{
    struct APC220data rd;
    
#ifdef DEMO
    delay(500);
    memcpy(&demoRadio, &d, sizeof(struct APC220data));
    currentSerialBaudRate = d.radioBaud;
    return(true);
#endif
    if (detectAPC220())
    {
      // "Write" command
      Serial.write(0xc3);
    
      // Frequency
      for (unsigned long l=100000 ; l > 0 ; l /= 10)
      {
          Serial.write('0' + ((d.frequency/l) % 10));
      }
    
      // Radio baud rate
//      Serial.write('0' + d.radioBaud);
      Serial.write(d.radioBaud);
    
      // RF Power
      Serial.write(d.rfPower);
    
      // Serial baud rate
//      Serial.write('0' + d.radioBaud);
      Serial.write(d.radioBaud);    
      
      // Parity
      Serial.write(d.serialParity);
    
      // 2 x 0
      Serial.write(0); Serial.write(0);
    
      // Network ID
      Serial.write(0); Serial.write(1);
    
      // Node ID
      Serial.write(0x12); Serial.write(0x34);
      Serial.write(0x56); Serial.write(0x78);
      Serial.write(0x9a); Serial.write(0xbc);
    
      // 11 x 0 (including the radio ID
      for (int i=0 ; i < 11 ; ++i)
      {
          Serial.write(0);
      }
    
      // 0xD2
      Serial.write(0xD2);
    
      if (readAPC220(rd,true))
      {
          if (memcmp(&d,&rd,sizeof(struct APC220data)) == 0)
          {
              return(true);
          }
      }
    }
    return(false);
}



boolean WaitForSerialAvailableUntil(unsigned long give_up_at)
{
    while(Serial.available() == 0)
    {
        if (millis() > give_up_at)
        {
            return(0);
        }
    }
    return(1);
}




void  CheckBacklightTimeout()
{
    if ((backlightOffTime > 0) && (millis() > backlightOffTime))
    {
        digitalWrite(LCD_BACKLIGHT_PIN,LOW);
        backlightOffTime = 0;
    }
}



void BacklightOn()
{
    if (backlightOffTime == 0)
    {
        digitalWrite(LCD_BACKLIGHT_PIN,HIGH);
    }
    backlightOffTime = millis() + BACKLIGHT_TIMEOUT;
}



void BatteryVoltage()
{
    float voltage;
    int a;
    
    do
    {
        a = analogRead(BATTERY_SENSE_PIN);
        delay(20);
        a = analogRead(BATTERY_SENSE_PIN);
        voltage = ((9.95+0.99)/0.99) * (a*1.1/1024.0);    // 10K - 1K resistor-divider across unregulated input voltage
        // 9.95K and 0.99K are the actual measured resistances on the resistor-divider
        lcd.clear();
        lcd.print("Battery: ");
        lcd.print(voltage);
        lcd.write('V');
        lcd.setCursor(0,1);
        lcd.print("A/D=");
        lcd.print(a);
        delay(500);
    } while(digitalRead(ROTARY_BUTTON_PIN) == LOW);
}

