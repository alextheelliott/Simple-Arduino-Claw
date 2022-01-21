/* APSC 160 Claw Control
 by Alex Elliott

 16/01/2022
*/

#include <Servo.h>
#include <NewPing.h>

#define ServoPin 9 //servo pin of servo
Servo priServo;    //create servo object to control a servo

#define TrgPin 12  // sonar trigger pin will be attached to Arduino pin 12
#define EchPin 11  // sonar echo pint will be attached to Arduino pin 11
#define GrdPin 10
#define MaxDis 200 // fmaximum distance set to 200 cm
NewPing sonar(TrgPin, EchPin, MaxDis); // initialize NewPing

#define PriPos 60  //default position, not gripped
#define SecPos 120 //gripped position
bool isInPriPos = true;

#define DistanceToFloor 15 //in cms, overestimate

/*------------------------------------------------------------------------------------------------------------*/

void toggleServo() {
  priServo.write(isInPriPos ? SecPos : PriPos);
  isInPriPos = !isInPriPos;
}

bool isTouchingFloor(int de) { 
  delay(de/2);
   
  int distance = sonar.ping_cm();
  Serial.println(distance);
  
  delay(de/2);

  //if the distance is close enough, return true
  return (distance <= DistanceToFloor) ? (distance > 0 ? true : false) : false;
}

bool countToTimeTouchingGround(int t) { //t in milis
  int loopDelay = 1000; //delay between loops in milis
  int count = (t)/loopDelay;
  
  for (int i = 0; i < count; i++) {
    if (!isTouchingFloor(loopDelay/2)) { //if the claw is no longer on the floor the method returns, resetting the count
      delay(loopDelay/2); //delay between each loop = variable loopDelay in miis
      return false;
    } else {
      delay(loopDelay/2); //delay between each loop = variable loopDelay in milis
    }
  }
  return true;
}

/*------------------------------------------------------------------------------------------------------------*/

void setup() {
  Serial.begin(9600);
  
  priServo.attach(ServoPin); //attaches the servo on pin 9 to the servo object 
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(EchPin, INPUT) ;
  pinMode(TrgPin, OUTPUT) ;
  pinMode(GrdPin, OUTPUT);    //tell pin 10 it is going to be an output

  priServo.write(PriPos); //sets to original point
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(GrdPin, LOW);  // tell pin 10 to output LOW (OV, or ground)
}

void loop() {
  if (countToTimeTouchingGround(3000)) {
    toggleServo();
  }
  digitalWrite(LED_BUILTIN, HIGH); //blink on built-in LED when attempt failed
  delay(100); //delay between trials
  digitalWrite(LED_BUILTIN, LOW); //blink off built-in LED for new attempt
}
