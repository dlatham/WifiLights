#include <ESP8266WiFi.h>
#include <Adafruit_NeoPixel.h>
#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266mDNS.h>          //Support .local URLs
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic

#define PIN 4 // Which pin on the Arduino is connected to the NeoPixels?
#define NUMPIXELS 16 // How many NeoPixels are attached to the Arduino?
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
uint32_t defaultColor = pixels.Color(255, 241, 224); // Default light color (warm white)
//uint32_t blue = pixels.Color(255, 255, 200);
ESP8266WebServer server(80);
String header; //variable to store the HTTP request

// SERVER HANDLER FUNCTIONS------------------------------------------>
void handleStatus(){
  Serial.println(F("SERVER: Status request"));
  server.send(200, "text/plain", "The light server is on.");
}

void handleColor(){
  Serial.println(F("SERVER: Color change request."));
  uint8_t r=0, g=0, b=0;
  r=atoi(server.arg("r").c_str());
  g=atoi(server.arg("g").c_str());
  b=atoi(server.arg("b").c_str());
  for(uint8_t i=0; i<NUMPIXELS; i++){
    pixels.setPixelColor(i, r, g, b);
  }
  pixels.show();
  server.send(200, "text/plain", "Color change request.");
}

void handleNotFound(){
  Serial.println(F("SERVER: Not found"));
  server.send(404, "text/plain", "Not found.");
}

// PROGRAM SETUP-------------------------------------------------------->
void setup() {
  Serial.begin(115200);
  Serial.println("White Wolf Lights v0.1");
  pixels.begin(); //Start the pixels up first before the web stuff
  for(uint8_t i=0; i<NUMPIXELS; i++){
    pixels.setPixelColor(i, defaultColor);
  }
  pixels.show();

  //Wifi setup
  WiFi.hostname("whitewolf-lights");
  WiFiManager wifiManager; // Start the wifimanager
  //first parameter is name of access point, second is the password
  wifiManager.autoConnect("whitewolf-lights", "whitewolf");

  //mDNS Setup
  if (!MDNS.begin("wifilights")) {             // Start the mDNS responder for wifilights.local
    Serial.println("Error setting up MDNS responder!");
  }
  Serial.println("mDNS responder started");

  //Server setup
  server.on("/status", HTTP_GET, handleStatus);
  server.on("/color", HTTP_GET, handleColor);
  server.onNotFound(handleNotFound);
  server.begin();

}

void loop() {
  /* the restful API will function like the following:
   * -------------------------------------------------
   * /color?r=128&g=128&b=128  Change all LEDs to the rgb value
   * /on?o=255  Turn all LEDs on to the specifid opacity (optional)
   * /off . Turn all LEDs off
   */
  server.handleClient();
   
}
