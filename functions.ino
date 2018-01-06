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

// Run a full development process
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
