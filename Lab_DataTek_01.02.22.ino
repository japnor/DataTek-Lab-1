#include <SPI.h>                //Inkluderer diverse biblioteker som er nødvendig for bruk av OLED-skjermen
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 32 

#define OLED_RESET     -1 
#define SCREEN_ADDRESS 0x3C 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define HCSR 0    //Definerer diverse tallverdier for senere bruk slik at koden blir lettere å forstå
#define TMP 1
#define PHOTO 2
#define ALARM 3

const int photoResPin = A0;   //Definerer diverse integers for de forskjellige input-ene og output-ene jeg skal bruke til sensorer og knapper
const int tmpPin = A1;
const int trigger = 8;
const int echo = 7;
const int buttonPin = 2;
const int buzzerPin = 11;

int OLEDstate = 0;        //Integers som brukes til å lage OLED-karusellen
int buttonState = 0;         
int lastButtonState = 0;     


void setup() {
  pinMode(photoResPin, INPUT);                        //Definerer pinModes til input og output
  pinMode(tmpPin, INPUT);
  pinMode(buttonPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(trigger, OUTPUT);
  Serial.begin(115200);                               //Starter den serielle overvåkeren
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
   }
  display.setTextSize(1);                             //Setter tekststørrelse og farge på OLED-skjermen
  display.setTextColor(WHITE);
}



void loop() {
  buttonState = digitalRead(buttonPin);      //Kode som sjekker om knappen har blitt påtrykket og 
  if (buttonState != lastButtonState) {      //hvis den har det skal OLEDstate gå opp med en verdi
    if (buttonState == HIGH) {
      OLEDstate++;
    }
    delay(50);
  }
   lastButtonState = buttonState;
   
   if (OLEDstate >= 4) {                     //Hvis OLEDstate blir lik eller over 4 skal den gå tilbake
    OLEDstate = 0;                           //til 0 slik at karusellen kan funke ordentlig
  }

  float read_voltage = readVoltage(photoResPin);  //Leser og skriver de diverse sensorverdiene til den serielle overvåkeren
  float read_temp = readTemp();
  float read_dist = readDist();
  Serial.println(String(read_voltage));
  Serial.println(String(read_temp));
  Serial.println(String(read_dist));
  display.clearDisplay();
  display.setCursor(0, 1);
  
  switch (OLEDstate) {                                              //Bruker switch-case til å vise sensorverdiene på individuelle
    case HCSR:                                                      //skjermer ved å bruke definisjonene slik at jeg kan spare plass
      display.println("Spenning:" + String(read_voltage) + "V");
      break;
    case TMP:
      display.println("Temperatur:" + String(read_temp) + "C");
      break;
    case PHOTO:
      display.println("Avstand:" + String(read_dist) + "cm");
      break;
    case ALARM:                                                     //Den fjerde casen er alarmcasen og viser "ALARM" på skjermen i tillegg 
      if(read_dist < 10){
        display.println("ALARM");                                   //til å invertere den og aktivere en buzzer
        display.invertDisplay(true);
        digitalWrite(buzzerPin, HIGH);
        }
      else{
        display.invertDisplay(false);
        }
  }
  display.display();
  
}

float readVoltage(int pin) {                    //Funksjon for spenning fra lyssensor, som gitt i oppgavetekst
  float readValue = analogRead(photoResPin);
  float voltage = (readValue*5)/1023;
  return voltage;
}
float readTemp() {                              //Funksjon for temperatursensor, som gitt i oppgavetekst
  float readValue = analogRead(tmpPin);
  // Regner om fra ADC-verdi til spenning
  float voltage = (readValue*5)/1023;
  // Regner om fra spenning til celsius
  float celsius = (voltage-0.5)*100;
return celsius;
}
float readDist(){                               //Funksjon for ultralydsensor, som gitt i oppgavetekst
  // Skrur trigger lav for å være sikker på at
  // høypulsen er "ren"
  digitalWrite(trigger, LOW);
  delayMicroseconds(2);
  // Skrur på triggerpin i 10μs, deretter av igjen
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);
  // Leser av echopinne, og returnerer lydbølgens reisetid
  // i mikrosekunder
  long duration = pulseIn(echo, HIGH);
  // Utregning av distanse. 0.0343 er lydens hastighet i cm/μs.
  // Deles på to da lyden skal både frem og tilbake igjen
  float distance = duration * 0.0343 / 2;
  return distance;
  }
  
