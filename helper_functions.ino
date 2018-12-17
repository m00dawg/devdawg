void updateDisplay(int totalSeconds, int temperature)
{
  char buffer[32];
  sprintf(buffer, "%02d:%02d  Temp: %02dC", int(totalSeconds / 60), int(totalSeconds % 60), temperature);
  lcd.setCursor(0,1);
  lcd.print(buffer);
}



byte wait(char line1[16], char line2[16])
{
  byte button=0;
  drawDisplay(line1, line2);
  while(!button)
    button = lcd.readButtons();
  lcd.clear();
  delay(250);
  return button;
}

void drawDisplay(char line1[16], char line2[16])
{
  lcd.clear();
  lcd.print(line1);
  lcd.setCursor(0,1);
  lcd.print(line2);
}

double collectTemperatures()
{
  sensors.requestTemperatures();
  if(sensors.getAddress(thermometer, 0))
  {
    return double(sensors.getTempC(thermometer));
  }
  return -255;
}

void runMotor()
{
  byte rpm = MOTOR_RPM;
  byte directionInterval = 5;
  bool runMotor = true;
  char buffer[32];
  byte button=0;

  sprintf(buffer, "RPM %02d : I %02d", rpm, directionInterval);
  drawDisplay("Turning Rotary", buffer); 
  while(true)
  {
    if(runMotor)
      controlMotor(toSpeed(rpm), directionInterval);
    else
      controlMotor(0, 0);
    button = lcd.readButtons();
    if(button)
    {
      Serial.println("In button if");
      switch(button)
      {
        case BUTTON_UP:
        {
          if(rpm < MOTOR_RPM)
            ++rpm;
          button = 0;
          break;
        }
        case BUTTON_DOWN:
        {
          if(rpm > MOTOR_MIN_RPM)
            --rpm;
          button = 0;
          break;
        }
        case BUTTON_LEFT:
        {
          if(directionInterval > 1)
            --directionInterval;
          button = 0;
          break;
        }
        case BUTTON_RIGHT:
        {
          if(directionInterval < 255)
            ++directionInterval;
          button = 0;
          break;
        }
        case BUTTON_SELECT:
        {
          if(runMotor)
          {
            drawDisplay("Turning Rotary", "Paused"); 
            runMotor = false;
          }
          else
            runMotor = true;
        }
      }
      if(runMotor)
      {
        sprintf(buffer, "RPM %02d : I %02d", rpm, directionInterval);
        drawDisplay("Running Motor", buffer); 
      }
    }
  }
}

int toRPM(byte value)
{
  return map(value, 0, 255, 0, MOTOR_RPM);
}

int toSpeed(byte value)
{
  return map(value, 0, MOTOR_RPM, 0, 255);
}
