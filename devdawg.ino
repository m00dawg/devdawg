/* DevDawg
 *  
 *  Open Source Arduino-Based Rotary Film Development Controller 
 *  
 */

#include "devtimes.h"

// Pins
const int motorForward = A1;
const int motorReverse = A2;
const int temperatureProbes = 2;
const int temperatureRelay = 3;
const int buzzer = 4;

const DevProcess process[] =
{
  {
    "C41",
    127,
    {
      {"Develop", 38, 195},
      {"Blix", 38, 195},
      {"Wash 1", 38, 360},
      {"Wash 2", 38, 120},
      {"Stabilizer", 0, 60}
    }
  },
  {
    "Delta 100 DDX 1+4",
    127,
    {
      {"Develop", 20, 720},
      {"Stop", 20, 60},
      {"Fix", 20, 300}
    }
  }
};

DevProcess currentProcess;

void setup() {
  

}

void loop() {
  // put your main code here, to run repeatedly:
  currentProcess = process[0];
}
