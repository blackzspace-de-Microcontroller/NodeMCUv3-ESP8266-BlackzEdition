/*

Author: BlackLeakz
Website: https://tutorials.blackzspace.de/esp8266/blackleakz-nodemcuv3


*/

#include <Arduino.h>
#include "FS.h"
#include <SPI.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266mDNS.h>
#include <AsyncElegantOTA.h>
#include <Adafruit_GFX.h>
#include <SD.h>
#include <ArduinoJson.h>


// Display Definitions

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET     -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_SSD1306 display2(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);






const char *ssid = "blackzspace.de";
const char *password = "018004646";


const char *dns_name = "blackleakz";


File myFile;
File inFile;


unsigned int numberOfNetworks;
unsigned long lastAusgabe;
const unsigned long intervall = 5000;


#define LOGO_HEIGHT 32
#define LOGO_WIDTH 32
const unsigned char NaN[] PROGMEM = {
    // 'favicon-32x32, 32x32px
    0xc0, 0x00, 0x00, 0x03, 0x80, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x30, 0x00, 0x0f, 0xf8,
    0x30, 0x00, 0x1f, 0xfc, 0x30, 0x00, 0x38, 0x08, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00,
    0x37, 0xf0, 0x30, 0x00, 0x3e, 0x78, 0x38, 0x00, 0x38, 0x1c, 0x1e, 0x00, 0x30, 0x1c, 0x0f, 0xc0,
    0x30, 0x0c, 0x03, 0xf0, 0x30, 0x0c, 0x00, 0xf8, 0x30, 0x0e, 0x00, 0x3c, 0x30, 0x0e, 0x00, 0x1c,
    0x30, 0x0c, 0x00, 0x0c, 0x30, 0x0c, 0x00, 0x0c, 0x30, 0x1c, 0x00, 0x1c, 0x38, 0x38, 0x70, 0x38,
    0x3f, 0xf0, 0x7f, 0xf0, 0x37, 0xe0, 0x1f, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x01, 0xc0, 0x00, 0x00, 0x03};








AsyncWebServer server(80);



void testscrolltext(void)
{
  display.clearDisplay();
  display.setTextSize(2); 
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.println(F("scroll"));
  display.display();
  delay(100);
  display.startscrollright(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrollleft(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrolldiagright(0x00, 0x07);
  delay(2000);
  display.startscrolldiagleft(0x00, 0x07);
  delay(2000);
  display.stopscroll();
  delay(1000);
}


void testdrawbitmap(void)
{
  display.clearDisplay();

  display.drawBitmap(
      (display.width() - LOGO_WIDTH) / 2,
      (display.height() - LOGO_HEIGHT) / 2,
      NaN, LOGO_WIDTH, LOGO_HEIGHT, 1);
  display.display();
  delay(4500);
}


String content;

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}




void wifi_scan_to_oled() {
  Serial.println("Console > Starting Network Scan !! :::..::.");
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Console > ");
  display.display();
  display.setCursor(2, 10);
  display.println("Scan for: ");
  display.display();
  display.setCursor(1, 0);
  display.println("Networkz !!");
  display.display();

  delay(900);

  if (numberOfNetworks <= 0)
  {
    numberOfNetworks = WiFi.scanNetworks();
    Serial.print(numberOfNetworks);
    Serial.println(F(" gefundene Netzwerke"));
  }
  else if (millis() - lastAusgabe > intervall)
  {
    numberOfNetworks--;
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Network-name: ");
    display.setCursor(0, 10);
    display.print(WiFi.SSID(numberOfNetworks));
    display.setCursor(0, 20);
    display.print("Signal strength: ");
    display.setCursor(0, 30);
    display.print(WiFi.RSSI(numberOfNetworks));
    display.display();
    Serial.print("Network name: ");
    Serial.println(WiFi.SSID(numberOfNetworks));
    Serial.print("Signal strength: ");
    Serial.println(WiFi.RSSI(numberOfNetworks));
    Serial.println("-----------------------");
    lastAusgabe = millis();
  }
}






void init_oled() {

  Serial.println("Console > Initialize Oled SSD1306 displays.\n");


  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3D))
  {
    Serial.println(F("Console > SSD1306 allocation failed"));
    for (;;)
      ;
  }


  if (!display2.begin(SSD1306_SWITCHCAPVCC, 0x3D))
  {
    Serial.println(F("Console > SSD1306 allocation failed"));
    for (;;)
      ;
  }
}




void setup() {
  Serial.begin(115200);
  Serial.println("Console > blackzspace.de - NodeMCUv3 ESP8266 - Started!!! ... .. .");
  Serial.println("Console > Initialize LED_BUILTIN pin as an output..: Blinking 3 times.");

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  delay(300);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);


 

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3D))
  {
    Serial.println(F("Console > SSD1306 allocation failed"));
    for (;;)
      ;
  }

  display.display();
  display.clearDisplay();
  testdrawbitmap();
  delay(5000);
  display.clearDisplay();
  display.setTextColor(WHITE); 
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Console >");
  display.display();
  display.setCursor(0, 10);
  display.println("  Started.");
  display.display();
 

 





  Serial.print("Initializing SD card...");

  if (!SD.begin(15)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  myFile = SD.open("index.html", FILE_READ);




  Serial.print("Setting soft-AP configuration ... ");
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("IP Address: ");
  display.display();
  display.setCursor(0,10);
  display.println(IP);  
  display.display();

  Serial.println(WiFi.localIP());
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  display.setCursor(0,20);
  display.println("IP Address: ");
  display.setCursor(0,30);  
  display.println(WiFi.localIP());
  display.display();



   // Starting mDNS
  if (MDNS.begin(dns_name))
  {
    Serial.println("DNS gestartet, erreichbar unter: ");
    Serial.println("http://" + String(dns_name) + ".local/");
  }
  else
  {
    Serial.println("Fehler beim starten des DNS");
  }





  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/html", myFile.readString());
    });

  server.on("/scan", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "Scan"); 
  });


  server.onNotFound(notFound);

  AsyncElegantOTA.begin(&server);


  server.begin();


  
}

void loop() {

  // mDNS update
  MDNS.update();

}
