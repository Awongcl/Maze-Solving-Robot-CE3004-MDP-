// Motor file
#include <PID_v1.h>
#include <EnableInterrupt.h>
#include <MsTimer2.h>
#include <DualVNH5019MotorShield.h>

// const values
const int RIGHT_PULSE = 3;     //Pin for right encoder
const int LEFT_PULSE = 11;     //Pin for left encoder
const int defaultSpeed = 250;  
const int forSpeed = 310;      // Speed for forward motions
const int maxSpeed = 400;      // Speed for fastest path
const int turnSpeed = 300;     // Speed for turnning                   
double desiredDistanceSensor = 5.25;   // Distance away from the wall after calibration
float offset1 = 0;                     // Offsets for the front 3 sensors for calibration
float offset2 = 0;
float offset3 = 0;
int caliCounter = 0;           // Counts the number of calibrations

//PID values
double tick_R = 0;
double tick_L = 0;
double speed_O = 0;
double previous_tick_R = 0;
double previous_error = 0;

const double kp =  4, ki = 5, kd = 0.00;   // PID for 10 cm
const double kp2 = 18, ki2 = 8, kd2 = 0; // PID for above 10 cm 

//Ticks values 
int Ticks[11] = {549, 1150, 1800, 2380, 2950, 3610, 4165, 4685, 5295, 0,6}; // for speed 250 (Default speed)  // 554 for speed 310
int Tickss[11] = {550, 1150, 1750, 2380, 2950, 3570, 4165, 4735, 5325, 0,6}; // for speed 350 (Faster speed)
int Ticksss[11] = {550, 1150, 1750, 2380, 2950, 3570, 4165, 4735, 5325, 0,6}; // for speed 400 (Fastest speed)
const int LEFTTICK[18] = {200, 200, 100, 0, 200, 0, 500, 55, 500, 65, 550, 75, 600, 85, 680,0, 730, 500};   // 10 =2, 20 = 4, 30 = 6, 40 = 8
const int RIGHTTICK[14] = {20, 25, 30, 35, 40, 313, 50, 55, 450, 65, 70, 75, 80, 85};
int TURN_TICKS_L = 760;   // 90 = 759
int TURN_TICKS_R = 753;  //   90 == 756

// Define PID function, by default left wheel is faster, input the slower left wheel for PID to match the faster wheel
DualVNH5019MotorShield md;
PID myPID(&tick_R, &speed_O, &tick_L, kp, ki, kd, REVERSE);   // This is for testing
PID myPID2(&tick_R, &speed_O, &tick_L, kp2, ki2, kd2, REVERSE);   // This is used

// M1 is left
// M2 is right

void moveForward(int distancee) {
  initializeTick();
  initializeMotor_Start();
  int distance = 0;
  double currentSpeed = 0;
  if (distancee == 1) {
    currentSpeed =  forSpeed;   
     distance = Ticks[distancee-1];
  } else {
    if(godMode || distancee == 11){
      currentSpeed = maxSpeed;
     distance = Tickss[distancee-1];    // Ticks for fastest speed
    }
    else{
      currentSpeed = defaultSpeed;
     distance = Ticks[distancee-1];       // Ticks for normal speed
    }
  }
  while (tick_R <= distance || tick_L <= distance) {
   if (myPID2.Compute())
        md.setSpeeds(currentSpeed + 2*speed_O, currentSpeed);
  }
  initializeMotor_End();
 
}

void moveBack(){
  initializeTick();
  initializeMotor_Start();
  int distance = 7;
  double currentSpeed = 0;
  currentSpeed = maxSpeed;   
  while (tick_R <= distance || tick_L <= distance) {
        if(myPID2.Compute())
           md.setSpeeds(-(currentSpeed + 2*speed_O), -(currentSpeed));
  }
  initializeMotor_End();
}

void turnLeft(int angle) {
  int i=0;
  double currentSpeed = turnSpeed;
  if (angle >= 90) {
    for (i = 0; i<angle; i+=90) {
      initializeTick();
      initializeMotor_Start();
      while (tick_R < TURN_TICKS_L || tick_L < TURN_TICKS_L) {
        if (myPID2.Compute())
          md.setSpeeds(-(currentSpeed + 2*speed_O), currentSpeed - 2*speed_O);
      }
      initializeMotor_End();
    }
  }
  if (angle - i > 0) {
    turnLeftDeg(angle-i);
  }
  initializeMotor_End();
  delay(50);
}
// For checkList
void turnLeftDeg(int angle) {
  int index = (angle-20)/5;
  int tick;
  if (index <= 14)
    tick = LEFTTICK[index];
    
  initializeMotor_Start();
  double currentSpeed = turnSpeed;
  initializeTick();
  while (tick_R < tick || tick_L < tick) {
    if (myPID.Compute())
      md.setSpeeds(-(currentSpeed + speed_O), currentSpeed - speed_O);
  }
  initializeMotor_End();
}
    
void turnRight(int angle) {
  int i=0;
  double currentSpeed = turnSpeed;
   if (angle >= 90){
    for (i=0; i<angle; i=i+90){
      initializeMotor_Start();
      initializeTick();
      while (tick_R < (TURN_TICKS_R) || tick_L < (TURN_TICKS_R)) {
        if (myPID2.Compute())
          md.setSpeeds((currentSpeed + 2*speed_O), -(currentSpeed - 2*speed_O));
      }
    }
  }
  initializeMotor_End();
  delay(50);
}

void setupMotorEncoder() {
  md.init();
  pinMode(LEFT_PULSE, INPUT);
  pinMode(RIGHT_PULSE, INPUT);
  enableInterrupt(LEFT_PULSE, leftEncoder, CHANGE);
  enableInterrupt(RIGHT_PULSE, rightEncoder, CHANGE);
}

void stopMotorEncoder() {
  disableInterrupt(LEFT_PULSE);
  disableInterrupt(RIGHT_PULSE);
}

void initializeMotor_Start() {
  md.setSpeeds(0, 0);
  md.setBrakes(0, 0);
}

void initializeMotor_End() {
  md.setSpeeds(0, 0);
  md.setBrakes(384, 384); //L//R
  delay(50);
}

void initializeTick(){
  tick_R = 0;
  tick_L = 0;
  speed_O = 0;
  previous_tick_R = 0;
}


void setupPID() {
  myPID.SetMode(AUTOMATIC);
  myPID.SetOutputLimits(-400, 400);
  myPID.SetSampleTime(5);

  myPID2.SetMode(AUTOMATIC);
  myPID2.SetOutputLimits(-400, 400);
  myPID2.SetSampleTime(5);
}

void leftEncoder() {
  tick_L++;
}

void rightEncoder() {
  tick_R++;
}

float getLeftRpm(){
  double pulse = pulseIn(LEFT_PULSE,HIGH);
  return ((1/(pulse*2))*(pow(10,6))*60)/562.25;
}
float getRightRpm(){
  double pulse = pulseIn(RIGHT_PULSE,HIGH);
  return ((1/(pulse*2))*(pow(10,6))*60)/562.25;
}


void rotateRight(int distance, int direct) {
  initializeTick();
  initializeMotor_Start();
  double currentSpeed = maxSpeed;

  while (tick_R < distance) {
    if (myPID2.Compute())
      md.setSpeeds(0, direct*(currentSpeed - 2*speed_O));
  }
  initializeMotor_End();
}

void rotateLeft(int distance, int direct) {
  initializeTick();
  initializeMotor_Start();

  double currentSpeed = maxSpeed;
  while (tick_L < distance) {
    if (myPID2.Compute())
      md.setSpeeds(direct*(currentSpeed - 2*speed_O), 0);
  }
  initializeMotor_End();
}

// Most important function to save the bot !
void calibrate(){
  caliCounter = 0; // Here the if statements will get the location of the obstacles, flat wall or staggered
  if(getir1Grid(getir1()) == 1 && getir2Grid(getir2()) == 1 &&getir3Grid(getir3()) == 1){   // Flat wall
    Serial.println("Flat wall calibrating...");
    offset1 = 0; offset3 = 0;
    caliFlat();
  }
  else if(getir1Grid(getir1()) == 1 && getir2Grid(getir2()) == 1 && getir3Grid(getir3()) != 1 ){  // Right 2
    offset1 = 0 ; offset2 = 0;
    Serial.println("Right 2 wall calibrating...");
    caliRight2();
  }
  else if(getir1Grid(getir1()) != 1 && getir2Grid(getir2()) == 1 && getir3Grid(getir3()) == 1){   // Right close left far
    offset1 = 0; offset3 = 0;
    Serial.println("Left 2 wall calibrating...");
    caliLeft2();
  }
  else{
    Serial.println("Flat wall calibrating...");
    offset1 = 0;offset3 = 0;
    caliFlat();
  }

   
 Serial.println("CALIDONE"); 
  
}

void caliFlat(){
  offset3 =0.1;
  moveBack();
  while(getir3() > 7 || getir1() > 7)
    moveForward(11);
  while(getir1()-offset1 < 5.25 || getir3()-offset3 < 5.25)   // Move back to desire distance
    moveBack(); 
  delay(50);
  int count = 0;
  double diffLeft = (getir1()-offset1) - desiredDistanceSensor;
  double diffRight = (getir3()-offset3) - desiredDistanceSensor;
  
  while(getir1()-offset1 > desiredDistanceSensor ||  getir2()-offset2 > desiredDistanceSensor || getir3()-offset3 > desiredDistanceSensor){
    //Serial.print("Calibrating...");
    //Serial.println(count+1);
    if (getir3()-offset3 > getir1()-offset1){
      rotateLeft(abs(diffLeft*5), 1);
    }
    else if (getir1()-offset1 > getir3()-offset3) {
      rotateRight(abs(diffRight*5), 1);
    }
    count++;
    if (count >= 17)
      break;
  }
  delay(50);
  //checkCali();
}

void caliRight2(){
  while(getir1() > 5){
      moveForward(11);
    }
  while(getir2()-offset2 < 5.3 || getir1()-offset1 < 5.3)        // Move back to desire distance
    moveBack(); 
  delay(50);
  int count = 0;
  double diffLeft = (getir2()-offset2) - desiredDistanceSensor;
  double diffRight = (getir1()-offset1) - desiredDistanceSensor;
  
  while(getir1()-offset1 > desiredDistanceSensor ||  getir2()-offset2 > desiredDistanceSensor){
    if (getir1()-offset1 < getir2()-offset2){
      rotateLeft(abs(diffLeft*5), 1);
    }
    else if (getir2()-offset2 < getir1()-offset1) {
      rotateRight(abs(diffRight*5), 1);
    }
    count++;
    if (count >= 16)
      break;
  }
}
void caliLeft2(){
  offset3 = 0.1;
  while(getir2() > 5)
     moveForward(11);
  while(getir2()-offset2 < 5.3 || getir3()-offset3 < 5.3)        // Move back to desire distance
    moveBack(); 
  delay(50);
  int count = 0;
  double diffLeft = (getir2()-offset2) - desiredDistanceSensor;
  double diffRight = (getir3()-offset3) - desiredDistanceSensor;
  
  while(getir3()-offset3 > desiredDistanceSensor ||  getir2()-offset2 > desiredDistanceSensor){
    //Serial.print("Calibrating...");
    //Serial.println(count+1);
    if (getir3()-offset3 > getir2()-offset2){
      rotateLeft(abs(diffLeft*5), 1);
    }
    else if (getir2()-offset2 > getir3()-offset3) {
      rotateRight(abs(diffRight*5), 1);
    }
    count++;
    if (count >= 16)
      break;
  }   
}

void checkCali(){
  int count = 0;
  Serial.println("Checking Cali result...");
  Serial.println(abs(getir1()-offset1 - getir3()-offset3));
  if(abs(getir1()-offset1 - getir3()-offset3) > 0.7){
    Serial.println("Re-calibrating");
    double diffLeft = (getir1()-offset1) - desiredDistanceSensor;
    double diffRight = (getir3()-offset3) - desiredDistanceSensor;
    while(getir1() < desiredDistanceSensor ||  getir3() < desiredDistanceSensor){
      //Serial.print("Calibrating...");
      //Serial.println(count+1);
      if (getir3()-offset3 < getir1()-offset1){
        rotateLeft(abs(diffLeft*5), -1);
      }
      else if (getir1()-offset1 < getir3()-offset3) {
        rotateRight(abs(diffRight*5), -1);
      }
      count++;
      if (count >= 17)
        break;
    }
  }
}
void rightAvoid(int m){
  if(m != '1')
    return;
  if(getir5() < 4.5){
    turnRight(90);
    delay(50);
    calibrate();
    delay(510);
    turnLeft(90);
  }
}
