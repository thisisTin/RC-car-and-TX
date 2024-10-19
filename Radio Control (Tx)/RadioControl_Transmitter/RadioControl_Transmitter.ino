#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Adafruit_SSD1306.h>

const uint64_t pipeOut = 0xE9E8F0F0E1LL;   //IMPORTANT: The same as in the receiver 0xE9E8F0F0E1LL 
RF24 radio(7, 8); // select CE,CSN pin | 

struct Signal {
byte throttle;
byte pitch;
byte roll;
byte yaw;
};

Signal data;

void ResetData() 
{
data.throttle = 127; // Motor Stop (254/2=127)| 
data.pitch = 127; // Center 
data.roll = 127; // Center 
data.yaw = 127; // Center 
}

void setup()
{
//Start everything up

radio.begin();
radio.openWritingPipe(pipeOut);
radio.stopListening(); //start the radio comunication for Transmitter | 
ResetData();
}

// Joystick center and its borders | 

int mapJoystickValues(int val, int lower, int middle, int upper, bool reverse)
{
val = constrain(val, lower, upper);
if ( val < middle )
val = map(val, lower, middle, 0, 128);
else
val = map(val, middle, upper, 128, 255);
return ( reverse ? 255 - val : val );
}

void loop()
{
// Control Stick Calibration 
// Setting may be required for the correct values of the control levers. 

data.throttle = mapJoystickValues( analogRead(A0), 524, 524, 1015, true );
data.roll = mapJoystickValues( analogRead(A1), 12, 524, 1020, true );      // "true" or "false" for servo direction 
data.pitch = mapJoystickValues( analogRead(A2), 12, 524, 1020, true );     // "true" or "false" for servo direction 
data.yaw = mapJoystickValues( analogRead(A3), 12, 524, 1020, true );       // "true" or "false" for servo direction 

radio.write(&data, sizeof(Signal));
}