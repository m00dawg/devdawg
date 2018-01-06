// Dev Times
struct DevStep
{
  char* name;
  byte targetTemperature;
  unsigned int duration;
  byte color;
};

struct DevProcess
{
  char* name;
  byte motorSpeed;
  byte steps;
  DevStep devStep[8];
};
