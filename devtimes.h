// Dev Times
struct DevStep
{
  char* name;
  byte targetTemperature;
  int time;
};

struct DevProcess
{
  char* name;
  byte motorSpeed;
  DevStep devStep[8];
};
