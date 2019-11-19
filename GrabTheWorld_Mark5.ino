#include <Servo.h>
#include <SoftwareSerial.h>


#define SOFTSERIAL_RX_PIN  D6
#define SOFTSERIAL_TX_PIN  D7

Servo servoMindinhoA;
Servo servoMedioB;
Servo servoPolegarB;
Servo servoIndicadorB;
Servo servoAnelarA;
int sideA[]={0,30,60,90,130,180};
int sideB[]={180,130,90,60,30,0};

SoftwareSerial softSerial(SOFTSERIAL_RX_PIN,SOFTSERIAL_TX_PIN);

int sensorPin = A0; 

int mindinho = D5;
int medio = D4;
int anelar = D3;
int polegar = D1;
int indicador = D2;

int voice = 0;

//EMG Variables
const int arraySize=300; /*Vector size, the bigger the number, more smooth it will be*/
int arrayPosition=0 , i, sensorValue=0,absSensorValue, pinValue=0, oldPinValue=0;
long sumSensor[arraySize];

float smoothed=0.0;
long sumNumbers;
unsigned long t0;
long dt = 1000; /* Time between samples */

struct mainValues{         /* Creates an easier acess to the values */ 
  unsigned long times=t0;
  int raw=sensorValue;
  float smooth=smoothed; 
};

mainValues values = {};  /* To acess time, raw and smooth use like values.times or values.raw or values.smooth */
//-----------------
//Voz
const char *voiceBuffer[] =
{
    "Turn on the light",
    "Turn off the light",
    "Play music",
    "Pause",
    "Next",
    "Previous",
    "Up",
    "Down",
    "Turn on the TV",
    "Turn off the TV",
    "Increase temperature",
    "Decrease temperature",
    "What's the time",
    "Open the door",
    "Close the door",
    "Left",
    "Right",
    "Stop",
    "Start",
    "Mode 1",
    "Mode 2",
    "Go",
};

//-----------------
//EMG Functions

void smoothedFunction (){

  t0 = micros();
  oldPinValue = pinValue; /* Needed to use in the derivation */
  pinValue = analogRead(sensorPin); 
  sensorValue = pinValue-oldPinValue;  /* With this line its possible to make the value 0 as our reference point */ 
  absSensorValue=abs(sensorValue); /*Transform negative values into positive values*/
  if (arrayPosition>=arraySize){ /*If the position that the program is writing in the array is bigger than the size of the array, it will replace the index nr 0 of the array*/
    arrayPosition=0;  
  }
  sumNumbers-=sumSensor[arrayPosition];  
  sumSensor[arrayPosition]=absSensorValue; /*Add the value received from the sensor to the array*/
  sumNumbers+=sumSensor[arrayPosition];
  ++arrayPosition; /*Increment 1 value to the array position, making it move to the next index from the array*/
  smoothed=(sumNumbers)/((float)arraySize); /*Make the average value using the values received*/
  values.times=t0;
  values.raw=pinValue;
  values.smooth=smoothed;
}
//-----------------
void setup() {
  Serial.begin(9600);
  softSerial.begin(9600);
  softSerial.listen();
  //Serial.begin(2000000);
  pinMode(mindinho,OUTPUT);
  pinMode(medio,OUTPUT);
  pinMode(anelar,OUTPUT);
  pinMode(polegar,OUTPUT);
  pinMode(indicador,OUTPUT);
  servoMindinhoA.attach(mindinho);
  servoAnelarA.attach(anelar);
  servoMedioA.attach(medio);
  servoPolegarB.attach(polegar);
  servoIndicadorB.attach(indicador);

  //EMG
  t0=micros();
  for(i=0; i<arraySize; i++) {
    sumSensor[i]=0;
  }
  sumNumbers=0;
  //-------

}
int lastV =0 ;
int lastMedio =0 ;
void loop() {
 /*if(Serial.available()){
  int v = Serial.parseInt();
  if(v != lastV){
    lastV = v;
   servoMindinhoA.write(sideA[v]);
   servoMedioB.write(sideB[v]);
   servoPolegarB.write(sideB[v]);
   servoAnelarA.write(sideA[v]);
  }*/
  //Voice Section
  char cmd;

  if(softSerial.available())
  {
      cmd = softSerial.read();
      //Serial.println(voiceBuffer[cmd - 1]);
  }
  if(cmd==19){
    voice=1;
    Serial.println(voice);
  }
  if(cmd==18){
    voice=0;
    Serial.println(voice);
  }
  if(voice==1){
    if(cmd==4){
      servoMindinhoA.write(sideA[0]);
      servoMedioA.write(sideA[0]);
      servoPolegarB.write(sideB[0]);
      servoAnelarA.write(sideA[0]);
      servoIndicadorB.write(sideB[0]);
      // servotoy.write(0);
    }
    if(cmd==5){
      servoMindinhoA.write(sideA[5]);
      servoMedioA.write(sideA[5]);
      servoPolegarB.write(sideB[5]);
      servoAnelarA.write(sideA[5]);
      servoIndicadorB.write(sideB[5]);
      //servotoy.write(90);
    }
   }
    
  
  if(voice==0){
    if ((micros()-t0)>=dt) {   /*Everything inside this if statment happens at the same time as the acquisition */
      smoothedFunction();  
      //Serial.println(values.smooth);
      if(values.smooth >25){
        servoMindinhoA.write(sideA[0]);
        servoMedioA.write(sideA[0]);
        servoPolegarB.write(sideB[0]);
        servoAnelarA.write(sideA[0]);
        servoIndicadorB.write(sideB[0]);
      }
      else{
        servoMindinhoA.write(sideA[5]);
        servoMedioA.write(sideA[5]);
        servoPolegarB.write(sideB[5]);
        servoAnelarA.write(sideA[5]);
        servoIndicadorB.write(sideB[5]);
      }
    }  
  }
}
 
