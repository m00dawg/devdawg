

// Pick a development process using the LCD/Buttons 
byte pickProcess()
{
  // Selected process (by array index)
  byte processIndex = 0;
  byte button = 0;

  while(true)
  {
    while(!button)
      button = wait("Dev for", process[processIndex].name);
    if (button & BUTTON_UP)
    {
      ++processIndex;
      if(processIndex > processCount - 1)
        processIndex = 0;
    }
    if (button & BUTTON_DOWN)
      --processIndex;
      // Greater than because we are using a byte so we need to check
      // for overflow rather than negative numbers.
      if(processIndex > processCount - 1)
        processIndex = processCount - 1;
    if (button & BUTTON_SELECT)
      return processIndex;
    button = 0;
    delay(250);
    Serial.print("Index: ");
    Serial.println(processIndex);
  }
}

// Run a development step
void processStep(DevStep step, byte motorSpeed, byte motorDirectionInterval, int targetTemperature)
{
  unsigned long currentMillis = millis();
  unsigned long previousMillis = 0;
  long timeRemainingMS = long(step.duration) * SECONDS_MS;
  int currentTemperature = collectTemperatures();
  byte tempUpdateCycle = 0;
  byte motorUpdateCycle = 0;
  byte motorDirection = FORWARD;
  
  Serial.println(step.duration);
  Serial.println(motorSpeed);
  Serial.println(targetTemperature);
  Serial.println(currentTemperature);
  Serial.println(timeRemainingMS);

  lcd.setBacklight(step.color);
  lcd.print(step.name);
  
  while(timeRemainingMS >= 0)
  {
    currentMillis = millis();

    controlTemp();
    
    // Update once a second
    if(currentMillis - previousMillis >= SECONDS_MS)
    {
      Serial.print("Time remaining: ");
      Serial.println(timeRemainingMS);
      
      // Every 10 seconds, check temperature
      if(tempUpdateCycle > TEMP_UPDATE_INTERVAL - 1)
      {
        currentTemperature = collectTemperatures();
        tempUpdateCycle = 0;
        Serial.print("Temperature: ");
        Serial.println(currentTemperature);
      }
      else
        ++tempUpdateCycle;
      controlMotor(motorSpeed, motorDirectionInterval);
      updateDisplay(int(timeRemainingMS / SECONDS_MS), currentTemperature);
      timeRemainingMS -= SECONDS_MS;
      previousMillis = millis();
    }
  }
}

// Run a full development process
void developFilm()
{
  pickedProcess = pickProcess();
  currentProcess = process[pickedProcess];
  wait(currentProcess.name, "Press Any Key");
  controlMotor(0, 0);
  // After picking a process, pre-heat then start dev
  targetTemperature = currentProcess.targetTemperature;
  preheat();
  for(byte i = 0 ; i < currentProcess.steps; ++i)
  {
    processStep(currentProcess.devStep[i], 
      currentProcess.motorSpeed, 
      currentProcess.motorDirectionInterval, 
      currentProcess.targetTemperature);
    Serial.println("Motor: Stopping");
    controlMotor(0, 0);
    Serial.println("done");
    wait(currentProcess.devStep[i].name, "Complete");
  }
  lcd.setBacklight(GREEN);
  wait("Dev Cycle", "Complete");
}

void preheat()
{
  unsigned long previousMillis = 0;
  byte button = 0;
  char buffer[17];
  char currentTempChar[6];
  char targetTempChar[6];
  
  while(true)
  {
    controlTemp();
    if(millis() - previousMillis >= SECONDS_MS * TEMP_UPDATE_INTERVAL || button)
    {
      currentTemperature = collectTemperatures();
      Serial.print("Current Temperature: ");
      Serial.println(currentTemperature);
      Serial.print("Target Temperature: ");
      Serial.println(targetTemperature);
      Serial.print("PID Output: ");
      Serial.println(pidOutput);
      /* Removing for now since for mixing chemicals we want it to still adjust
       *  temperatures (since heating the chemicals would bring the water
       *  temperature down over time).
      if(currentTemperature == targetTemperature)
      {
        wait("Target", "Reached");
        return;
      }
      else
      {
      */
      dtostrf(currentTemperature, 5, 2, currentTempChar);
      dtostrf(targetTemperature, 5, 2, targetTempChar);
      sprintf(buffer, "%sC / %sC", currentTempChar, targetTempChar);
      //sprintf(buffer, "%02dC / %02dC", currentTemperature, targetTemperature);
      drawDisplay("Preheating", buffer);
      previousMillis = millis();
    }
    button = lcd.readButtons();
    if(button & BUTTON_LEFT)
      return;
    if(button & BUTTON_UP)
      targetTemperature += TEMP_ADJUSTMENT_PRECISION;
    if(button & BUTTON_DOWN)
    targetTemperature -= TEMP_ADJUSTMENT_PRECISION;
  }
}


void controlTemp()
{
  pid.Compute();  
  if (millis() - pidWindowStartTime > pidWindowSizeMS)
    pidWindowStartTime += pidWindowSizeMS;
  if (pidOutput > millis() - pidWindowStartTime)
  {    
    if(heaterState != ON)
    {
      heaterState = ON;
      Serial.println("Turned Heater On");
    }
    digitalWrite(heater, HIGH);
  }
  else
  {
    if(heaterState != OFF)
    {
      heaterState = OFF;
      Serial.println("Turned Heater Off");
    }
    digitalWrite(heater, LOW);
  }
}

void controlMotor(byte speed, byte directionInterval)
{
  analogWrite(motorSpeed, speed);  

  if(directionInterval > 0)
  {
    if(millis() - previousMotorMillis >= SECONDS_MS * directionInterval)
    {
      if(motorDirection == FORWARD)
      {
        Serial.println("Motor: Reverse");
        digitalWrite(motorX, LOW);
        digitalWrite(motorY, HIGH);
        motorDirection = REVERSE;
      }
      else
      {
        Serial.println("Motor: Forward");
        digitalWrite(motorX, HIGH);
        digitalWrite(motorY, LOW);
        motorDirection = FORWARD;
      }
      previousMotorMillis = millis();
    }
  }
  /* If directionInterval is zero, never change speed */
  else
  {
    digitalWrite(motorX, HIGH);
    digitalWrite(motorY, LOW);
    motorDirection = FORWARD;
  }
}



