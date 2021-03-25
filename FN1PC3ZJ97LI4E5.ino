/*TR-01 Rotary Compression Tester
Originally distributed by John Doss 2017

This code assumes you're using a 0-200 psi - 0.5-4.5 vdc pressure transducer connected to the AI0 pin.
You can use sensors with other pressure or voltage ranges but you'll need to modify the code.
*/

#define SENSOR 0          //Analog input pin that sensor is connected too

int face[3];              //3 faces per rotor
int v;                    //voltage from pressure sensor
int i;                    //counting faces
int minimum;              //psi to drop before new face
int firstrun=0;           //first time through test

unsigned long time;       //time for calc RPM
unsigned long OldTime;    //OldTime for calc RPM
unsigned long RPM;        //RPM variable

char buf[12],buf2[19],buf3[12],buf4[16];  //setting up arrays for compression and rpm data

void setup() {
  Serial.begin(19200);      //serial speed
  delay(500);               //hang out for half a second
  
  Serial.println("      TR-01 Open-Source");
  Serial.println("Rotary Engine Compression Tester");
  Serial.println("          Firmware v0.1");
  Serial.println("");
}

void loop(){
    if (firstrun==0){       //prints below text on bootup
      Serial.println("PSI: 000 000 000   RPM: 000");
      firstrun=1;
    }
    
  OldTime=millis();           //record cycle begining time for RPM calculation
  face[0]=face[1]=face[2]=0;  //zero out the faces
  for (i=0; i<3; i++)         //the following code reads the sensor (in psi), looks for a peak pulse, assigns that to one of the faces, repeats the process for the next 2 faces and then moves on to the rest of the code
  {
    v = (analogRead(SENSOR)-103)/4.096;
    if (v < 0){v = 0;}
    while ((face[i]-v)<=5)
    {
      if(face[i]<v) face[i]=v;
      v = (analogRead(SENSOR)-103)/4.096;
      if (v < 0){v = 0;}
    }   
    minimum=v;
    while((v-minimum)<5)
    {
      v=(analogRead(SENSOR)-103)/4.096;
      if (v < 0){v = 0;}
      if (minimum>v) minimum=v;
    }
  }
  time=millis();              //record cycle ending time for RPM calculation
  RPM=(180000/(time-OldTime));//calculate RPM

  strcpy(buf2,"PSI: ");       //Prep the "PSI results" array
  
  itoa(face[0],buf,10);                 //convert first face peak compression pulse from int to string and dump it into an array
  if (strlen(buf)<3) strcat(buf2,"0");  //if value is only 1 digit then dump a 0 into the array
  if (strlen(buf)<2) strcat(buf2,"0");  //if value is only 2 digits then dump a 0 into the array
  strcat(buf2,buf);                     //take the contents of that array and dump it into the "PSI results" array
  strcat(buf2," ");                     //dump a space into the "test results" array
  
  itoa(face[1],buf,10);
  if (strlen(buf)<3) strcat(buf2,"0");
  if (strlen(buf)<2) strcat(buf2,"0");
  strcat(buf2,buf);
  strcat(buf2," ");
  
  itoa(face[2],buf,10);
  if (strlen(buf)<3) strcat(buf2,"0");
  if (strlen(buf)<2) strcat(buf2,"0");
  strcat(buf2,buf);
  
  strcpy(buf4,"RPM: ");       //Prep the "RPM results" array
  
  itoa(RPM,buf3,10);                    //convert the calculated RPM from int to string and dump it into an array
  if (strlen(buf3)<3) strcat(buf4,"0"); //if value is only 1 digit then dump a 0 into the array
  if (strlen(buf3)<2) strcat(buf4,"0"); //if value is only 2 digits then dump a 0 into the array
  strcat(buf4,buf3);                    //take the contents of that array and dump it into the "RPM results" array

  Serial.print(buf2);       //print the "PSI results" array to the serial port
  Serial.print("   ");      //print a space to the serial port
  Serial.print(buf4);       //print the "RPM resutls" array to the serial port
  Serial.println();         //sends the "newline" command to the serial port
}
