//codeDirtyToMe
//DIY Gear Indicator
//Made to work with that 16*2 LCD monitor that is pretty popular with Arduino kits.

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4);  // Set the address to 0x27 for a 16 chars and 2 lines.

// Upshift signal on pin 2
const int upShift = 2;

// Downshift signal on pin 3
const int downShift = 4;

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
  // Initialize input pins.
  pinMode(upShift, INPUT);
  pinMode(downShift, INPUT);
  pinMode(neutralResetSwitch, INPUT);  
  
  // Initialize the lcd
  lcd.init();
  lcd.backlight();
  
  // Stupid boot up screen. Really just a way to give the board a few seconds to get itself together.
  // I could use a loop here, but copy and paste was easier!!! So dirty.
      delay(500);
      lcd.setCursor(2,0);
      lcd.print("Starting in:");
      lcd.setCursor(8,1);
      lcd.print("3");
      delay(1000);
      lcd.clear();
      lcd.setCursor(8,1);
      lcd.print("2");
      delay(1000);
      lcd.clear();
      lcd.setCursor(8,1);
      lcd.print("1");
      delay(1000);
      lcd.clear();
  
  neutralReset(); //Always start in neutral after booting. This assumes that the bike is started in neutral.
}

//This function is how I deal with the neutral position. 
void neutralReset(){
  
  //Start by printing neutral position.
  lcd.setCursor(6,0);
  lcd.print("GEAR");
  lcd.setCursor(4,1);
  lcd.print("Neutral");

/* I need this function to only determine the first shift. In other words, once this boolean value changes
 *  state, the thread will be passed back to main loop.*/
bool firstShift = false;

// While no shift has been detected, continue scanning for it.
while (firstShift == false){
  upShiftState = digitalRead(upShift);
  downShiftState = digitalRead(downShift);

  // If an upshift is detected,
  if (upShiftState == HIGH){
    gearState = 2; // Set gearState to 2.

    // Print out the gear.
    lcd.clear();
    lcd.setCursor(6,0);
    lcd.print("GEAR");
    lcd.setCursor(7,1);
    lcd.print("#");
    lcd.setCursor(8,1);
    lcd.print(gearState);
    delay(500);
    
    firstShift = true; // Stop the loop.
  }

  //If a downshift is detected,
  else if (downShiftState == HIGH){
    gearState = 1; // Set gearState to 1.

    // Print out the gear.   
    lcd.clear();
    lcd.setCursor(6,0);
    lcd.print("GEAR");
    lcd.setCursor(7,1);
    lcd.print("#");
    lcd.setCursor(8,1);
    lcd.print(gearState);
    delay(500);
    
    firstShift = true; // Stop the loop.
  }
}
  return; // Continue to loop.
}

void loop()
{
  // Get the screen ready.
  lcd.setCursor(6,0);
  lcd.print("GEAR");
  lcd.setCursor(7,1);
  lcd.print("#");
  lcd.setCursor(8,1);
  
  // Read state of upshift & downshift
  upShiftState = digitalRead(upShift);
  downShiftState = digitalRead(downShift);

  // Read state of neutral reset switch
  neutralState = digitalRead(neutralResetSwitch);

  // If the neutral reset switch has been hit,
  if (neutralState == HIGH){
    neutralReset(); //Display neutral and scan for next shift.
  }
  // If upshift detected,
  else if (upShiftState == HIGH){
    nextGearState = gearState + 1; // Predict next gear position. 
    /* BTW, doing if(gearState++ >= 6) will not work as the increment operator actually increments it permanently.
     * In other words, a temporary holder is needed. Hence, the nextGearState variable.*/
    if (nextGearState >= 6){ // No 6th gear on DR650 :(
      gearState = 5; // Keep it at 5th.
      delay(500);
      lcd.print(gearState);
    }
    else {
    gearState++; // Upshift detected and less than 6, go ahead and increment.
    delay(500);
    lcd.print(gearState);
    }
  }
  //If downshift detected, pretty much same as above.
  else if (downShiftState == HIGH){
    nextGearState = gearState - 1;
    if (nextGearState <= 0){
      gearState = 1;
      delay(500);
      lcd.print(gearState);
    }
    else {
    gearState--;
    delay(500);
    lcd.print(gearState);
  }
}
}
