// Dev Times

/* Specific development step part of a larger development process
 *  
 *  name: Name of the step
 *  targetTemeprature: Desired temperature for the duraction of the step
 *  duration: How many seconds this step takes
 *  color: Color of the LCD for the step.
 */
struct DevStep
{
  char* name;
  unsigned int duration;
  byte color;
};

/* Full development process, which consists of the name, an array of dev steps and various setup info.
 *  
 *  name: Name of the process, e.g C-41
 *  motorSpeed: PWM value for the speed of the motor
 *  motorDirectionInterval: How many seconds between changing direction of rotation
 *  steps: How many total development steps for the process (e.g. Dev, Stop, Fix would be 3)
 *  DevStep: An array of DevStep structs. Note uses a const for the array size.
 */
struct DevProcess
{
  char* name;
  byte motorSpeed;
  byte motorDirectionInterval;
  byte targetTemperature;
  byte steps;
  DevStep devStep[MAX_DEV_STEPS];
};
