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
//create an RF24 object
RF24 radio(7, 8);  // CE, CSN

//address through which two modules communicate.
const byte address[6] = "00001";
struct Signal {
byte throttle;
byte steer;
byte servoPos;
};

Signal data;
void setup()
{
  Serial.begin(9600);
  radio.begin();
  //initialize control over the keyboard:
  //set the address
  radio.openWritingPipe(address);
  
  //Set module as transmitter
  radio.stopListening();

  //OLED display setup   
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
  progress=progress+8;
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
  //Send message to receiver
  
  //
  int yAxis = analogRead(A0); 
  int xAxis = analogRead(A1);
  int value = analogRead(A3);
  int servoPos = map(value, 0, 1023, 0, 180);
  data.throttle = map(yAxis, 0, 1023, 15, 255);
  data.steer = map(xAxis, 0, 1023, 15, 255);
  data.servoPos= servoPos;
  //Transmitt
  radio.write(&data, sizeof(Signal));
  Serial.println(data.throttle);
  Serial.println(data.steer);
  //USER INTERFACE (UI)
  display.clearDisplay();
  display.setTextSize(0);
  //THROTTLE UI
  display.setCursor(0, 0);  // (x,y)
  display.println("Throttle: ");  // Text or value to print
  display.drawRect(50, 1, 78, 4, WHITE);
  display.fillRect(51, 2, data.throttle/3.282051282, 2, WHITE);
  //Steering UI 
  display.setCursor(0, 10);  // (x,y)
  display.println("Steer: ");  // Text or value to print
  display.drawRect(50, 13, 78, 4, WHITE);
  display.fillRect(51, 14, data.steer/3.282051282, 2, WHITE); 
  //Servo angle UI 
  display.setCursor(0, 20);  // (x,y)
  display.println("Cam: ");  // Text or value to print
  display.drawRect(50, 23, 55.05, 4, WHITE);
  display.fillRect(51, 24, servoPos/3.282051282, 2, WHITE);
  display.setCursor(110, 21);
  display.println(servoPos);
  //PRINT ALL
  display.display();
  delay(10); //least delay
}