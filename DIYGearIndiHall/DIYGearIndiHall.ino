
//DIY Gear Indicator w/ Hall effect sensors
//Made to work with a Diymall OLED display.
//codeDirtyToMe

//I stole some of the code from the folks below so that I could talk to the OLED.

/**************************************************************************
 This is an example for our Monochrome OLEDs based on SSD1306 drivers

 Pick one up today in the adafruit shop!
 ------> http://www.adafruit.com/category/63_98

 This example is for a 128x32 pixel display using I2C to communicate
 3 pins are required to interface (two I2C and one reset).

 Adafruit invests time and resources providing this open
 source code, please support Adafruit and open-source
 hardware by purchasing products from Adafruit!

 Written by Limor Fried/Ladyada for Adafruit Industries,
 with contributions from the open source community.
 BSD license, check license.txt for more information
 All text above, and the splash screen below must be
 included in any redistribution.
 **************************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Upshift signal on pin 2
const int upShift = A1;

// Downshift signal on pin 3
const int downShift = A2;

// Neutral reset switch on pin 7
const int neutralResetSwitch = 7;

// Global Gear state variables. I shouldn't use globals, BUT this is pretty simple and I wanna knock this out quick.
int upShiftState = 0;
int downShiftState = 0;
int gearState = 0;
int nextGearState = 0;
int neutralState = 0;

void setup()
{
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  
  // Initialize input pins.
  pinMode(neutralResetSwitch, INPUT);  
  
  // Stupid boot up screen. Really just a way to give the board a few seconds to get itself together.
  // I could use a loop here, but copy and paste was easier!!! So dirty.
      display.clearDisplay();
      delay(1000);
      display.setTextSize(3);
      display.setTextColor(WHITE);
      display.setCursor(30, 20);
      display.println("BOOT");
      display.display();
      delay(1000);
      display.clearDisplay();
  
   neutralReset(); //Always start in neutral after booting. This assumes that the bike is started in neutral.
}

//This function is how I deal with the neutral position. 
void neutralReset(){
  
  //Start by printing neutral position.
  display.clearDisplay();
  display.setTextSize(5);
  display.setCursor(50,25);
  display.println("N");
  display.display();
  delay(1000);
  
/* I need this function to only determine the first shift. In other words, once this boolean value changes
 *  state, the thread will be passed back to main loop.*/
bool firstShift = false;

// While no shift has been detected, continue scanning for it.
while (firstShift == false){
  upShiftState = analogRead(upShift);
  downShiftState = analogRead(downShift);
  Serial.println(upShiftState);
  
  // If an upshift is detected,
  if (upShiftState <= 100){
    gearState = 2; // Set gearState to 2.
    Serial.println("upshift");
    // Print out the gear.
    display.clearDisplay();
    display.setTextSize(5);
    display.setCursor(50,25);
    display.println(gearState);
    display.display();
    delay(500);
    display.clearDisplay();
    
    firstShift = true; // Stop the loop.
  }

  //If a downshift is detected,
  else if (downShiftState <= 100){
    gearState = 1; // Set gearState to 1.

    Serial.println("downshift");
    // Print out the gear.   
    display.clearDisplay();
    display.setTextSize(5);
    display.setCursor(50,25);
    display.println(gearState);
    display.display();
    delay(500);
    //display.clearDisplay();
    
    firstShift = true; // Stop the loop.
  }
}
  return; // Continue to loop.
}

void loop()
{
  // Get the screen ready.
  display.setCursor(50,25);
  
  // Read state of upshift & downshift
  upShiftState = analogRead(upShift);
  downShiftState = analogRead(downShift);

  // Read state of neutral reset switch
  neutralState = digitalRead(neutralResetSwitch);

  Serial.println(upShiftState);
  
  // If the neutral reset switch has been hit,
  if (neutralState == HIGH){
    Serial.println(neutralState);
    delay(100);
    neutralReset(); //Display neutral and scan for next shift.
  }
  // If upshift detected,
  else if (upShiftState <= 100){
    nextGearState = gearState + 1; // Predict next gear position. 
    /* BTW, doing if(gearState++ >= 6) will not work as the increment operator actually increments it permanently.
     * In other words, a temporary holder is needed. Hence, the nextGearState variable.*/
 
    if (nextGearState >= 6){ // No 6th gear on DR650 :(
      gearState = 5; // Keep it at 5th.
      delay(500);
      display.println(gearState);
      display.display();
    }
    else {
    gearState++; // Upshift detected and less than 6, go ahead and increment.
    delay(500);
    display.clearDisplay();
    display.println(gearState);
    display.display();
    }
  }
  //If downshift detected, pretty much same as above.
  else if (downShiftState <= 100){
    nextGearState = gearState - 1;
    if (nextGearState <= 0){
      gearState = 1;
      delay(500);
      display.println(gearState);
      display.display();
    }
    else {
    gearState--;
    delay(500);
    display.clearDisplay();
    display.println(gearState);
    display.display();
  }
}
}
