#include <stddef.h>
#include <Wire.h>
#include <LiquidCrystal.h>
#include <Encoder.h>

// Define DEMO to simulate the presence of a radio
// #define DEMO

// Define DEBUG to send debugging messages to the serial port (9600 baud)
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

// Barometer pin (MPX4115A or MPXA6115A)
// (Remove this to omit support for MPX4115/MPLA6115A)
// #define BAROMETER_PIN      A1

// Include support for Adafruit BMP390 module ()
// (Remove this to omit support for the module)
#define USE_ADAFRUIT_BMP390 1

// *** NB: Only only one of the two barometer options above. It makes no sense
// ***     to include both, and the code won't compile
#if (defined(BAROMETER_PIN) && defined(USE_ADAFRUIT_BMP390))
#error "Don't define both BAROMETER_PIN and USE_ADAFRUIT_BMP390 - use one or the other (or neither)"
#endif

#ifdef USE_ADAFRUIT_BMP390
// Requires the Adafruit BMP3XX Library and its dependancies
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP3XX.h>
#endif

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
Encoder rotary(ROTARY_A_PIN, ROTARY_B_PIN);
int currentSerialBaudRate=-1;
unsigned long backlightOffTime=0;

#ifdef USE_ADAFRUIT_BMP390
Adafruit_BMP3XX bmp;
bool havebmp = FALSE;
#endif

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
        
#ifdef BAROMETER_PIN
    // Set up the analog pin connected to the barometric pressure sensor
    pinMode(BAROMETER_PIN, INPUT);
#endif
#ifdef USE_ADAFRUIT_BMP390
    if (bmp.begin_I2C())
    {
#ifdef DEBUG      
        Serial.begin(9600);
        Serial.println("Initialised BMP385 on I2C bus");
        Serial.end();
#endif        
        havebmp = TRUE;
        bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
        bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
        bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
        bmp.setOutputDataRate(BMP3_ODR_50_HZ);
        bmp.performReading();   // First reading looks to be crap...throw it away
    }
    else
    {      
#ifdef DEBUG      
        Serial.begin(9600);
        Serial.println("Failed to initialise BMP390");
        Serial.end();
#endif        
    }
#endif

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
    pinMode(BATTERY_SENSE_PIN, INPUT);
    analogReference(INTERNAL);

    // Pause for dramatic effect (also to allow A/D converter to settle to new reference, although it doesn't need this long)
    delay(618);
    
    // Display battery voltage, then switch the ADC to use the 5V reference (for use with the barometric pressure sensor)
    BatteryVoltage();
    analogReference(DEFAULT);
    delay(618);
}



void loop()
{
    struct MAIN_MENU_ITEMS{
        char* menuText;
        unsigned char x, y;
        unsigned char menuIndex;
    };

#define MAIN_MENU_READ 1
#define MAIN_MENU_WRITE 2
#define MAIN_MENU_PING 3
#define MAIN_MENU_BAROMETER 4
#define MAIN_MENU_MONITOR 5

#if (defined(BAROMETER_PIN) || defined(USE_ADAFRUIT_BMP390))
    static struct MAIN_MENU_ITEMS mainMenuItems[] = 
    {
      /*
      +----------------+
      | Read     Write |
      | Ping  Bar  Mon |
      +----------------+
      */
      { "Read", 1, 0, MAIN_MENU_READ },
      { "Write", 10, 0, MAIN_MENU_WRITE },
      { "Ping", 1, 1, MAIN_MENU_PING },
      { "Bar", 7, 1, MAIN_MENU_BAROMETER },
      { "Mon", 12, 1, MAIN_MENU_MONITOR },
    };
    static unsigned char mainMenuItemCount=5;
#else
    static struct MAIN_MENU_ITEMS mainMenuItems[] = 
    {
      /*
      +----------------+
      | Read     Write |
      | Ping   Monitor |
      +----------------+
      */
      { "Read", 1, 0, MAIN_MENU_READ },
      { "Write", 10, 0, MAIN_MENU_WRITE },
      { "Ping", 1, 1, MAIN_MENU_PING },
      { "Monitor", 8, 1, MAIN_MENU_MONITOR },
    };
    static unsigned char mainMenuItemCount=4;
#endif
    int lastRotaryPosition=0;
    static byte currentMenuItem=0;
    
    // Display main menu items
    lcd.clear();
    for (int i=0 ; i < mainMenuItemCount ; ++i)
    {
        lcd.setCursor(mainMenuItems[i].x, mainMenuItems[i].y);
        lcd.print(mainMenuItems[i].menuText);
    }
    
    // Wait for an event. An event is either the rotary encoder moving 
    // by ROTARY_SWITCH_INDENTS or the button being pressed
    lastRotaryPosition = rotary.read();
    while(1)
    {
                
        // Draw brackets around current menu item
        lcd.setCursor(mainMenuItems[currentMenuItem].x-1, mainMenuItems[currentMenuItem].y);
        lcd.write('[');
        lcd.setCursor(mainMenuItems[currentMenuItem].x + strlen(mainMenuItems[currentMenuItem].menuText), mainMenuItems[currentMenuItem].y);
        lcd.write(']');

        // See if the rotary encoder has moved
        int rotaryPosition = rotary.read();
        if (abs(rotaryPosition-lastRotaryPosition) >= ROTARY_SWITCH_INDENTS)
        {
            int lastMenuItem = currentMenuItem;
            if ((rotaryPosition < lastRotaryPosition) && (currentMenuItem > 0))
            {
                currentMenuItem--;
            }
            else if ((rotaryPosition > lastRotaryPosition) && (currentMenuItem < (mainMenuItemCount-1)))
            {
                currentMenuItem++;
            }
            if (currentMenuItem != lastMenuItem)
            {
                // Erase brackets around last
                lcd.setCursor(mainMenuItems[lastMenuItem].x-1, mainMenuItems[lastMenuItem].y);
                lcd.write(' ');
                lcd.setCursor(mainMenuItems[lastMenuItem].x + strlen(mainMenuItems[lastMenuItem].menuText), mainMenuItems[lastMenuItem].y);
                lcd.write(' ');
            }
            lastRotaryPosition = rotaryPosition;
            BacklightOn();
        }
        
        // If the button is pressed, follow the currently-selected menu item
        if (DebounceRotaryButton())
        {
            BacklightOn();
            switch(mainMenuItems[currentMenuItem].menuIndex)
            {
                case MAIN_MENU_READ:
                    Read();
                    break;
                case MAIN_MENU_WRITE:
                    Write();
                    break;
                case MAIN_MENU_PING:
                    Ping();
                    break;
#if (defined(BAROMETER_PIN) || defined(USE_ADAFRUIT_BMP390))
                case MAIN_MENU_BAROMETER:
                    Barometer();
                    break;
#endif                    
                case MAIN_MENU_MONITOR:
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
        unsigned int pingCount = 0;
        unsigned long nextPingTime = 0;
        while(1)
        {
          nextPingTime = millis() + 1000;
#ifdef BAROMETER_PIN
          float pressureSensorVoltage;
          float pressure = GetPressure(&pressureSensorVoltage);
#endif
#ifdef USE_ADAFRUIT_BMP390
          float pressure = GetPressure();
#endif
          pingCount++;
          Serial.print("Ping #" + String(pingCount));
#ifdef BAROMETER_PIN
          Serial.print(" : Vout=" + String(pressureSensorVoltage, 3));
#endif
#if (defined(BAROMETER_PIN) || defined(USE_ADAFRUIT_BMP390))
          Serial.print(" : Local Pressure=" + String(pressure, 2) + "hPa");
#endif
          Serial.println("");

          lcd.clear();
#if (defined(BAROMETER_PIN) || defined(USE_ADAFRUIT_BMP390))
          lcd.print("Sent " + String(pingCount) + " pings");
          lcd.setCursor(0,1);
          lcd.print(String(pressure, 2) + "hPa");
#else
          lcd.print("Sent " + String(pingCount) + " pings");
#endif
          while (millis() < nextPingTime)
          {
            if (DebounceRotaryButton())
            {
              goto EXIT_PINGS;
            }
          }
        }
EXIT_PINGS:
        Serial.end();
    }
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



#if (defined(BAROMETER_PIN) || defined(USE_ADAFRUIT_BMP390))
void Barometer()
{
    // TODO: Continuously display atmospheric pressure on LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.write("Current Pressure");
    while(1)
    {
        lcd.setCursor(0, 1);
#ifdef BAROMETER_PIN
          float pressure = GetPressure(NULL);
#endif
#ifdef USE_ADAFRUIT_BMP390
          float pressure = GetPressure();
#endif        
        lcd.print(String(pressure, 2) + "hPa");
        unsigned long nextUpdate = millis() + 1000;
        while (millis() < nextUpdate)
        {
            if (DebounceRotaryButton())
            {
                return;
            }
        }
    }
}
#endif

#ifdef BAROMETER_PIN
float GetPressure(float* pinVoltage)
{
#define OVERSAMPLE_BAROMETER 256
#define VREF 5.0          
    unsigned long t=0;
    for (int i=0 ; i < 256 ; ++i)   // Oversample to increase effective ADC resolution
    {
      t += analogRead(BAROMETER_PIN);
      delay(2);
    }
    float pressureSensorVoltage = (t * VREF) / (1024.0 * OVERSAMPLE_BAROMETER);
    if (pinVoltage) *pinVoltage = pressureSensorVoltage;
    float pressure = ((pressureSensorVoltage / VREF) + 0.095) / 0.009;   // Pressure in KPa - see MXP4115A/MXPA6115A datasheet
    return(pressure * 10.0);  // Return pressure in hPa
}
#endif

#ifdef USE_ADAFRUIT_BMP390
float GetPressure()
{
    if (havebmp)
    {
        if (!bmp.performReading()) 
        {
            return(0.0);
        }
        return(bmp.pressure / 100.0);
    }
    return(0.0);
}
#endif



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

