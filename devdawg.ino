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
#define VERSION "v0.03"
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
#define DEVELOP_FILM 1
#define PREHEAT 2
#define TEST_MOTOR 3
#define MAX_OPTION 3

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
const byte motorX = 6;
const byte motorY = 7;
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
unsigned long previousMotorMillis = 0;
bool motorDirection = FORWARD;

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
    255,
    10,
    24,
    5,    
    {
      {"Develop", 30, RED},
      {"Blix", 35, VIOLET},
      {"Wash 1", 36, BLUE},
      {"Wash 2", 37, TEAL},
      {"Stabilizer", 40, WHITE}
    }
  },
  {
    "C41",
    255,
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
    255,
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
  pinMode(motorX, OUTPUT);
  pinMode(motorY, OUTPUT);
  pinMode(recircPump, OUTPUT);
  
  digitalWrite(buzzer, LOW);
  digitalWrite(heater, LOW);
  analogWrite(motorSpeed, 0);
  digitalWrite(motorX, LOW);
  digitalWrite(motorY, LOW);
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

  /* Setup PID */
  pidWindowStartTime = millis();
  pidSetpoint = 100;
  pid.SetOutputLimits(0, pidWindowSizeMS);
  pid.SetMode(AUTOMATIC);
}

void loop()
{
  /* Menu Functions */
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
    case TEST_MOTOR:
    {
      testMotor();
      mode = 0;
      break;
    }
    // Main Menu
    default:
    {
      switch(option)
      {
        case DEVELOP_FILM:
        {
          button = wait("Select Mode", "Develop Film");
          break;
        }
        case PREHEAT:
        {
          button = wait("Select Mode", "Preheat");
          break;
        }
        case TEST_MOTOR:
        {
          button = wait("Select Mode", "Test Motor");
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


