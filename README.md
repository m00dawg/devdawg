DevDawg by Tim Soderstrom
=========================

Open source Arduino-based microcontroller for DIY film development projects.

Overview
--------

This started as a means to simply control the temperature of a water bath
for chemicals and turned into what is currently mostly just a code proof
of concept for what a more full featured controller would look like. 

The current code is mostly written for controlling the temperature of a 
water bath via PID control, keeping track of development steps, and 
controlling the speed and direction of a motor that could be used to make
a rotary. I had also thought about having it also control the state of
a circulation pump.

It is currently based off an Arduino Uno. I did that because such a design is 
very easy to convert to a purpose built board while still being DIY friendly
(using mostly through hole components). Right now I'm building it as a prototype
using shields and pre-built boards for things.

Used Libraries
--------------

  * RGB LCD Shield (https://github.com/adafruit/Adafruit-RGB-LCD-Shield-Library)
  * DB18S20 Temperature Control (https://github.com/milesburton/Arduino-Temperature-Control-Library)
  * Arduino PID Library (https://github.com/br3ttb/Arduino-PID-Library/)

Pin Map
-------

Uno:

  * I2C: A4, A5
  * DB18S20 Temperature Probe: D2
  * Buzzer: D3
  * Heater Relay: D4
  * Motor: D5 (PWM), D6, D7

Needed Parts
------------

Base + Temperature Control:

  * Arduino Uno
  * Adafruit RGB LCD Shield (https://learn.adafruit.com/rgb-lcd-shield)
  * DS18S20 Waterproof Temperature Sensor (e.g. https://www.adafruit.com/product/381)
  * 120V Relay (PowerSwitch Tail II is a nice safe choice)
  * Submersible Heater
  * Container for Water and Chemical Bottles, Film Tank (e.g. Ice Chest)

Rotary Processing:

  * DC Motor
  * H-Bridge Controller
  * A buncha 3D printed and metal and other parts I haven't figured out yet


Current State
-------------

Right now I'm looking for a heating element that can bring water up to at least
40C (104) but ideally up to 50C for being able to pre-heat water for mixing
chemicals as well. I've found a few options, some which seems sketch but basically
to keeps costs low, probably an AC submersible heating element that I can control
with a relay. The PID library can handle both relays and PWM options but for a 
decent amount of water, a simple relay may be enough (still need to test).

Otherwise currently the basic UI is implemented allowing one to select various
modes and development processes. It has routines to pre-heat the water bath and
run through a development cycle.

A dev cycle is a sequence of development steps to go through such as Development,
Blix, Wash, etc. The controller will track the remaining time while also keeping
the water bath at the desired temperature.

A partial implemetnation of routines to manage rotation speed and direction 
has been implemented, but is untested. I have a motor and H-bridge in the
mail to test at least the motor functionality, but building the rig will
take some time and design work. The code is pretty easy - the physical 
design work, not so much.

License
-------

Code is licensed Under the GPLv3. Please see LICENSE for more information.

TODO
----

Near-Term:

  * Find a heater element, test/tune PID
  * Finish Motor/Rotation Routine
  * Add a buzzer (for warnings and when dev steps finish)

Future Work:

  * Purpose built all in one controller board
  * Better heating solution
  * Fancy controller enclosure
  * Fancy chemcial bath, rotary solution
  * Somehow a means to automatically cool the water for BW?
