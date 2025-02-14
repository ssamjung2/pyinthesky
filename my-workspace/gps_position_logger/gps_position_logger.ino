/*
  GPS Position Logger
  gps-position-logger.ino
  Read GPS data from BN-220 or any serial GPS sensor
  Requires TinyGPS++ Library
  Save to SD or microSD card

  DroneBot Workshop 2021
  https://dronebotworkshop.com/using-gps-modules/#Build_a_GPS_Position_Logger
*/

// Include required libraries
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <SD.h>

// GPS Connections
static const int RXPin = 4, TXPin = 3;

// GPS Baud rate (change if required)
static const uint32_t GPSBaud = 9600;

// SD Card Select pin
const int chipSelect = 5;

// Write LED
const int recLED = 7;

// String to hold GPS data
String gpstext;

// GPS write delay counter variables
// Change gpsttlcount as required
int gpscount = 0;
int gpsttlcount = 30;

// TinyGPS++ object
TinyGPSPlus gps;

// SoftwareSerial connection to the GPS device
//SoftwareSerial ss(RXPin, TXPin);
SoftwareSerial ss(TXPin, RXPin);

void setup()
{
  // Set LED pin as output
  pinMode(recLED, OUTPUT);

  // Start Serial Monitor for debugging
  Serial.begin(115200);

  // Start SoftwareSerial
  ss.begin(GPSBaud);

  // Initialize SD card
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    //don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");

  // Blink LED so we know we are ready
  digitalWrite(recLED, HIGH);
  delay(50);
  digitalWrite(recLED, LOW);
  delay(50);
  digitalWrite(recLED, HIGH);
  delay(50);
  digitalWrite(recLED, LOW);
  delay(50);
  digitalWrite(recLED, HIGH);
  delay(50);
  digitalWrite(recLED, LOW);

}

void loop()
{

  // Turn off LED
  digitalWrite(recLED, LOW);

  Serial.println("a");

  // See if data available
  while (ss.available() > 0)
    Serial.println("b");
    
    if (gps.encode(ss.read()))
      Serial.println("c");
      
      // See if we have a complete GPS data string
      if (displayInfo() != "0")
      {
        Serial.println("d");
        
        // Get GPS string
        gpstext = displayInfo();


        // Check GPS Count
        Serial.println(gpscount);
        if (gpscount == gpsttlcount) {

          // LED On to indicate data to write to SD card
          digitalWrite(recLED, HIGH);

          //Open the file on card for writing
          File dataFile = SD.open("gpslog.csv", FILE_WRITE);

          if (dataFile) {

            // If the file is available, write to it and close the file
            dataFile.println(gpstext);
            dataFile.close();

            // Serial print GPS string for debugging
            Serial.println(gpstext);
          }
          // If the file isn't open print an error message for debugging
          else {
            Serial.println("error opening datalog.txt");
          }

        }
        // Increment GPS Count
        gpscount = gpscount + 1;
        if (gpscount > gpsttlcount) {
          gpscount = 0;
        }

      }
      Serial.println("e");
}

// Function to return GPS string
String displayInfo()
{
  // Define empty string to hold output
  String gpsdata = "";

  // Get latitude and longitude
  if (gps.location.isValid())
  {
    gpsdata = String(gps.location.lat(), 6);
    gpsdata += (",");
    gpsdata += String(gps.location.lng(), 6);
    gpsdata += (",");
  }
  else
  {
    return "0";
  }

  // Get Date
  if (gps.date.isValid())
  {
    gpsdata += String(gps.date.year());
    gpsdata += ("-");
    if (gps.date.month() < 10) gpsdata += ("0");
    gpsdata += String(gps.date.month());
    gpsdata += ("-");
    if (gps.date.day() < 10) gpsdata += ("0");
    gpsdata += String(gps.date.day());
  }
  else
  {
    return "0";
  }

  // Space between date and time
  gpsdata += (" ");

  // Get time
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) gpsdata += ("0");
    gpsdata += String(gps.time.hour());
    gpsdata += (":");
    if (gps.time.minute() < 10) gpsdata += ("0");
    gpsdata += String(gps.time.minute());
    gpsdata += (":");
    if (gps.time.second() < 10) gpsdata += ("0");
    gpsdata += String(gps.time.second());
  }
  else
  {
    return "0";
  }

  // Return completed string
  return gpsdata;
}
