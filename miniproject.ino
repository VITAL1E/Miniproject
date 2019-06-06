#include <LiquidCrystal.h>

LiquidCrystal lcd(8,9,4,5,6,7);

unsigned long prevmillis; // To store time
unsigned long duration; // To store time difference
unsigned long lcdrefresh; // To store time for LCD refresh

void setup(){
  pinMode(2,INPUT);
  lcd.begin(16,2);     
  prevmillis = 0;
}

class Timed
{
private:
    long previousRun = 0;
    long delay = 0;
    virtual void _run()=0;
public:
    void run() // If enough time passed since last _run, call _run again and update time 
    {
        auto now = millis();
        if (now - previousRun >= delay)
        {
            _run();
            previousRun = now;
        }
    }
    void setDelay(long delay)
    {
        this->delay = delay;
    }
};

class StringScroller : public Timed
{
    String string = "Default String";
    int startIndex = 0;
    int row = 1;

    void _run() override
    {
        int endIndex = startIndex + 16;
         if (endIndex > string.length())
             endIndex = string.length();
        lcd.setCursor(0, row); // Set the lcd cursor position
        String toPrint = string.substring(startIndex, endIndex); // Return the substring to print
        if (toPrint.length() < 16) // Check if message full displayed
          toPrint += string.substring(0, 16 - toPrint.length()); // Reprint the message
        lcd.print(toPrint);
        startIndex = (startIndex + 1) % string.length();
    }
public:
    StringScroller(String string, int delay=400): 
        string(string) // Initialize string field from class
    {
        setDelay(delay);
    }
};

class LED : public Timed
{
    long onTime;
    long offTime;
    long ledState = LOW;
    int ledPin = 0;


    void _run() override // Override run method from Timed class
    {
        toggle(); 
    }

public:
    LED(int ledPin, long OnTime, long OffTime):
    ledPin (ledPin), onTime(OnTime), offTime(OffTime) // Initialize fields from class
    {
      setDelay(offTime);
      pinMode(ledPin, OUTPUT);
    }

    void off()
    {
      ledState = LOW;
      digitalWrite(ledPin, ledState);
    }
    
    void toggle() // Change states and delay of the LED 
    {
        if (ledState == HIGH)
            setDelay(offTime);
        else
            setDelay(onTime);
        ledState = ledState == HIGH? LOW: HIGH;
        digitalWrite(ledPin, ledState);  // Update the actual LED
    }

    
} led(12, 100, 100);

StringScroller
  story2018 = {"  Recycle model with sound for Aalborg municipality. "},
  story2016 = {"  Steel model with light for Okolariet. "},
  story2010 = {"  First Energy Wheel for Roskilde Festival. "},
  storyDefault = {"                                   "};

class YearDecrementer : public Timed
{
  int year = 2019;
  int previousState = LOW;
  int dataIN = 2;
  void _run() override // Override run method from Timed class
  {
    auto currentstate = digitalRead(dataIN);
    if(previousState != currentstate){ // If there is change in input and from LOW to HIGH
            if(currentstate == HIGH){
              if (year > 0)
                year--;
              else year = 2019;
            }
            previousState = currentstate;
    }
  }
public:
  int getYear()
  {
    return year;
  }

  YearDecrementer(){
    setDelay(100);
  }
} yearDecrementer;

void loop()
{
    yearDecrementer.run();
    

    if((millis() - lcdrefresh) >= 100) {
        lcd.setCursor(0, 0);
        auto year = yearDecrementer.getYear();
        
        lcd.print("Year is ");
        lcd.print(year);
        
        if (year == 2018) {
            story2018.run();
            led.run();
        } else if (year == 2016) {
            story2016.run();
            led.run();
        } else if (year == 2010) {
            story2010.run();
            led.run();
        } else {
          storyDefault.run();
          led.off();
        }
            
        lcdrefresh = millis();   
    }
}
