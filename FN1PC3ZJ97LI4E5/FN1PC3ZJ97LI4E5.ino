/* Compression Tester
Originally based on and distributed by John Doss 2017 as Rotary Compression Tester
Further modified by Steven Critchfield 2021 as a Rotary and piston compression tester

This code assumes you're using a 0-200 psi - 0.5-4.5 vdc pressure transducer connected to the AI0 pin.
You can use sensors with other pressure or voltage ranges but you'll need to modify the code.
*/

#define SENSOR A0          //Analog input pin that sensor is connected too

int i;                    //counting faces
int firstrun=0;           //first time through test
int style=0;              // what style compression testing

unsigned long time;       //time for calc RPM
unsigned long OldTime;    //OldTime for calc RPM
unsigned long RPM;        //RPM variable

int pointer = 0;          // which buffer to use
int store[3];             // storage
int peakStore[3];         // peak values
char buf[12];             //setting up arrays for compression and rpm data

void setup() {
  for( i=0; i<3; i++){
    peakStore[i] = 0;
  }

  Serial.setTimeout(5000);
  Serial.begin(19200);      //serial speed
  delay(500);               //hang out for half a second
  
  Serial.println("      Open-Source");
  Serial.println("Engine Compression Tester");
  Serial.println("");
  
  while(style < 1){
    detectSetup();
  }
}

void detectSetup(){
  Serial.println("What style do you want to test for?");
  Serial.println("1. Rotary engine");
  Serial.println("2. Piston Engine");
  Serial.print("? ");
  style = readOptions();
}

int readOptions(){
  while(Serial.available() < 1){
    delay(100);
  }
  int incoming = Serial.read();
  if(incoming == 49 ){
    style = 1;
  }
  if(incoming == 50){
     style = 2;
  }
  Serial.println(style);
  return style;
}

float readSensor(){
  // Any special math should be done here.
  // 0psi = .5v
  // 200psi = 4.5v
  // Range is 0-1023
  float ret = (readPort()-98)*200/818;
  Serial.println(ret);
  delay(500);
  return ret;
}

float readPort(){
  return analogRead(SENSOR);
}

float readTillPeak(){
  int v, peak;
  peak = 0;
  v = readSensor();
  if (v < 0){v = 0;}
  while ((peak-v)<=5)
  {
    if(peak<v) peak=v;
    v = readSensor();
    if (v < 0){v = 0;}
  }   
  
  return peak;
}

void readTillLow(){
  int v, minimum;
  minimum = readSensor();
  // stupid stub amount
  v = minimum + 10;
  while((v-minimum)<5){
    v = readSensor();
    if (v < 0){v = 0;}
    if (minimum>v) minimum=v;
  }
}

void loop(){
  OldTime=millis();           //record cycle begining time for RPM calculation
  if(style == 1){
    // Rotary style
    rotaryCheck();
    time = millis();
    if ((time-OldTime) > 60000){
      //1 minute divided by the time it took to do the rotary check 
      RPM = (60000/(time-OldTime));
    }else{
      RPM = 0;
    }
  }
  if(style == 2){
    // piston style
    pistonCheck();    time = millis();
    if ((time-OldTime) > 60000){
      //1 minute divided by the time it took to do the piston check times 2
      // for 1 pulse every other rotation 
      RPM = (60000/(time-OldTime)*2);
    }else{
      RPM = 0;
    }
  }
  
  report();
}

void report(){
  if(style == 1){
    // Rotary style, so print out all 3 faces
    printVals(3);
  }
  if(style == 2){
    printVals(1);
  }
  Serial.print("  RPM: ");
  Serial.println(RPM);
}

void printVals(int p){
  Serial.print("PSI current:");
  for(i=0; i<p; i++){
    Serial.print(" ");
    itoa(store[i], buf, 10);
    if(strlen(buf) < 3) Serial.print("0"); // Zero pad for less than 3 digits
    if(strlen(buf) < 2) Serial.print("0"); // Zero pad for less than 2 digits
    Serial.print(buf); // print value
  }
  Serial.print("  PSI peak:");
  for(i=0; i<p; i++){
    Serial.print(" ");
    itoa(peakStore[i], buf, 10);
    if(strlen(buf) < 3) Serial.print("0"); // Zero pad for less than 3 digits
    if(strlen(buf) < 2) Serial.print("0"); // Zero pad for less than 2 digits
    Serial.print(buf); // print value
  }
  
  itoa(RPM,buf,10);                    //convert the calculated RPM from int to string and dump it into an array
  if (strlen(buf)<3) Serial.print("0"); //if value is only 1 digit then dump a 0 into the array
  if (strlen(buf)<2) Serial.print("0"); //if value is only 2 digits then dump a 0 into the array
  Serial.print(buf);
  Serial.println();         //sends the "newline" command to the serial port
}

void pistonCheck() {
  pointer = 0;
  store[pointer] = readTillPeak();
  if (store[pointer] > peakStore[pointer]) peakStore[pointer] = store[pointer];
  readTillLow();
}

void rotaryCheck(){
  store[0]=store[1]=store[2]=0;  //zero out the faces
  for (i=0; i<3; i++)         //the following code reads the sensor (in psi), looks for a peak pulse, assigns that to one of the faces, repeats the process for the next 2 faces and then moves on to the rest of the code
  {
    store[i] = readTillPeak();
    if(peakStore[i] < store[i]) peakStore[i] = store[i];
    readTillLow();
  }
}
