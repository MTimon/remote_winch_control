//------------------------------------------------------------------------------------
// Libraries Needed For This Project
//------------------------------------------------------------------------------------
#include <ESP8266WiFi.h>      // The Basic Function Of The ESP NODE MCU 
//#include "WiFi.h"    //option for ESP32 NodeMCU
//#include <ESP8266WiFiMulti.h> // Access to multiple WiFi
#include <WiFiUdp.h>          // UDP Library

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <TaskScheduler.h>
//------------------------------------------------------------------------------------
// Defining I/O Pins
//------------------------------------------------------------------------------------
#define       LED0      16        // SDK LED
#define       LED1      2         // WIFI Module LED 
#define       BUTTON    15         // Finger Contact Button
//------------------------------------------------------------------------------------
// Defining WiFi signal strength
//------------------------------------------------------------------------------------
#define Wifistr4 -60
#define Wifistr3 -80
#define Wifistr2 -100
//------------------------------------------------------------------------------------
// Defining Fader Properties
//------------------------------------------------------------------------------------
#define MinFader 75
#define MaxFader 890
#define dspMnFd 31
#define dspMxFd 85
//------------------------------------------------------------------------------------
// Defining Tacho Properties
//------------------------------------------------------------------------------------
#define MaxFreq 7500
#define MinFreq 0000
#define MinCur 0000
#define MaxCur 0500
//------------------------------------------------------------------------------------
// Scheduler
//------------------------------------------------------------------------------------
// Callback methods prototypes

void t1Callback();
//Tasks
Task t1(100, TASK_FOREVER, &t1Callback);
Scheduler runner;

//------------------------------------------------------------------------------------
// Defining OLED
//------------------------------------------------------------------------------------

//#define OLED_RESET LED_BUILTIN // 4
#define OLED_RESET LED1
Adafruit_SSD1306 display(OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2


#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

#define torqueCenterX 30
#define torqueCenterY 63
#define speedCenterX 97
#define speedCenterY 63
float angleT = 88;
float angleS = 88;

boolean fingBlink = true;
boolean fadBlink = true;
//Scale Tacho 61x33
const unsigned char PROGMEM scal_61_33 [] = {
  0x00, 0x00, 0x00, 0x7F, 0xF0, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x03, 0x82, 0x0E, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x1C, 0x02, 0x01, 0xC0, 0x00, 0x00,
  0x00, 0x00, 0x60, 0x00, 0x00, 0x30, 0x00, 0x00,
  0x00, 0x01, 0x90, 0x00, 0x00, 0x4C, 0x00, 0x00,
  0x00, 0x06, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00,
  0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00,
  0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00,
  0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00,
  0x00, 0x50, 0x00, 0x00, 0x00, 0x00, 0x50, 0x00,
  0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00,
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00,
  0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00,
  0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00,
  0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00,
  0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x80,
  0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
  0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40,
  0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40,
  0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20,
  0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20,
  0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20,
  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10,
  0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30,
  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10,
  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10,
  0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08,
  0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08,
  0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08,
  0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08,
  0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08,
  0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8
};

//WiFi symbol 1 16x16
const unsigned char PROGMEM wifi01 [] = {
  0x00, 0x00,
  0x00, 0x00,
  0x00, 0x00,
  0x00, 0x00,
  0x00, 0x00,
  0x00, 0x00,
  0x00, 0x00,
  0x00, 0x00,
  0x00, 0x00,
  0x00, 0x00,
  0x00, 0x00,
  0x00, 0x00,
  0x00, 0x03,
  0x00, 0x07,
  0x00, 0x0F,
  0x00, 0x0F
};

//WiFi symbol 2 16x16
const unsigned char PROGMEM wifi02 [] = {
  0x00, 0x00,
  0x00, 0x00,
  0x00, 0x00,
  0x00, 0x00,
  0x00, 0x00,
  0x00, 0x00,
  0x00, 0x00,
  0x00, 0x00,
  0x00, 0x00,
  0x00, 0x07,
  0x00, 0x08,
  0x00, 0x10,
  0x00, 0x23,
  0x00, 0x47,
  0x00, 0x4F,
  0x00, 0x4F
};

//WiFi symbol 3 16x16
const unsigned char PROGMEM wifi03 [] = {
  0x00, 0x00,
  0x00, 0x00,
  0x00, 0x00,
  0x00, 0x00,
  0x00, 0x00,
  0x00, 0x07,
  0x00, 0x18,
  0x00, 0x60,
  0x00, 0x80,
  0x01, 0x07,
  0x01, 0x08,
  0x02, 0x10,
  0x02, 0x23,
  0x04, 0x47,
  0x04, 0x4F,
  0x04, 0x4F
};

//WiFi symbol 4 16x16
const unsigned char PROGMEM wifi04 [] = {
  0x00, 0x0F,
  0x00, 0x70,
  0x00, 0x80,
  0x03, 0x00,
  0x04, 0x00,
  0x08, 0x07,
  0x10, 0x18,
  0x10, 0x60,
  0x20, 0x80,
  0x41, 0x07,
  0x41, 0x08,
  0x42, 0x10,
  0x82, 0x23,
  0x84, 0x47,
  0x84, 0x4F,
  0x84, 0x4F
};
//WiFi symbol Finger 16x16
const unsigned char PROGMEM Fing [] = {
  0x00, 0x06,
  0x00, 0x0F,
  0x00, 0x0F,
  0x00, 0x0F,
  0x00, 0x0F,
  0x00, 0x0F,
  0x00, 0xEF,
  0x0E, 0xFF,
  0x6F, 0xFF,
  0xFF, 0xFF,
  0xFF, 0xFF,
  0xFF, 0xFF,
  0xFF, 0xFF,
  0xFF, 0xFF,
  0x7F, 0xFE,
  0x7F, 0xFE
};
const unsigned char PROGMEM Fader [] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x38, 0x1C, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x7F, 0xFE, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x7F, 0xFE, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x7F, 0xFE, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x03, 0xC0, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x03, 0xC0, 0x00, 0x00, 0x00,
  0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8,
  0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE,
  0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};
const unsigned char PROGMEM Fader_soc [] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8,
  0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE,
  0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};
const unsigned char PROGMEM Fader_K [] = {
  0x70, 0x38,
  0xFF, 0xFC,
  0xFF, 0xFC,
  0xFF, 0xFC,
  0x07, 0x80,
  0x07, 0x80
};



//------------------------------------------------------------------------------------
// FADER Variables
//------------------------------------------------------------------------------------
const int     AnalogIn          = A0;
const float     dspFdgain = -0.0662;//(dspMnFd-dspMxFd) / (MaxFader - MinFader);
const float     dspoff    = 90;//dspMxFd - dspFdgain * MinFader;
//------------------------------------------------------------------------------------
// UDP Variables
//------------------------------------------------------------------------------------
WiFiUDP Udp;
IPAddress ip(192, 168, 4, 2); // where xx is the desired IP Address
IPAddress gateway(192, 168, 4, 1); // set gateway to match your network
IPAddress subnet(255, 255, 255, 0); // set subnet mask to match your network
unsigned int localUdpPort = 4210;  // local port to listen on
const char* ssid = "ESPsoftAP_01";
const char* password = "pass-to-soft-AP";
char incomingPacket[255];
char  replyPacekt[] = "Hi there! Got the message :-)";
char StatFader[] = {0};
//------------------------------------------------------------------------------------
// State Variables
//------------------------------------------------------------------------------------
String myString = "";
String myString1 = "";
int rFreq = 0;
int rVolt = 0;
int rCurr = 0;
int rVDC = 0;
int rStat = 0;
int rTorq = 0;
int rTHS = 0;
int rState = 0;
//------------------------------------------------------------------------------------
// WiFi
//------------------------------------------------------------------------------------
//ESP8266WiFiMulti wifiMulti;
//boolean connectionWasAlive = true;

//Function Prototypes

void monitorWiFi(void);
int ReadButton(void);
void setup(void);

//====================================================================================

void t1Callback()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    if (digitalRead(LED1) == LOW)
    {
      digitalWrite(LED1, 1);       // Turn WiFi LED Off
    }
    else
    {
      digitalWrite(LED1, 0);      // Turn WiFi LED on
    }
    display.clearDisplay();
    disp_wifi();
    disp_tacho();
    disp_fing();
    disp_fad();
    display.display();
    Serial.println(angleS);
  }
}

//====================================================================================

void setup()
{
  // Setting The Serial Port ----------------------------------------------
  Serial.begin(9600);           // Computer Communication

  // Setting The Mode Of Pins ---------------------------------------------
  pinMode(LED0, OUTPUT);          // WIFI OnBoard LED Light
  pinMode(LED1, OUTPUT);          // WIFI LED
  pinMode(BUTTON, INPUT_PULLUP);  // Initiate Connectivity
  digitalWrite(LED0, !LOW);
  digitalWrite(LED1, 1);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x64)
  display.clearDisplay();
  display.display();
  Serial.println();
  display.clearDisplay();
  start();
  display.display();
  delay(500);
  WiFi.config(ip, gateway, subnet);
  delay(100);
  WiFi.mode(WIFI_STA);
  delay(100);
  WiFi.begin(ssid, password);
  display.clearDisplay();
  please_wait();
  display.display();
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  Udp.begin(localUdpPort);

  //delay(100);

  runner.init();
  runner.addTask(t1);
  t1.enable();

  Serial.println("Enabled t1");


  // Print Message Of I/O Setting Progress --------------------------------
  Serial.println("\nI/O Pins Modes Set .... Done");

  // Starting To Connect --------------------------------------------------
  //wifiMulti.addAP("ESPsoftAP_01", "pass-to-soft-AP");
}

//====================================================================================

void loop()
{
  runner.execute();
  //monitorWiFi();
  if (WiFi.status() == WL_CONNECTED)
    //if (wifiMulti.run() == WL_CONNECTED && connectionWasAlive == true)
  {
    //Serial.println(analogRead(AnalogIn));
    //angleT = float(analogRead(AnalogIn)) / 1000 * 180;
    //angleS = float(analogRead(AnalogIn)) / 1000 * 180;
    digitalWrite(LED0, LOW); //WiFi LED on
    sprintf(StatFader, "%d,%*d", ReadButton(), 4, analogRead(AnalogIn));
    //Serial.println(StatFader);
    //digitalWrite(LED1, LOW); // WiFi Communication
    /*
      Udp.beginPacket(WiFi.gatewayIP(), localUdpPort);
      Udp.write(StatFader);
      Udp.endPacket();
    */
    sendMessage(StatFader);
    Serial.println(StatFader);
    digitalWrite(LED1, HIGH); // WiFi Communication
    Serial.print("Gateway IP: ");
    Serial.println(WiFi.gatewayIP());
    Serial.print("Lokale IP: ");
    Serial.println(WiFi.localIP());

    int packetSize = Udp.parsePacket();
    if (packetSize)
    {
      // receive incoming UDP packets
      Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
      int len = Udp.read(incomingPacket, 255);
      if (len > 0)
      {
        digitalWrite(LED1, LOW); // WiFi Communication
        incomingPacket[len] = 0;
        digitalWrite(LED1, HIGH); // WiFi Communication
      }
      Serial.printf("UDP packet contents: %s\n", incomingPacket);
      myString = String(incomingPacket);
      myString1 = myString.substring(0, 4);
      rFreq = myString1.toInt();
      myString1 = myString.substring(6, 9);
      rVolt = myString1.toInt();
      myString1 = myString.substring(11, 14);
      rCurr = myString1.toInt();
      myString1 = myString.substring(16, 19);
      rVDC  = myString1.toInt();
      myString1 = myString.substring(21, 26);
      rStat = myString1.toInt() % 256; // % 256;
      myString1 = myString.substring(28, 31);
      rTorq = myString1.toInt();
      myString1 = myString.substring(33, 35);
      rTHS = myString1.toInt();
      myString1 = myString.charAt(36);
      rState = myString1.toInt();
      delay(1);
      Serial.printf("Frequency: %d, Voltage: %d, Current: %d, V_DC: %d, Gain/Stat: %d, %torque: %d, Temp_HS: %d, State: %d\n", rFreq, rVolt, rCurr, rVDC, rStat, rTorq, rTHS, rState);
      // send back a reply, to the IP address and port we got the packet from
      //Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
      //Udp.write(replyPacekt);
      //Udp.endPacket();
    } //Serial.printf("Frequency: %d, Voltage: %d, Current: %d, V_DC: %d, Gain/Stat: %d, %torque: %d, Temp_HS: %d\n", rFreq, rVolt, rCurr, rVDC, rStat, rTorq, rTHS);
    /*
        display.clearDisplay();
        disp_wifi();
        disp_tacho();
        disp_fing();
        disp_fad();
        display.display();
    */
  }
  else
  {
    WiFi.config(ip, gateway, subnet);
    delay(100);
    WiFi.mode(WIFI_STA);
    delay(100);
    WiFi.begin(ssid, password);
    display.clearDisplay();
    please_wait();
    display.display();
    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    Udp.begin(localUdpPort);
  }
  delay(1);
}

//====================================================================================
int ReadButton()
{
  // Reading The Button
  if (digitalRead(BUTTON))
  {
    //Serial.println(1);
    return 1;
  }
  else
  {
    //Serial.println(0);
    return 0;
  }
}

//====================================================================================
/*void monitorWiFi()
  {
  if (wifiMulti.run() != WL_CONNECTED)
  {
    if (connectionWasAlive == true)
    {
      connectionWasAlive = false;
      Serial.print("Looking for WiFi ");
      display.clearDisplay();
      please_wait();
      display.display();
    }
    Serial.print(".");
    if (digitalRead(LED0))
    {
      digitalWrite(LED0, LOW);
    }
    else
    {
      digitalWrite(LED0, HIGH);
    }
    delay(500);
  }
  else if (connectionWasAlive == false)
  {
    connectionWasAlive = true;
    Serial.printf(" connected to %s\n", WiFi.SSID().c_str());
    Udp.begin(localUdpPort);
    Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);
    display.clearDisplay();
    wificonnected();
    display.display();
    delay(2000);
  }
  }*/
//====================================================================================

void please_wait(void) {
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("Bitte");
  display.println();
  display.print("warten!");
  display.display();
  delay(1);
}

void start(void) {
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("Starte");
  display.println();
  display.print("Sender");
  display.display();
  delay(1);
}
void wificonnected(void) {
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("Mit WiFi-");
  display.println();
  display.print("Empfaenger");
  display.println();
  display.print("verbunden");
  display.println();
  display.printf("%d dBm", WiFi.RSSI());
  display.display();
  delay(1);
}
/*
  void speed_torque(void) {
  display.drawBitmap(0, 0, d_torque, 128, 64, WHITE);
  delay(1);
  }
*/
void disp_wifi(void) {
  if (WiFi.RSSI() > Wifistr4)
  {
    display.drawBitmap(128 - 16, 0, wifi04, 16, 16, WHITE);
  }
  else if (WiFi.RSSI() > Wifistr3)
  {
    display.drawBitmap(128 - 16, 0, wifi03, 16, 16, WHITE);
  }
  else if (WiFi.RSSI() > Wifistr2)
  {
    display.drawBitmap(128 - 16, 0, wifi02, 16, 16, WHITE);
  }
  else
  {
    display.drawBitmap(128 - 16, 0, wifi01, 16, 16, WHITE);
  }
  delay(1);
}
void disp_tacho(void) {// noch auf Modbus-Antwort anpassen
  //angleT = 45;
  //angleS = 120;
  angleT = 88 - rCurr * +0.165;
  if (rState == 1)
  {
    angleS = 88;
    //algleT =88-rCurr*+0.165;
  }
  if (rState == 2)
  {
    angleS = 88 - rFreq * 0.011;
    //algleT =88-rCurr*+0.165;
  }
  if (rState == 3)
  {
    angleS = 88 + rFreq * 0.011;
    //algleT =88-rCurr*+0.165;
  }

  float angleTrad = angleT * 0.0174833;
  float angleSrad = angleS * 0.0174833;
  display.drawBitmap(torqueCenterX - 30, torqueCenterY - 32, scal_61_33, 61, 33, WHITE);
  display.drawLine(torqueCenterX, torqueCenterY, torqueCenterX + cos(angleTrad) * 27, torqueCenterY - sin(angleTrad) * 27, WHITE);
  display.drawBitmap(speedCenterX - 30, speedCenterY - 32, scal_61_33, 61, 33, WHITE);
  display.drawLine(speedCenterX, speedCenterY, speedCenterX + cos(angleSrad) * 27, speedCenterY - sin(angleSrad) * 27, WHITE);
  delay(1);
}
void disp_fing(void) {
  if (!ReadButton())
  {
    if (fingBlink)
    {
      fingBlink = false;
      display.drawBitmap(0, 0, Fing, 16, 16, WHITE);
      delay(1);
    }
    else
    {
      fingBlink = true;
      //display.drawBitmap(0, 0, Fing, 16, 16, WHITE);
      delay(1);
    }
  }
  else
  {
    display.drawBitmap(0, 0, Fing, 16, 16, WHITE);
  }
  delay(1);
}
void disp_fad(void) {
  if (rState == 0)
  {
    if (ReadButton())
    {
      if (fadBlink)
      {
        fadBlink = false;
        display.drawBitmap(33, 0, Fader, 64, 16, WHITE);
        delay(1);
      }
      else
      {
        fadBlink = true;
        //display.drawBitmap(33, 0, Fader, 64, 16, WHITE);
        delay(1);
      }
      delay(1);
    }
  }
  else
  {
    float shift = analogRead(AnalogIn) * dspFdgain + dspoff;
    display.drawBitmap(33, 0, Fader_soc, 64, 16, WHITE);
    display.drawBitmap((int)shift, 4, Fader_K, 14, 6, WHITE); //31..85
    //display.drawBitmap(85, 4, Fader_K, 14, 6, WHITE);
    //analogRead(AnalogIn)
    Serial.println(dspFdgain);
    Serial.println(dspoff);
    Serial.println(shift);
  }
}
//-------------------------------------------------------------------------
// send Message function
//-------------------------------------------------------------------------
void sendMessage(char data[]) {     //ESP8266
  //void sendMessage(uint8_t data[]) {  //ESP32
  Udp.beginPacket(gateway, localUdpPort);
  Udp.write(data);//ESP8266
  //Udp.write(data,11);//ESP32
  Udp.endPacket();
}
/*
  void I2C_Begin()
  {
  /* ----------------------------------------------------------------------
     Setting The I2C Pins SDA, SCL
     Because We Didnt Specify Any Pins The Defult
     SDA = D4 <GPIO2>, SCL = D5 <GPIO14> For ESP8266 Dev Kit Node MCU v3
    ---------------------------------------------------------------------
  // Frequency Of The I2C
  #define TWI_FREQ  400000L       // I2C Frequency Setting To 400KHZ

  // If Ever You Needed To Use Wire I2C
  Wire.begin();                   // Begginning The I2C

  // Setting Up The I2C Of The MPU9250 ------------------------------------
  Wire.setClock(TWI_FREQ);        // Setting The Frequency MPU9250 Require
  }
*/
