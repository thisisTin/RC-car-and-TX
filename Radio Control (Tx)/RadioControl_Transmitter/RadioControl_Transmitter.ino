#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
Adafruit_SSD1306 display;
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
int progress=0; 
const uint64_t pipeOut = 0xE9E8F0F0E1LL;   //IMPORTANT: The same as in the receiver 0xE9E8F0F0E1LL 
RF24 radio(7, 8); // select CE,CSN pin | 

struct Signal {
byte throttle;
byte steer;
};

Signal data;

void ResetData() 
{
data.throttle = 127; // Motor Stop (254/2=127)| 
data.steer = 127; // Center 
}

void setup()
{
//Start everything up
Serial.begin(9600);
radio.begin();
radio.openWritingPipe(pipeOut);
radio.stopListening(); //start the radio comunication for Transmitter | 
ResetData();
//OLED   
display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // Initialize display with the I2C address of 0x3C
display.clearDisplay();  // Clear the buffer
display.setTextColor(WHITE);  // Set color of the text
display.setRotation(0);  // Set orientation. Goes from 0, 1, 2 or 3
display.setTextWrap(false);  // By default, long lines of text are set to automatically “wrap” back to the leftmost column.
                               // To override this behavior (so text will run off the right side of the display - useful for
                               // scrolling marquee effects), use setTextWrap(false). The normal wrapping behavior is restored
                               // with setTextWrap(true).
display.dim(0);  //Set brightness (0 is maximun and 1 is a little dim)
 
display.clearDisplay();
// LOADING PROCEDURE
display.setTextSize(0);
display.setTextColor(WHITE);
display.setCursor(31, 12);
display.print("TX Loading..");
for (progress=0;progress<=127;progress++)
{
  progress=progress+4;
  display.drawRect(0, 127, 78, 6, WHITE);
  display.setCursor(0, 17);
  display.fillRect(1, 25, progress, 5, WHITE);
  display.display();
  if(progress==127)
  {
    loop();
  }
}
display.display();
}

void loop()
{
// Control Stick Calibration 
// Setting may be required for the correct values of the control levers. 
//UI
display.clearDisplay();
display.setTextSize(0);
//TX PROFILE
display.setCursor(30, 0);  // (x,y)
display.println(" RC car TX");
data.throttle = map( analogRead(A0), 0, 1023, 0, 255 );
data.steer = map( analogRead(A1), 0, 1023, 0, 255 );     

radio.write(&data, sizeof(Signal));

//THROTTLE UI
display.setCursor(0, 10);  // (x,y)
display.println("Throttle: ");  // Text or value to print
display.drawRect(50, 12, 78, 4, WHITE);
display.fillRect(51, 13, data.throttle/3.282051282, 2, WHITE);
//Steering UI 
display.setCursor(0, 20);  // (x,y)
display.println("Steer: ");  // Text or value to print
display.drawRect(50, 21, 78, 4, WHITE);
display.fillRect(51, 22, data.steer/3.282051282, 2, WHITE); 
//PRINT ALL
display.display();

}