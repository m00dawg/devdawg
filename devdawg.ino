/* DevDawg
 *  
 *  Open Source Arduino-Based Rotary Film Development Controller 
 *  
 */

#include <Adafruit_RGBLCDShield.h>
#include "devtimes.h"

// Defines
#define RED 0x1
#define YELLOW 0x3
#define GREEN 0x2
#define TEAL 0x6
#define BLUE 0x4
#define VIOLET 0x5
#define WHITE 0x7

#define LCD_COLUMNS 16
#define LCD_ROWS 2

#define TEMP_UPDATE_INTERVAL 10
#define SECONDS_MS 1000

// Pins
const int motorForward = A1;
const int motorReverse = A2;
const int temperatureProbes = 2;
const int temperatureRelay = 3;
const int buzzer = 4;

// Simple Variables
byte pickedProcess = 0;

// Objects
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

const byte processCount = 3;
// Dev Processes
const DevProcess process[] =
{
  {
    "Test",
    127,
    5,
    {
      {"Develop", 38, 20, RED},
      {"Blix", 38, 20, VIOLET},
      {"Wash 1", 38, 20, BLUE},
      {"Wash 2", 38, 20, TEAL},
      {"Stabilizer", 0, 20, WHITE}
    }
  },
  {
    "C41",
    127,
    5,
    {
      {"Develop", 38, 195, RED},
      {"Blix", 38, 195, VIOLET},
      {"Wash 1", 38, 360, BLUE},
      {"Wash 2", 38, 120, TEAL},
      {"Stabilizer", 0, 60, WHITE}
    }
  },
  {
    "Dlta100 DDX 1+4",
    127,
    3,
    {
      {"Develop", 20, 720, BLUE},
      {"Stop", 20, 60, RED},
      {"Fix", 20, 300, WHITE}
    }
  }
};

DevProcess currentProcess;

void setup() {
  Serial.begin(9600); 
  lcd.begin(LCD_COLUMNS, LCD_ROWS);


  /* Print Title and Version */
  lcd.setBacklight(YELLOW);
  lcd.setCursor(0,0);
  lcd.print("DevDawg");
  lcd.setCursor(0,1);
  lcd.print("v0.01");
  delay(500);
  
  
}

void loop()
{
  pickedProcess = pickProcess();
  currentProcess = process[pickedProcess];

  wait(currentProcess.name, "Press Any Key");
  // After picking a process, start dev
  for(byte i = 0 ; i < currentProcess.steps; ++i)
  {
    processStep(currentProcess.devStep[i].name, 
      currentProcess.devStep[i].color,
      currentProcess.motorSpeed,
      currentProcess.devStep[i].targetTemperature, 
      currentProcess.devStep[i].duration);
    wait(currentProcess.devStep[i].name, "Complete");
  }
  wait("Dev Cycle", "Complete");
}

byte pickProcess()
{
  byte processIndex = 0;
  byte button = 0;

  while(true)
  {
    while(!button)
      button = wait("Dev for", process[processIndex].name);
    if (button & BUTTON_UP)
    {
      ++processIndex;
      if(processIndex > processCount)
        processIndex = 0;
    }
    if (button & BUTTON_DOWN)
      --processIndex;
      // Greater than because we are using a byte so we need to check
      // for overflow rather than negative numbers.
      if(processIndex > processCount)
        processIndex = processCount;
    if (button & BUTTON_SELECT)
      return processIndex;
    button = 0;
  }
}

void processStep(char* name, byte color, byte motorSpeed, byte targetTemperature, unsigned int duration)
{
  unsigned long currentMillis = millis();
  unsigned long previousMillis = 0;
  long timeRemainingMS = long(duration) * SECONDS_MS;
  unsigned int temperature = 20;
  byte tempUpdateCycle = 0;
  
  Serial.println(duration);
  Serial.println(motorSpeed);
  Serial.println(targetTemperature);
  Serial.println(timeRemainingMS);
  Serial.println("Loop");

  lcd.setBacklight(color);
  lcd.print(name);
  
  while(timeRemainingMS >= 0)
  {
    currentMillis = millis();
    // Update once a second
    if (currentMillis - previousMillis >= SECONDS_MS)
    {
      previousMillis = currentMillis;
      Serial.print("ms: ");
      Serial.println(millis());
      Serial.print("Time remaining: ");
      Serial.println(timeRemainingMS);
      updateDisplay(timeRemainingMS / SECONDS_MS, temperature);
      timeRemainingMS -= SECONDS_MS;
      
      // Every 10 seconds, check temperature
      if(tempUpdateCycle >= TEMP_UPDATE_INTERVAL)
      {
        // Placeholder until a real temp sensor is in place
        temperature = random(15,25);
        tempUpdateCycle = 0;
      }
      else
        tempUpdateCycle++;
    }
  }
}

void updateDisplay(int totalSeconds, int temperature)
{
  char buffer[16];
  sprintf(buffer, "%02d:%02d  Temp: %dC", totalSeconds / 60, totalSeconds % 60, temperature);
  lcd.setCursor(0,1);
  lcd.print(buffer);
}

byte wait(char line1[16], char line2[16])
{
  byte button=0;
  lcd.clear();
  lcd.print(line1);
  lcd.setCursor(0,1);
  lcd.print(line2);
  while(!button)
    button = lcd.readButtons();
  lcd.clear();
  return button;
}

