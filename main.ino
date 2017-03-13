/*
 * JsonDatalogger
 *
 * Created: 8/2/2016
 * Author: HyDz
 * Humidity, temperature and time datalogger stored in µSD as JSON format. Also Display Date Time Température and Humidity on a SSD1306 based screen
 */ 

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DHT.h"
#include <ArduinoJson.h>
#include <SPI.h>
#include <SD.h>
#include "RTClib.h"

/* Libraries for SDFat * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4 */ 

const int chipSelect = 4;
int ledpin = 13;

// Date and time functions using a DS1307 RTC connected via I2C and Wire lib

RTC_DS1307 rtc;
char daysOfTheWeek[7][12] = {"Dimanche", "Lundi", "Mardi", "Mercredi", "Jeudi", "Vendredi", "Samedi"};
String dateTime;
String JSONized;
String tstamp;
String dayweek;

// Set Screen

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

// Set DHT

#define DHTPIN 8     // what digital pin we're connected to
#define DHTTYPE DHT11   // DHT 11
DHT dht(DHTPIN, DHTTYPE); 

int hint;   // variables to store temperature and humidity
int tint;


void setup() {

pinMode(ledpin, OUTPUT);
digitalWrite(ledpin, HIGH);
initSERIAL();
display.begin(SSD1306_SWITCHCAPVCC, 0x3c);  // initialize with the I2C addr 0x3D (for the 128x64)
dht.begin();

initSD();
initRTC();
display.clearDisplay();
display.setTextSize(3);
display.setTextColor(WHITE);
display.setCursor(35,3);  
display.println("HyDz");
display.display();
delay(4000);
display.clearDisplay();
digitalWrite(ledpin, LOW);

}

void loop() {

digitalWrite(ledpin, HIGH);
float h = dht.readHumidity();
float t = dht.readTemperature();
getTIME();
hint = (int) h;
tint = (int) t;
printScreen(hint, tint, dateTime);
JSONize("Date", dateTime, "Humidite", h, "Temperature", t);
writeToSD(JSONized);
digitalWrite(ledpin, LOW);
// delay(100);

}

void initSD(){
  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");
  }

void initSERIAL(){
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
 }

void initRTC(){
  Wire.begin(); //Démarrage de la librairie wire.h
  rtc.begin(); //Démarrage de la librairie RTClib.h
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
//  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
   // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2016, 6, 21, 19, 0, 0));
  }

}

 void writeToSD(String dataTowrite){
    
    // make a string for assembling the data to log:
  String dataString = "";
  dataString += String(dataTowrite);
  dataString += "\r";
   // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    Serial.println(dataString);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }
  
  }

  void getTIME(){
    DateTime now = rtc.now();
    char buffer [25] = "";
    char bstamp [25] = "";
    sprintf(bstamp, "%11d", now.unixtime());
    sprintf(buffer, "%02d/%02d/%04d %02d:%02d:%02d", now.day(), now.month(), now.year(), now.hour(), now.minute(), now.second());
    dayweek = daysOfTheWeek[now.dayOfTheWeek()] ;
    dateTime = buffer;
    tstamp = bstamp;
     delay(300);
    }

    void JSONize(String dataname, String datavalue, String dataname2, float datavalue2, String dataname3, float datavalue3){

       DynamicJsonBuffer  jsonBuffer;

       JsonObject& root = jsonBuffer.createObject();
       root[dataname] = datavalue;
       root[dataname2] = datavalue2;
       root[dataname3] = datavalue3;
       JSONized = "";
       root.prettyPrintTo(JSONized);
      // Serial.println(JSONized);
      
      }

  void printScreen(int h, int t, String dateTime){
  display.clearDisplay();  
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(5,0);
  display.println(dateTime);
 // display.display();
  display.setCursor(35,8);
  display.println(dayweek);
  display.setTextSize(2);
  display.setCursor(5,17);
  display.println(t);
  display.setCursor(30,17);
  display.println("C");
  display.setCursor(80,17);
  display.println(h);
  display.setCursor(105,17);
  display.println("%");
  display.display();
  // delay(100);
 }
