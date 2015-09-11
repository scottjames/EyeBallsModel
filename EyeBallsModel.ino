

/*
* Arduino Pan & Tilt with Wii Nunchuk
* http://www.jangeox.be/2014/02/arduino-pan-tilt-met-wii-nunchuck.html
* 
*
*
* This sample code uses the library wiinunchuck.h :
*
* File  : wiinunchuk.h V0.9
* Author: Tim Teatro
* Date  : Feb 2012
* http://www.timteatro.net/2012/02/10/a-library-for-using-the-wii-nunchuk-in-arduino-sketches/
*
* download the wiinunchuk.h file in your libraries folder of the Arduino environment
*    (something like C:\arduino-1.0.4\libraries\wii , where you first create a folder 'wii' yourself)
*
* I tried two methods to drive the servo's of the pan-tilt system
* METHOD 1: use joystick of the Nunchuk controller
* METHOD 2: use the sensors of the Nunchuk controller
* The first one is more accurate
* The second one is more fun ;-)
*
* This code has method 2 commented out, so it acts as method 1
* To try method 2 : comment out the sections marked with 'Method 2' and put exisiting sections 'Method 1' in comment
*
* The Wii nunchuck uses 3.3V supply from your Arduino Board
* The data and clock MUST be connected to the I2C interface pins (A4 = data and A5 = clock on Arduino Uno)
* There's a WiiChuck adapter available here
* http://todbot.com/blog/2008/02/18/wiichuck-wii-nunchuck-adapter-available/
*
* or try it yourself using a double sided experiment board (works fine, see my blog)
*
* this code compiles to approx 5kb so it works fine Atmega8 MCUs
*
*/

#include <Wire.h>
#include <Servo.h> 
#include <WiiNunchuck.h>


// drive 2 servos (tilt/pan) for each of two eyeballs in model
Servo pan0; 
Servo tilt0;
Servo pan1; 
Servo tilt1;

int pinPan0  = 10;
int pinTilt0 = 11;
int pinPan1  = 12;
int pinTilt1 = 13;


int panPos = 70;
int tiltPos = 80;
int panPosDelay;
int tiltPosDelay;

int speedPan;
int speedTilt;

void setup(){

    delay(100);
    
  // start serial port at 9600 bps:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
    
    

// METHOD 1: use joystick of the Nunchuk controller
  panPosDelay = panPos * 10;
  tiltPosDelay = tiltPos * 10;
  
// METHOD 2: use the sensors of the Nunchuk controller
/*
  panPosDelay = panPos;
  tiltPosDelay = tiltPos;
*/
    delay(100);
    
  nunchuk_init();
  
  delay(50);
 
  nunchuk_calibrate_joy();
  
}

void loop(){
  
/* METHOD 1: use joystick of the Nunchuk controller
   The movement of the joystick determines the speed of the camera movement. 
   So the camera does NOT follow the position of the joystick, 
     it just moves in the desired direction at a slower/faster speed 
     determined by how far the joystick is pushed.
   In both methods, you need to press the Z button to move the camera
   If you release the joystick, the camera holds its position
*/

  nunchuk_get_data(); // receives 6 data bytes from controller
//  if (nunchuk_zbutton() == 1) {
  
    // attach servos if not done yet
    if (pan0.attached() == 0){pan0.attach(pinPan0);};
    if (tilt0.attached() == 0){tilt0.attach(pinTilt0);};
    if (pan1.attached() == 0){pan1.attach(pinPan1);};
    if (tilt1.attached() == 0){tilt1.attach(pinTilt1);};

    delay(5);     
  
  // TODO just use map() to translate joystick 0-255 --> 0-1024 (servo output)
  
    // speedPan and speedTilt will vary from approx -10 .. 10
    speedPan =  (130 - nunchuk_cjoy_x()) / 10;
    speedTilt =  (130 - nunchuk_cjoy_y()) / 10;

    // following code ignores small tilt movements, 
    //   I found it much better to control
    if ((speedTilt <= 2) & (speedTilt >= -2)) {speedTilt = 0;};
    if (speedTilt > 2){speedTilt -= 2;};
    if (speedTilt < -2){speedTilt += 2;};    
    
    // calculate position of the servos with speed 
    if ((speedPan != 0) or (speedTilt != 0)){
      panPosDelay += speedPan;
      tiltPosDelay += speedTilt;    
    }
    
    //SAJ added clamp values to range of 0 - 2000
    if ( panPosDelay < 0 ) panPosDelay = 0;
    if ( panPosDelay > 2000 ) panPosDelay = 2000;
    if ( tiltPosDelay < 0 ) tiltPosDelay = 0;
    if ( tiltPosDelay > 2000 ) tiltPosDelay = 2000;
  
    // write data to servos
    pan0.write(panPosDelay / 10);
    tilt0.write(tiltPosDelay / 10);
    pan1.write(panPosDelay / 10);
    tilt1.write(tiltPosDelay / 10);
    
// output servo for testing
Serial.print("POS=");
Serial.print(panPosDelay,DEC);
Serial.print(" ");
Serial.println(tiltPosDelay,DEC);

//  }
  
  delay(1);  
    
  //return;
  
  


/* METHOD 2: use the sensors of the Nunchuk controller
   The position of the controller determines the position of the camera. 
   In both methods, you need to press the Z button to move the camera
   If you release the joystick, the camera holds its position
   VERY IMPORTANT NOTE: 
   1.the servos are 'detached' when you release the button.
     If your camera is heavy, this may mean that it tilts to one position
     because the servos don't hold it in place.
     I only put this code in to save battery power
     When attached to mains supply or you're not worried about battery life, 
     you could prefer to delete the lines :
          pan.detach();
          tilt.detach();
     in the code below
   2.Movement of the servos is very fast. 
     Testing it without camera attached would be very good practice
     
*/

 
/*  nunchuk_get_data();
  if (nunchuk_zbutton() == 1){

  panPos = 160 - (nunchuk_rollangle() + 90);
  if (panPos < 0){panPos = 0;};
  if (panPos > 140){panPos = 140;};

  tiltPos = 160 - (nunchuk_pitchangle() + 90);
  if (tiltPos < 48){tiltPos = 48;};
  if (tiltPos > 140){tiltPos = 140;};
  
  if (pan.attached() == 0){pan.attach(pinPan);};
  if (tilt.attached() == 0){tilt.attach(pinTilt);};

  int del = 30 - abs (panPos - panPosDelay);
  if (del < 1){del=1;};
  int del2 = 30 - abs (tiltPos - tiltPosDelay);
  if (del2 < 1){del2=1;};
  if (del2 < del){del = del2;};
  delay(del);     

  if (panPos > panPosDelay){panPosDelay++;};
  if (panPos < panPosDelay){panPosDelay--;};

  if (tiltPos > tiltPosDelay){tiltPosDelay++;};
  if (tiltPos < tiltPosDelay){tiltPosDelay--;};
  
  pan.write(panPosDelay);
  tilt.write(tiltPosDelay);

  }
  else
  {  
   if (!(pan.attached() == 0)){

     do {
       if (panPosDelay < 70){panPosDelay ++;};
       if (panPosDelay > 70){panPosDelay --;};
       if (tiltPosDelay > 80){tiltPosDelay --;};
       if (tiltPosDelay < 80){tiltPosDelay ++;};
       pan.write(panPosDelay);
       tilt.write(tiltPosDelay);
       delay(5);
     }
     while ((panPosDelay != 70) & (tiltPosDelay != 80)) ;
    
     panPosDelay = 70;
     tiltPosDelay = 80;

     delay(100);  
     pan.detach();
     tilt.detach();

     }
  }
  
  delay(1);  
    
  return;
 */
  
 
}

