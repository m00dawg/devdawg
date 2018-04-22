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

void testMotor()
{
  drawDisplay("Testing", "Motor");
  while(true)
    controlMotor(255, 10);
}

