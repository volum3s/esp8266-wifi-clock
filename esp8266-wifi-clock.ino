#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <LiquidCrystal.h>
#include <WifiClientSecure.h>
#include <ArduinoJson.h>
#include "credentials.h"

// Set up LiquidCrystal library with  LCD1602 pins
// Pinout from LCD1602 is (RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(12, 14, 2, 0, 4, 5);

// Configuring DarkSky API connection
const String blockParams = "?exclude=minutely,hourly,daily,alerts,flags";
const char* host = "darksky.net";

// This offset is for UTC-7
const long utcOffsetInSeconds = -25200;
char daysOfTheWeek[7][12] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

// Using millis instead of delay. Different delays for time and weather loops.
#define NTP_INTERVAL 1000
#define WEATHER_INTERVAL 30000
unsigned long ntpTimer = 0;
unsigned long weatherTimer = 0;

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

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  lcd.setCursor(0,0);
  lcd.print("Connecting to:");
  lcd.setCursor(0,1);
  lcd.print(WIFI_SSID);

  while ( WiFi.status() != WL_CONNECTED ) {
    lcd.setCursor(15, 1);
    lcd.print("\\");
    delay(200);
    lcd.setCursor(15, 1);
    lcd.print("\|");
    delay(200);
    lcd.setCursor(15, 1);
    lcd.print("/");
    delay(200);
      }
  lcd.clear();
  lcd.print("Wi-Fi Connected!");
  lcd.setCursor(0,1);
  lcd.print("IP: " + WiFi.localIP());
  delay(2000);
  
  timeClient.begin();  
  lcd.clear();
}

void loop() {
  // Get time
  if(millis() >= ntpTimer + NTP_INTERVAL){
    ntpTimer +=NTP_INTERVAL;
    timeClient.update();
    lcd.setCursor(0, 0);
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
    lcd.print(" ");
  }
  
  // Get weather
  if(millis() >= weatherTimer + WEATHER_INTERVAL){
    WiFiClientSecure client;
    client.setInsecure();
    lcd.setCursor(0, 1);
  
    if (!client.connect(host, 443))
      lcd.print("Cant get weather");

    else {
      lcd.print("Getting weather!");
      // Make the HTTP request
      client.println(String("GET https://api.") + host + "/forecast/" + DS_API_KEY + "/" + DS_LAT + "," + DS_LON + blockParams + " HTTP/1.0");
      client.print("Host: ");
      client.println(host);
      client.println("Connection: close");
      client.println();
    }
    
    while (client.connected()) {
      String line = client.readStringUntil('\n');
      if (line == "\r") {
        break;
      }
    }
    
    String line = client.readStringUntil('\n');
    const size_t capacity = JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(19) + 340;
    DynamicJsonDocument doc(capacity);    
    deserializeJson(doc, line);
    lcd.setCursor(13, 0);
    lcd.print(doc["currently"]["temperature"].as<int>());
    lcd.print("F");
    lcd.setCursor(0, 1);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    lcd.print(doc["currently"]["summary"].as<String>());
    weatherTimer +=WEATHER_INTERVAL;
  }
}
