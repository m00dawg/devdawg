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

