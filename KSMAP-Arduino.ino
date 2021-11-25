//knihovny
#include <SoftwareSerial.h>
#include "DHT.h"

#define DHTPIN 7 //pin pro DHT senzor
#define DHTTYPE DHT22 //typ cidla
#define ATOMPIN 9

String readString;
int statePower = 0;
int ledBlue = 2;
int ledRed = 3;
int ledGreen = 4;

//teplota + vlhkost
double temp;
double hum;
double real;
int limit = 50;

//promenne cas
unsigned long actual;
unsigned long last;
unsigned long timeHum;
boolean firstIteration = false;
boolean humSet = false;

SoftwareSerial BT = SoftwareSerial(10, 11);
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  BT.begin(9600);
  dht.begin();

  pinMode(ATOMPIN, OUTPUT);

  pinMode(ledRed, OUTPUT);
  pinMode(ledGreen, OUTPUT);
}

void loop() {

  while (BT.available()) {
    delay(10);  //zpozdeni, aby se naplnil buffer

    char c = BT.read();  //cteni bytu ze serioveho bufferu
    if (c == ',') {
      break;
    }  //carka rozdeluje jednotlive prikazy
    readString += c;
  }

  //zpracovani signalu
  if (readString.length() > 0) {
    BT.println(readString); //kontrolni vypis do BT konzole

    if (readString.indexOf("ON") == 0) {
      statePower = 1;
    }
    if (readString.indexOf("OFF") == 0) {
      statePower = 0;
    }
    if (readString.indexOf("L") == 0) {
      String readString2 = readString;
      readString2 = readString2.substring(readString2.lastIndexOf("L") + 1);
      limit = readString2.toInt();
    }
    readString = ""; //vymazani pro novy vstup
  }

  //nastaveni casu
  actual = millis();

  //odeslani signalu do mobilni aplikace
  if (firstIteration || (actual - last > 1000)) {
    //teplota
    temp = dht.readTemperature();
    sendMessage("T", temp);

    //pocitova teplota
    real = dht.computeHeatIndex(temp, hum, false); //false - stupne celsia
    sendMessage("R", real);

    //vlhkost
    hum = dht.readHumidity();
    sendMessage("H", hum);

    //limit
    sendMessage("L", limit);

    firstIteration = false;
    last = actual;

    //debug
    Serial.println(hum);
  }

  //rozsveceni ledek

  if (statePower == 1) {
    digitalWrite(ledGreen, HIGH);
    digitalWrite(ledRed, LOW);
  } else {
    digitalWrite(ledGreen, LOW);
    digitalWrite(ledRed, HIGH);
  }

  //zvlhcovac
  if (statePower == 1) {
    if (hum < limit) {
      digitalWrite(ATOMPIN, HIGH);

    }
    else {
      digitalWrite(ATOMPIN, LOW);
    }
  }
  else {
    digitalWrite(ATOMPIN, LOW);
  }
}



void sendMessage(String symbol, double data) {
  String tmp = (String) symbol + data;
  BT.println(tmp);
}

void sendMessage(String symbol, int data) {
  String tmp = (String) symbol + data;
  BT.println(tmp);
}
