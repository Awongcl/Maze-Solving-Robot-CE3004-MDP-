
//Sensor file
#include <SharpIR.h>
#include <RunningMedian.h>
#include <timer.h>

#define IR1 A0 // define signal pin
#define IR2 A1 // define signal pin
#define IR3 A2 // define signal pin
#define IR4 A3 // define signal pin
#define IR5 A4 // define signal pin
#define IR6 A5 // define signal pin
#define model 1080 
/*ir: the pin where your sensor is attached
10 to 80 cm GP2Y0A21YK0F  use 1080
20 to 150 cm GP2Y0A02YK0F use 20150
*/
SharpIR ir1(IR1, model);
SharpIR ir2(IR2, model);
SharpIR ir3(IR3, model);
SharpIR ir4(IR4, model);
SharpIR ir5(IR5, model);
SharpIR ir6(IR6, 20150);  // Long sensor

const int NUM_SAMPLES_MEDIAN = 19;
double ir1_value = 0,ir2_value = 0,ir3_value = 0,ir4_value = 0,ir5_value = 0,ir6_value = 0;

// readir functions returns the calibrated values from the analog values
// getir functions returns the running median values using readir values
// getirGrid functions reading the Grid distance according to getir values

// get the rough ditance from analog read
double readir1(){
  return abs((1/(0.00028022854859461900*analogRead(A0) - 0.02089061175704690000)) + 1);
}
double readir2(){
  return abs((1/(0.00022119058193341900*analogRead(A1) - 0.00658566742009741000))+2.8);
}
double readir3(){
  return abs((1/(0.00019928046606508800*analogRead(A2) - 0.00164954339594651000)) - 1);
}
double readir4(){
  return abs((1/(0.00027527279026935100*analogRead(A3) - 0.01950769978960690000)) + 1);
}
double readir5(){
  return abs((1/(0.00023043640601837300*analogRead(A4) - 0.00692230909424498000)) + 0.1);
}
double readir6(){
  return abs((1/(0.00003046459955539680*analogRead(A5) + 0.00468468858653021000)) - 30); 
}

// Using running median get more accurate distance for each sensors

double getir1() {
  RunningMedian median = RunningMedian(NUM_SAMPLES_MEDIAN);
  for (int n = 0; n < NUM_SAMPLES_MEDIAN; n++) {
    double irDistance = readir1();   
    median.add(irDistance);
    if (median.getCount() == NUM_SAMPLES_MEDIAN) {
      if (median.getHighest() - median.getLowest() > 30)
        return 30;
      ir1_value = median.getMedian();
    }
  }
  if(ir1_value < 9.5)
    return ir1_value - 3.69;
  else if (ir1_value >= 9.5 && ir1_value < 20)
    return ir1_value - 3.22;
  return ir1_value - 4.5; //offset
}
double getir2() {
  RunningMedian median = RunningMedian(NUM_SAMPLES_MEDIAN);
  for (int n = 0; n < NUM_SAMPLES_MEDIAN; n++) {
    double irDistance = readir2();   
    median.add(irDistance);
    if (median.getCount() == NUM_SAMPLES_MEDIAN) {
      if (median.getHighest() - median.getLowest() > 30)
        return 30;
      ir2_value = median.getMedian();
    }
  }
  if(ir2_value < 11)
    return ir2_value - 5.47;
  return ir2_value - 4.61;
}
double getir3() {
  double t;
  RunningMedian median = RunningMedian(NUM_SAMPLES_MEDIAN);
  for (int n = 0; n < NUM_SAMPLES_MEDIAN; n++) {
    double irDistance = readir3();   
    
    median.add(irDistance);
    if (median.getCount() == NUM_SAMPLES_MEDIAN) {
      if (median.getHighest() - median.getLowest() > 30)
        return 30;
       ir3_value = median.getMedian();
    }
  }
  if(ir3_value < 9.5)
    return ir3_value - 3.5;
  return ir3_value -3.25;
}
double getir4() {
  RunningMedian median = RunningMedian(NUM_SAMPLES_MEDIAN);
  for (int n = 0; n < NUM_SAMPLES_MEDIAN; n++) {
    double irDistance = readir4();   
    median.add(irDistance);
    if (median.getCount() == NUM_SAMPLES_MEDIAN) {
      if (median.getHighest() - median.getLowest() > 30)
        return -1;
      ir4_value = median.getMedian();
    }
  }
  return ir4_value -4.3;
}
double getir5() {
  RunningMedian median = RunningMedian(NUM_SAMPLES_MEDIAN);
  for (int n = 0; n < NUM_SAMPLES_MEDIAN; n++) {
    double irDistance = readir5();   
    median.add(irDistance);
    if (median.getCount() == NUM_SAMPLES_MEDIAN) {
      if (median.getHighest() - median.getLowest() > 30)
        return -1;
      ir5_value = median.getMedian();
    }
  }
  return ir5_value -4.6;
}
double getir6() {
  RunningMedian median = RunningMedian(NUM_SAMPLES_MEDIAN);
  for (int n = 0; n < NUM_SAMPLES_MEDIAN; n++) {
    double irDistance = readir6();   
    median.add(irDistance);
    if (median.getCount() == NUM_SAMPLES_MEDIAN) {
      if (median.getHighest() - median.getLowest() > 30)
        return -1;
      ir6_value = median.getMedian();
    }
  }
  if(ir6_value < 35)
    return ir6_value -3.7;
  else if(ir6_value >= 35 && ir6_value <= 60)  
    return ir6_value -5.2 ;
  else if(ir6_value > 60 && ir6_value <= 65 )  
    return ir6_value -4 ; 
  else if(ir6_value > 65 && ir6_value <= 70 )  
    return ir6_value -1.5 ;
  else if(ir6_value > 70 && ir6_value <= 75 )  
    return ir6_value +2 ;  
  else if(ir6_value > 75 && ir6_value <= 80 )  
    return ir6_value +3 ;
  else
    return ir6_value +2 ;
}
// Convert sensor data into grid values

int getir1Grid(double range){
  if(range > 2.5 && range <= 9.2)
    return 1;
   else if (range > 9.2 && range <= 20)
    return 2; 
   else return -1;
}
int getir2Grid(double range){
  if(range > 2.5 && range <= 9.2)
    return 1;
   else if (range > 9.2 && range <= 19.5 && getir1() > 4.2 && getir3() > 4.2)  // Real 2 blocks away
    return 2;
   else if (range > 9.2 && range <= 19.5 && getir1() > 4.2 && getir3() > 4.2)  // Real 2 blocks away
    return 2;
   else if (range > 9.2 && range <= 19.5 && getir1() < 4.2 && getir3() < 4.2)  // When it is too close to the obstacle reading will go up, check 1 and 3
    return 1;
   else 
    return -1; 
}
int getir3Grid(double range){
  if(range > 2.5 && range <= 9.2)
    return 1;
   else if (range > 9.2 && range <= 20)
    return 2;
   else 
    return -1; 
}
int getir4Grid(double range){
  if(range > 2.5 && range <= 11)
    return 1;
   else if (range > 11 && range <= 19)
    return 2;
   else 
    return -1; 
}
int getir5Grid(double range){
  if(range > 2.5 && range <= 11)
    return 1;
   else if (range> 11 && range <= 19)
    return 2;
   else 
    return -1; 
}
int getir6Grid(double range){
  if(range > 19.5 && range  <= 32.1)
    return 3;
   else if ( range   > 32.1 && range  <= 41)
    return 4;
   else 
    return -1; 
}

// Send data to Rpi
void sendGridData(){
  // "SDATA:SRFL:SRFC:SRFR:SRL:SRR:SRL" FORMAT
  delay(2);
  String s = "SDATA:"+String(getir3Grid(getir3()))+":"+String(getir2Grid(getir2()))+":"+
              String(getir1Grid(getir1()))+":"+String(getir4Grid(getir4()))+":"+
              String(getir5Grid(getir5()))+":"+String(getir6Grid(getir6()));
  Serial.println(s);

}

// Function to read all sensors for monitoring
void readAllSensors(){
  Serial.print(analogRead(A0));
  Serial.print(",");
  Serial.print(analogRead(A1));
  Serial.print(",");
  Serial.print(analogRead(A2));
  Serial.print(",");
  Serial.print(analogRead(A3));
  Serial.print(",");
  Serial.print(analogRead(A4));
  Serial.print(",");
  Serial.print(analogRead(A5));
  Serial.print(",  ||  ");
  Serial.print(readir3());
  Serial.print(",");
  Serial.print(readir2());
  Serial.print(",");
  Serial.print(readir1());
  Serial.print(",");
  Serial.print(readir4());
  Serial.print(",");
  Serial.print(readir5());
  Serial.print(",");
  Serial.print(readir6());
  Serial.print(", ||  ");
  Serial.print(getir3());
  Serial.print(",");
  Serial.print(getir2());
  Serial.print(",");
  Serial.print(getir1());
  Serial.print(",");
  Serial.print(getir4());
  Serial.print(",");
  Serial.print(getir5());
  Serial.print(",");
  Serial.println(getir6());

  
}
