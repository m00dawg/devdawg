/* DevDawg
 *  
 * Open Source Arduino-Based Rotary Film Development Controller.
 * by Tim Soderstrom
 *  
 *  Licensed under the GPLv3 (see LICENSE for more information)
 */

/**********
 * Defines 
 **********/
#define VERSION "v0.02"
#define BANNER_DELAY_MS 1000

#define RED 0x1
#define YELLOW 0x3
#define GREEN 0x2
#define TEAL 0x6
#define BLUE 0x4
#define VIOLET 0x5
#define WHITE 0x7

#define LCD_COLUMNS 16
#define LCD_ROWS 2

#define SECONDS_MS 1000

#define FORWARD 0
#define REVERSE 1

#define ON 1
#define OFF 0

// Menu Options
#define MAIN_MENU 0
#define PREHEAT 1
#define DEVELOP_FILM 2
#define MAX_OPTION 2

// Tunable Defines
// Maximum number of dev steps a particular Dev Process can have.
#define MAX_DEV_STEPS 8
// How often to check the temperature probe
#define TEMP_UPDATE_INTERVAL 10
// Default Preheat Temperature (C)
#define DEFAULT_PREHEAT_TEMP 39
// PID Parameters
#define KP 2
#define KI 5
#define KD 1

/***********
 * Includes 
 ***********/
#include <PID_v1.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_RGBLCDShield.h>
#include "structs.h"

/*******
 * Pins
 *******/
const byte temperatureProbes = 2;
const byte buzzer = 3;
const byte heater = 4;
const byte motorSpeed = 5;
const byte motorForward = 6;
const byte motorReverse = 7;
const byte recircPump = 8;

/************
 * Variables 
 ************/
byte pickedProcess = 0;
byte button = 0;
byte mode = 0;
byte option = 0;
double pidInput, pidOutput, pidSetpoint;
unsigned long pidWindowStartTime;
unsigned int pidWindowSizeMS = 5000;
bool heaterState = OFF;

/**********
 * Objects 
 **********/
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(temperatureProbes);
DallasTemperature sensors(&oneWire);
DeviceAddress thermometer;

// PID Object
PID pid(&pidInput, &pidOutput, &pidSetpoint, KP, KI, KD, DIRECT);

/******************************
 * Development Process Structs 
 ******************************/

// Dev Processes (These can eventually be in the EEPROM)
const byte processCount = 3;
// Dev Processes
const DevProcess process[] =
{
  {
    "Test",
    127,
    2,
    24,
    5,    
    {
      {"Develop", 5, RED},
      {"Blix", 10, VIOLET},
      {"Wash 1", 20, BLUE},
      {"Wash 2", 30, TEAL},
      {"Stabilizer", 40, WHITE}
    }
  },
  {
    "C41",
    127,
    10,
    38,
    5,
    {
      {"Develop", 195, RED},
      {"Blix", 195, VIOLET},
      {"Wash 1", 360, BLUE},
      {"Wash 2", 120, TEAL},
      {"Stabilizer", 60, WHITE}
    }
  },
  {
    "Dlta100 DDX 1+4",
    127,
    10,
    20,
    3,
    {
      {"Develop", 720, BLUE},
      {"Stop", 60, RED},
      {"Fix", 300, WHITE}
    }
  }
};

DevProcess currentProcess;

/*******
 * Code 
 ********/

void setup()
{
  pinMode(buzzer, OUTPUT);
  pinMode(heater, OUTPUT);
  pinMode(motorSpeed, OUTPUT);
  pinMode(motorForward, OUTPUT);
  pinMode(motorReverse, OUTPUT);
  pinMode(recircPump, OUTPUT);
  
  digitalWrite(buzzer, LOW);
  digitalWrite(heater, LOW);
  analogWrite(motorSpeed, 0);
  digitalWrite(motorForward, LOW);
  digitalWrite(motorReverse, LOW);
  digitalWrite(recircPump, LOW);
  
  Serial.begin(9600); 
  lcd.begin(LCD_COLUMNS, LCD_ROWS);

  /* Print Title and Version */
  lcd.setBacklight(YELLOW);
  lcd.setCursor(0,0);
  lcd.print("DevDawg");
  lcd.setCursor(0,1);
  lcd.print(VERSION);
  delay(BANNER_DELAY_MS);

  pidWindowStartTime = millis();

  //initialize the variables we're linked to
  pidSetpoint = 100;

  //tell the PID to range between 0 and the full window size
  pid.SetOutputLimits(0, pidWindowSizeMS);

  //turn the PID on
  pid.SetMode(AUTOMATIC);
}

void loop()
{
  switch(mode)
  {
    case PREHEAT:
    {
      preheat(DEFAULT_PREHEAT_TEMP);
      mode = 0;
      break;
    }
    case DEVELOP_FILM:
    {
      developFilm();
      mode = 0;
      break;
    }
    // Main Menu
    default:
    {
      switch(option)
      {
        case PREHEAT:
        {
          button = wait("Select Mode", "Preheat");
          break;
        }
        case DEVELOP_FILM:
        {
          button = wait("Select Mode", "Develop Film");
          break;
        }
        default:
        {
          button = wait("Main Menu","");
          break;
        }
      }
      delay(250);
      switch(button)
      {
        case BUTTON_UP:
        {
          ++option;
          if(option > 2)
            option = 0;
          Serial.println("Button Up");
          Serial.print("Option: ");
          Serial.println(option);
          break;
        }
        case BUTTON_DOWN:
        {
          --option;
          if(option > 2)
            option = MAX_OPTION;
          Serial.println("Button Down");
          Serial.print("Option: ");
          Serial.println(option);
          break;
        }
        case BUTTON_SELECT:
        {
          Serial.println("Button Select");
          mode = option;
          option = 0;
          break;
        }
      }
    }
  }

  // Reset back to main menu
  button = 0;
}


