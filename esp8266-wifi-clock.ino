#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <LiquidCrystal.h>
#include <HTTPClient.h>

// Set up LiquidCrystal library with  LCD1602 pins
// Pinout from LCD1602 is (RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(12, 14, 2, 0, 4, 5);

const char *ssid     = "ENTER_SSID_HERE";
const char *password = "ENTER_PASS_HERE";

// Configuring DarkSky API (Add your API key and coordinates below)
const String apiKey = "<INSERT API KEY HERE>/"
const String lat = "<INSERT LATITUDE HERE>,"
const String lon = "<INSERT LONGITUDE HERE>"
const String url = "https://api.darksky.net/forecast/"

// This offset is for UTC-7
const long utcOffsetInSeconds = -25200;
char daysOfTheWeek[7][12] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

void setup(){
  Serial.begin(115200);

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  lcd.setCursor(0, 0);  
  lcd.print(" WiFi NTP Clock ");
  lcd.setCursor(0, 1);  
  lcd.print("    @volum3s    ");
  delay(3000);
  lcd.clear();

  WiFi.begin(ssid, password);
  lcd.setCursor(0,0);
  lcd.print("Connecting to:");
  lcd.setCursor(0,1);
  lcd.print(ssid);

  while ( WiFi.status() != WL_CONNECTED ) {
    // Serial.print ( "." );
    lcd.setCursor(15, 1);
    lcd.print("\\");
    delay(200);
    lcd.setCursor(15, 1);
    lcd.print("|");
    delay(200);
    lcd.setCursor(15, 1);
    lcd.print("/");
    delay(200);
      }
  lcd.clear();
  lcd.print("Wi-Fi Connected!");
  lcd.setCursor(0,1);
  lcd.print("IP: " + WiFi.localIP());
  delay(3000);
  
  timeClient.begin();  
  lcd.clear();
}

void loop() {
  timeClient.update();
  lcd.setCursor(0, 0);

  lcd.print("  ");
  lcd.print(daysOfTheWeek[timeClient.getDay()]);
  lcd.print(" ");

  // Print HH:MM:SS, prepending a zero for single digits
  if ((timeClient.getHours()) < 10) {
    lcd.print("0");
  }
  lcd.print(timeClient.getHours());
  lcd.print(":");
  
  if ((timeClient.getMinutes()) < 10) {
    lcd.print("0");
  }
  lcd.print(timeClient.getMinutes());
  lcd.print(":");
  
  if ((timeClient.getSeconds()) < 10) {
    lcd.print("0");
  }
  lcd.print(timeClient.getSeconds());
  
  lcd.setCursor(0,1);
// Planning to add weather info here like "55F - Rain"

  delay(1000);
}
