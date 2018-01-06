

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
        // Placeholder until a real temp sensor is in place
        currentTemperature = collectTemperatures();
        tempUpdateCycle = 0;
        Serial.print("Temperature: ");
        Serial.println(currentTemperature);
      }
      else
        ++tempUpdateCycle;

      // Reverse direction of rotation every specified seconds
      if(motorUpdateCycle > motorDirectionInterval - 1)
      {
        if(motorDirection == FORWARD)
          motorDirection = REVERSE;
        else
          motorDirection = FORWARD;
        motorUpdateCycle = 0;
        Serial.print("Motor Direction: ");
        Serial.println(motorDirection);
      }
      else
        ++motorUpdateCycle;

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
  // After picking a process, pre-heat then start dev
  preheat(currentProcess.targetTemperature);
  for(byte i = 0 ; i < currentProcess.steps; ++i)
  {
    processStep(currentProcess.devStep[i], 
      currentProcess.motorSpeed, 
      currentProcess.motorDirectionInterval, 
      currentProcess.targetTemperature);
    Serial.println("done");
    wait(currentProcess.devStep[i].name, "Complete");
  }
  lcd.setBacklight(GREEN);
  wait("Dev Cycle", "Complete");
}

void preheat(int desiredTemperature)
{
  unsigned long previousMillis = 0;
  byte button = 0;
  int targetTemperature = desiredTemperature;
  int currentTemperature = 0;
  char buffer[17];
  
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
      if(currentTemperature == targetTemperature)
      {
        wait("Target", "Reached");
        return;
      }
      else
      {
        sprintf(buffer, "%02dC / %02dC", currentTemperature, targetTemperature);
        drawDisplay("Preheating", buffer);
      }
      previousMillis = millis();
    }
    button = lcd.readButtons();
    if(button & BUTTON_LEFT)
      return;
    if(button & BUTTON_UP)
      targetTemperature += 1;
    if(button & BUTTON_DOWN)
    targetTemperature -= 1;
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

