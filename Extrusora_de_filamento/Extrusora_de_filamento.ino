#include <Thermistor.h> //INCLUSÃO DA BIBLIOTECA
#include <PID_v1.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// Connections to A4988
const int PinoDir = 12;  // Direction D6
const int PinoStep = 13; // Step      D7
// Motor steps per rotation
const int STEPS = 200;

LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7,3, POSITIVE);  // set the LCD address for a 16x2 line display
double DefinirTemp, DefinirVelocidade, TempAtual, Output;

int s0 = 2; //D4
int s1 = 0; //D3
int s2 = 16; //D0
int PinoAquecedor = 14; //D5

// OPÇÃO CONTROLE TEMPERATURA 1
//Thermistor temp(A0); //VARIÁVEL DO TIPO THERMISTOR, INDICANDO O PINO ANALÓGICO (A2) EM QUE O TERMISTOR ESTÁ CONECTADO

int PinoAnalogico = A0;

//Specify the links and initial tuning parameters
PID myPID(&TempAtual, &Output, &DefinirTemp,2,5,1, DIRECT);

//the time we give the sensor to calibrate (10-30 secs according to the datasheet)
int calibrationTime = 10; 

// OPÇÃO CONTROLE TEMPERATURA 2 
double Thermister(int RawADC) {
 double Temp;
 Temp = log(((10240000/RawADC) - 10000));
 Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp ))* Temp );
 Temp = Temp - 273.15;            // Convert Kelvin to Celcius
 //Temp = (Temp * 9.0)/ 5.0 + 32.0; // Convert Celcius to Fahrenheit
 return Temp;
}

void setup() {
  // Setup the pins as Outputs
  pinMode(PinoStep,OUTPUT); 
  pinMode(PinoDir,OUTPUT);
  
  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(PinoAquecedor, OUTPUT);
  Serial.begin(9600);
  lcd.begin (16,2);
  lcd.clear();
  
  //give the sensor some time to calibrate
  Serial.print("calibrando sensor ");
  lcd.setCursor(0,0);
  lcd.print("calibrando...");
    for(int i = 0; i < calibrationTime; i++){
      Serial.print(".");
      delay(1000);
      }
    Serial.println(" done");
    Serial.println("SENSOR ACTIVE");
    delay(50);

  digitalWrite(s0,LOW);
  digitalWrite(s1,LOW);
  digitalWrite(s2,LOW);
  TempAtual = double(Thermister(analogRead(PinoAnalogico))); 
  //delay(100);
  
  digitalWrite(s0,HIGH);
  digitalWrite(s1,LOW);
  digitalWrite(s2,LOW);  
  DefinirTemp = map(analogRead(PinoAnalogico),0,1023,0,300);
  //delay(100);
   
  lcd.clear();
  lcd.setCursor(0,0);
   lcd.print("V    |Ti  |Tf ");
  
  //turn the PID on
  myPID.SetMode(AUTOMATIC);
}

void loop() {
  digitalWrite(s0,LOW);
  digitalWrite(s1,LOW);
  digitalWrite(s2,LOW);
  TempAtual = double(Thermister(analogRead(PinoAnalogico)));  
  //TempAtual = temp.getTemp();  
  //delay(100);
  
  digitalWrite(s0,HIGH);
  digitalWrite(s1,LOW);
  digitalWrite(s2,LOW);  
  DefinirTemp = map(analogRead(PinoAnalogico),0,1023,0,300);
  //delay(100);

  digitalWrite(s0,LOW);
  digitalWrite(s1,HIGH);
  digitalWrite(s2,LOW);
  DefinirVelocidade = map(analogRead(PinoAnalogico),0,1023,1000,2000);
  
  digitalWrite(PinoDir,HIGH); 
  for(int x = 0; x < STEPS; x++) {
    digitalWrite(PinoStep,HIGH);
    delayMicroseconds(DefinirVelocidade);
    digitalWrite(PinoStep,LOW);
    delayMicroseconds(DefinirVelocidade);
  }
  //delay(100);
  
  myPID.Compute();
  analogWrite(PinoAquecedor,Output);
  //digitalWrite(PinoAquecedor,LOW);  
  // Print a message to the LCD.
  lcd.setBacklight(HIGH);
  lcd.setCursor(0,1);
  lcd.print("    ");
  lcd.setCursor(0,1);
  lcd.print(DefinirVelocidade,0);

  lcd.setCursor(5,1);
  lcd.print("|");

  lcd.setCursor(6,1);
  lcd.print("    ");
  lcd.setCursor(6,1);
  lcd.print(DefinirTemp,0);

  lcd.setCursor(10,1);
  lcd.print("|");

  lcd.setCursor(11,1);
  lcd.print("    ");
  lcd.setCursor(11,1);
  lcd.print(TempAtual,0);
  
  Serial.print(DefinirTemp);  Serial.print("     ");
  Serial.print(TempAtual);  Serial.print("     ");
  Serial.println(DefinirVelocidade);
}
