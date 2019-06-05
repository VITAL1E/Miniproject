#include <LiquidCrystal.h>

LiquidCrystal lcd(8,9,4,5,6,7);   //defining lcd pins

const int dataIN = 2; //IR sensor INPUT
// const int acc = 5; // Accuracy of output
const int N = 2; // Number of points of reference
int year = 2019; // Starting year
// int result = 0;

unsigned long prevmillis; // To store time
unsigned long duration; // To store time difference
unsigned long lcdrefresh; // To store time for LCD refresh

int setrpm;
int rpm; // RPM value

boolean currentstate; // Current state of IR input scan
boolean prevstate; // State of IR sensor in previous scan

void setup(){
  pinMode(dataIN,INPUT);
  lcd.begin(16,2);     
  prevmillis = 0;
  prevstate = LOW; 
}

struct StringScroller {
  String string = "Default String";
  int startIndex = 0;
  int delayTime = 400;
  unsigned long lastPrintTime = 0;
  int row = 1; // ?
  void print(LiquidCrystal *lcd) {
      auto now = millis();
      if (now - lastPrintTime >= delayTime) { // Check if not enough time has passed
        int endIndex = startIndex + 16;
         if (endIndex > string.length())
             endIndex = string.length();
        lcd->setCursor(0, row); // Set the lcd cursor position
        String toPrint = string.substring(startIndex, endIndex); // Return the substring to print
        if (toPrint.length() < 16) // Check if message full displayed
          toPrint += string.substring(0, 16 - toPrint.length()); // Reprint the message
        lcd->print(toPrint);
        startIndex = (startIndex + 1) % string.length(); // Scroll string by moving index left
        lastPrintTime = now; // Update time
      } else return;
  }

  StringScroller(String string): string(string) 
  {}
};

StringScroller
  story2018 = {"  Recycle model with sound for Aalborg municipality. "},
  story2016 = {"  Steel model with light for Okolariet. "},
  story2010 = {"  First Energy Wheel for Roskilde Festival. "},
  storyDefault = {"                                   "};
 
void loop(){
 // Measure Actual RPM 
 currentstate = digitalRead(dataIN); // Read IR sensor state
 if(prevstate != currentstate){ // If there is change in input
     if(currentstate == HIGH){ // If input changes only from LOW to HIGH
         duration = (micros() - prevmillis) * N; // Time difference between revolution in microseconds
         year--;
         prevmillis = micros(); // store time for next revolution calculation
         rpm = (60000000 / duration);  // rpm = (1/ time millis)*1000*1000*60;
       }
 }
  prevstate = currentstate; // store this scan (prev scan) data for next scan
  
//   Measure RPM required
  setrpm = map(analogRead(5), 0, 1023, 0, 8000); // Read set value from analog pin 5 (0 & 8000 min. amd max. rpm)                         
  
  // LCD Display
  if((millis() - lcdrefresh) >= 100) {
      lcd.setCursor(0, 0);
      lcd.print("Year is ");
      lcd.print(year);
      
      if (year == 2018) {
         story2018.print(&lcd);
      } else if (year == 2016) {
        story2016.print(&lcd);
      } else if (year == 2010) {
        story2010.print(&lcd);
      } else
        storyDefault.print(&lcd);
      
      //lcd.setCursor(0,0);
      //lcd.setCursor(0,1);
      lcdrefresh = millis();   
    }
}
