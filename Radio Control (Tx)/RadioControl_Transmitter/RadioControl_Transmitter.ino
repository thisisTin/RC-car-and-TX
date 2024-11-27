#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Servo.h> 
Adafruit_SSD1306 display;
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
int progress=0; 
Servo myservo;  

int servoPin = 6;       
const byte address[6] = "00001";
RF24 radio(7,8); // select CE,CS pin 

struct Signal {
byte throttle;
byte steer;
byte servoPos;
};

Signal data;

//Recalibrate Joystick to the center postition
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
radio.setDataRate(RF24_2MBPS); //Max transmitt speed
radio.setPALevel(RF24_PA_MAX); //Use maximum power amplification
radio.openWritingPipe(address);
radio.setChannel(108);  // Set the transmitting channel to 108
radio.stopListening(); //start the radio comunication for Transmitter | 
ResetData();

//Servo definitions
myservo.attach(servoPin); 

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
Serial.begin(9600);  //Set the baud rate to 9600

//Servo set
int value = analogRead(A3);// Đọc giá trị biến trở
    int servoPos = map(value, 0, 1023, 0, 180);
    myservo.write(servoPos);
    Serial.println(servoPos);
//Motor
    // Getting analog values of each Joystick Channel
    int yAxis = analogRead(A0); 
    int xAxis = analogRead(A1);
    Serial.println(yAxis);
    // Mapping analog value from 1-1023 to 0-255
    data.throttle = map(yAxis, 0, 1023, 0, 255);
    data.steer = map(xAxis, 0, 1023, 0, 255);
    data.servoPos= servoPos;
//     //Throttle and Steering
//   // Y-axis used for forward and backward control
//   if (yAxis < 470) {
//     // Set Motor A backward
//     digitalWrite(in1, HIGH);
//     digitalWrite(in2, LOW);
//     // Set Motor B backward
//     digitalWrite(in3, HIGH);
//     digitalWrite(in4, LOW);
//     // Convert the declining Y-axis readings for going backward from 470 to 0 into 0 to 255 value for the PWM signal for increasing the motor speed
//     motorSpeedA = map(yAxis, 470, 0, 0, 255);
//     motorSpeedB = map(yAxis, 470, 0, 0, 255);
//   }
//   else if (yAxis > 550) {
//     // Set Motor A forward
//     digitalWrite(in1, LOW);
//     digitalWrite(in2, HIGH);
//     // Set Motor B forward
//     digitalWrite(in3, LOW);
//     digitalWrite(in4, HIGH);
//     // Convert the increasing Y-axis readings for going forward from 550 to 1023 into 0 to 255 value for the PWM signal for increasing the motor speed
//     motorSpeedA = map(yAxis, 550, 1023, 0, 255);
//     motorSpeedB = map(yAxis, 550, 1023, 0, 255);
//   }
//   // If joystick stays in middle the motors are not moving
//   else {
//     motorSpeedA = 0;
//     motorSpeedB = 0;
//   }
// Serial.println("Motor 1:");
// Serial.println(motorSpeedA);
// Serial.println("Motor 2:");
// Serial.println(motorSpeedB);
// analogWrite(enA,motorSpeedA);
// analogWrite(enB,motorSpeedB);

    // if(data.steer>135)  // Steering to the right
    // {

    //     motorSpeedA = map(yAxis, 0, 1023, 0, 255);
    //     motorSpeedB = map(yAxis, 0, 1023, 0, 255);
    // }
    // else if (data.steer<100) //Steering to the left
    // {  
    //     motorSpeedA = map(yAxis, 0, 1023, 0, 255);
    //     motorSpeedB = map(yAxis, 0, 1023, 0, 255);
    // }
    // else  
    // {
    //     motorSpeedA = 0;
    //     motorSpeedB = 0;
    // }

//Transmit data
const char text[] = "Tinhuhu";
radio.write(&data, sizeof(Signal));
radio.write(&text, sizeof(text));
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
delay(0); //no delay
}