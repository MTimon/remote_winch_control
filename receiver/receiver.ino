// Parametrierung wieder herstellbar "node." --> "node." ersetzen

//------------------------------------------------------------------------------------
// Libraries Needed For This Project
//------------------------------------------------------------------------------------
#include <ESP8266WiFi.h>
#include <ModbusMaster.h>
#include <WiFiUdp.h>
#include <TaskScheduler.h>
//------------------------------------------------------------------------------------
// Define Demo Mode
//------------------------------------------------------------------------------------
#define     Demo      0                  // 0-normal Mode, 1-Demo Mode
//------------------------------------------------------------------------------------
// Defining I/O Pins
//------------------------------------------------------------------------------------
// Define I/O Pins
#define     LED0      16          // SDK LED
#define     LED1      2           // WIFI Module LED
#define     LED2      15          // Enclosure LED
//------------------------------------------------------------------------------------
// Defining Modbus
//------------------------------------------------------------------------------------
/*!
  We're using a MAX485-compatible RS485 Transceiver.
  Rx/Tx is hooked up to the hardware serial port at 'Serial'.
  The Data Enable and Receiver Enable pins are hooked up as follows:
*/
#define MAX485_DE     0
#define MAX485_RE_NEG 4
#define ModWait       5 //Minimum 5

//------------------------------------------------------------------------------------
// Defining Drive and Fader
//------------------------------------------------------------------------------------
#define InvDir    0     //Inverse Direction
#define MinFreq   0050  //0.5Hz
#define MaxFreq   7500  //75Hz
#define MinFader  75    //min Value Fader
#define Mid1Fader 445   //middle Value1 Fader
#define Mid2Fader 505   //middle Value2 Fader
#define MaxFader  890   //max Value Fader
#define gain1     (MinFreq-MaxFreq)/(Mid1Fader-MinFader)   //gain MinFader...Mid1Fader
#define gain2     (MaxFreq-MinFreq)/(MaxFader-Mid2Fader)   //gain Mid2Fader...MaxFader
#define offset1   MaxFreq-gain1*MinFader                   //MinFader...Mid1Fader
#define offset2   MaxFreq-gain2*MaxFader                   //offset Mid2Fader...MaxFader
#define initTout  15*4    //initial Timeout
#define stepTout  250  //iTimeout step 250ms
#define waitTout  2*4     //iTimeout wait
//------------------------------------------------------------------------------------
// UDP global new Variables
//------------------------------------------------------------------------------------
WiFiUDP Udp;
const IPAddress local(192, 168, 4, 1);
const IPAddress subnet(255, 255, 255, 0);
IPAddress ipremote(192, 168, 4, 2);
unsigned int localUdpPort = 4210;
unsigned int remoteUdpPort = 4210;
char incomingPacket[255];
char replyPacekt[] = "Hi there! Got the message :-)";
char statDrive[] = {0};
//------------------------------------------------------------------------------------
// instantiate ModbusMaster object
//------------------------------------------------------------------------------------
ModbusMaster node;

void preTransmission()
{
  digitalWrite(MAX485_RE_NEG, 1);
  digitalWrite(MAX485_DE, 1);
}

void postTransmission()
{
  digitalWrite(MAX485_RE_NEG, 0);
  digitalWrite(MAX485_DE, 0);
}
//------------------------------------------------------------------------------------
// Scheduler
//------------------------------------------------------------------------------------
// Callback methods prototypes
void t1Callback();
//Tasks
Task t1(stepTout, TASK_FOREVER, &t1Callback);
Scheduler runner;
//------------------------------------------------------------------------------------
// Custom Variables
//------------------------------------------------------------------------------------
boolean Button = false;
boolean nsetMinfreq = true;
boolean gotUDP = false;
int Fader = (Mid1Fader + Mid2Fader) / 2;
int Fader_1 = (Mid1Fader + Mid2Fader) / 2;
String myString = "";
int sec = 0;
int state = 0;
int Freq = 0;
int cnt = 0;
int iTimeout = 0;
uint8_t result;
int rFreq = 0;
int rVolt = 0;
int rCurr = 0;
int rVDC = 0;
int rStat = 0;
int rTorq = 0;
int rTHS = 0;

//====================================================================================
void t1Callback()
{
  //digitalWrite(LED2, 0);
  if (gotUDP == 0)
  {
    //Fader = (Mid1Fader + Mid2Fader) / 2;
    Button = 0;
  }
  if (Button == 1)
  {
    iTimeout = initTout;  //reset timeout
  }
  else
  {
    if (iTimeout == 0)
    {
      iTimeout = 0;
    }
    else
    {
      iTimeout -= 1;
    }
  }
  gotUDP = 0;

  //Serial.swap(); //Serial atD7 and D8
  //Serial.print("t1: ");
  //Serial.println(millis());
  //Serial.print("iTimeout: ");
  //Serial.println(iTimeout);
  if (Demo == 1)
  {
    if (sec == 0)
    {
      sec = 3;
      Serial.println();
      Serial.printf("Stations connected = %d\n", WiFi.softAPgetStationNum());
      Serial.printf("State: %d, iTimeout: %d\n", state, iTimeout);
      Serial.printf("Fmin: %d, Fmax: %d, Fader: %d, Freq: %d\n", MinFreq, MaxFreq, Fader, Freq);
      Serial.printf("gain1: %d, offset1: %d, gain2: %d, offset2: %d\n", gain1, offset1, gain2, offset2);
      Serial.printf("Frequency: %d, Voltage: %d, Current: %d, V_DC: %d, Gain/Stat: %d, %torque: %d, Temp_HS: %d\n", rFreq, rVolt, rCurr, rVDC, rStat, rTorq, rTHS);
    }
    else
    {
      sec -= 1;
    }
  }
  else
  {

  }

  //Serial.swap(); //Serial atD7 and D8
  // Read 16 registers starting at 0x3100

  /*
    if (digitalRead(LED2) == 0)
    {
    //digitalWrite(LED0, 1);
    digitalWrite(LED2, 1);
    }
    else
    {
    //digitalWrite(LED0, 0);
    digitalWrite(LED2, 0);
    }
  */
  if (Demo == 1)
  {
    rFreq = Freq;
    //delay(10);
    rVolt = 325;
    //delay(10);
    rCurr = 200;
    //delay(10);
    rVDC = 2000;
    //delay(10);
    rStat = 1000;
    //delay(10);
    rTorq = 200;
    //delay(10);
    rTHS = 300;
  }
  else
  {
    Serial.flush();
    delay(5);
    result = node.readHoldingRegisters(0x1000, 0x0008);
    //delay(150);
    if (result == node.ku8MBSuccess)
    {

      if (digitalRead(LED2) == 0)
      {
        //digitalWrite(LED0, 1);
        digitalWrite(LED2, 1);
      }
      else
      {
        //digitalWrite(LED0, 0);
        digitalWrite(LED2, 0);
      }

      rFreq = node.getResponseBuffer(0x00);
      //delay(10);
      rVolt = node.getResponseBuffer(0x01);
      //delay(10);
      rCurr = node.getResponseBuffer(0x02);
      //delay(10);
      rVDC = node.getResponseBuffer(0x04);
      //delay(10);
      rStat = node.getResponseBuffer(0x05);
      //delay(10);
      rTorq = node.getResponseBuffer(0x06);
      //delay(10);
      rTHS = node.getResponseBuffer(0x07);
      delay(10);
      Serial.flush();
      delay(5);
    }
  }
  sprintf(statDrive, "%*d,%*d,%*d,%*d,%*d,%*d,%*d,%*d", 4, rFreq, 4, rVolt, 4, rCurr, 4, rVDC, 6, rStat, 4, rTorq, 3, rTHS, 1, state);
  //sprintf(statDrive, "%*d,%*d,%*d,%*d,%*d,%*d,%*d,%*d", 4, rFreq, 4, rVolt, 4, rCurr, 4, rVDC, 6, rStat, 4, rTorq, 3, rTHS, 1, result);
  //Serial.println(statDrive);
  sendMessage(statDrive);

  //delay(100);
  /*
      Udp.beginPacket(remote, remoteUdpPort);
      //Udp.write(replyPacekt);
      Udp.write(statDrive);
      Udp.endPacket();
  */
  //Serial.println();
  //Serial.println(ipremote);
  //Serial.println(remoteUdpPort);


  //digitalWrite(LED2, 1);
}

//====================================================================================
void setup()
{
  if (Demo == 1)
  {
    pinMode(LED0, OUTPUT);
    pinMode(LED2, OUTPUT);
    digitalWrite(LED0, 1);
    digitalWrite(LED2, 1);
    // Setting The Serial Port ----------------------------------------------
    Serial.begin(9600);           // Computer Communication
    // Modbus slave ID 1
    node.begin(1, Serial);
  }
  else
  {
    // Setting Modbus -------------------------------------------------------
    pinMode(MAX485_RE_NEG, OUTPUT);
    pinMode(MAX485_DE, OUTPUT);
    pinMode(LED0, OUTPUT);
    pinMode(LED2, OUTPUT);
    // Init in receive mode
    digitalWrite(MAX485_RE_NEG, 0);
    digitalWrite(MAX485_DE, 0);
    digitalWrite(LED0, 1);
    digitalWrite(LED2, 1);
    // Setting The Serial Port ----------------------------------------------
    Serial.begin(9600);           // Computer Communication
    // Modbus slave ID 1
    node.begin(1, Serial);

    // Callbacks allow us to configure the RS485 transceiver correctly
    node.preTransmission(preTransmission);
    node.postTransmission(postTransmission);
    delay(100);
    // Setting The Mode Of Pins ---------------------------------------------
    //pinMode(LED0, OUTPUT);          // WIFI OnBoard LED Light
    //pinMode(LED1, OUTPUT);          // Indicator For Client #1 Connectivity
    //pinMode(LED2, OUTPUT);          // Indicator For Client #2 Connectivity
    //pinMode(BUTTON, INPUT_PULLUP);  // Initiate Connectivity
    //Serial.println(node.getResponseBuffer(0x04));
    // Drive Initialisation -------------------------------------------------
    delay(ModWait);
    node.writeSingleRegister(0x2000, 0x0009);//Reset
    delay(ModWait);
    //Serial.println(node.getResponseBuffer(0x04));
    delay(ModWait);
    node.writeSingleRegister(0x010C, MinFreq);//Minimalfrequenz
    delay(ModWait);
    //Serial.println(node.getResponseBuffer(0x04));
    delay(ModWait);
    node.writeSingleRegister(0x010B, MaxFreq);//Maximalfrequenz
    delay(ModWait);
    //Serial.println(node.getResponseBuffer(0x04));
    delay(ModWait);
    node.writeSingleRegister(0x0203, 0x000A);//Main frequency source X Modbus
    delay(ModWait);
    //Serial.println(node.getResponseBuffer(0x04));
    delay(ModWait);
    delay(ModWait);
    node.writeSingleRegister(0x010E, 0x0005);//1st Accelaration time 0,5s
    delay(ModWait);
    delay(ModWait);
    node.writeSingleRegister(0x010F, 0x0005);//1st Deccelaration time 0,5s
    delay(ModWait);

    delay(ModWait);
    node.writeSingleRegister(0x0905, 0x0035);//Communication-Timeout 3,5s
    delay(ModWait);
   
    Serial.println();
    Serial.println("Umrichter initialisiert");
  }
  // Print Message Of I/O Setting Progress
  Serial.println();
  Serial.println("I/O Pins Modes Set .... Done");

  Serial.print("Setting soft-AP ... ");
  WiFi.mode(WIFI_AP);//eingefügt
  WiFi.softAPConfig(local, local, subnet);//eingefügt
  boolean result = WiFi.softAP("ESPsoftAP_01", "pass-to-soft-AP");
  if (result == true)
  {
    Serial.println("Ready");
    delay(1000);
    Udp.begin(localUdpPort);
    Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.softAPIP().toString().c_str(), localUdpPort);
    Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);
  }
  else
  {
    Serial.println("Failed!");
  }
  runner.init();
  runner.addTask(t1);
  t1.enable();
  Serial.println("Enabled t1");
  Serial.println("Initialisierung abgeschlossen");
  //Serial.swap(); //Serial atD7 and D8
}

//====================================================================================

void loop()
{
  runner.execute();
  int packetSize = Udp.parsePacket();
  if (packetSize)
  {
    // receive incoming UDP packets
    //Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
    int len = Udp.read(incomingPacket, 255);
    if (len > 0)
    {
      incomingPacket[len] = 0;
      gotUDP = 1;
    }
    //Serial.printf("UDP packet contents: %s\n", incomingPacket);
    //Reply
    //Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    //Udp.write(replyPacekt);
    //Udp.endPacket();
    // Read and check variable Button -------------------------------------------

    myString = String(incomingPacket).charAt(0);
    //if (String(incomingPacket).charAt(0) == "1")
    if (myString.substring(0) == "1")
    {
      Button = true;
    }
    else if (myString.substring(0) == "0")
    {
      Button = false;
    }
    else
    {
      Button = false;
    }

    //Serial.printf("Button is: %d\n", Button);
    //Serial.println(myString);

    // Read and check variable Fader -------------------------------------------
    myString = String(incomingPacket);
    myString = myString.substring(myString.length() - 4, myString.length());
    myString.replace(" ", "");
    Fader = myString.toInt();
    if ((Fader > MinFader) || (Fader < MaxFader))
    {

    }
    else {
      Fader = (Mid1Fader + Mid2Fader) / 2;
    }
  }
  //remote = IPAddress(192, 168, 4, 100);
  // State Machine -----------------------------------------------------------
  switch (state) { //0-Disable, 1-Enable, 2-Forward, 3-Reverse
    case 0: // Disable
      delay(ModWait);
      if (Demo == 1)
      {

      }
      else
      {
        node.writeSingleRegister(0x2000, 0x0003); //DecStop
      }
      delay(ModWait);
      if (nsetMinfreq)
      {
        delay(ModWait);
        if (Demo == 1)
        {

        }
        else
        {
          node.writeSingleRegister(0x010D, MinFreq);
        }
        delay(ModWait);
        Freq = MinFreq;
        nsetMinfreq = false;
      }
      if ((Button == 1) && ((Fader > Mid1Fader) && (Fader < Mid2Fader)))
      {
        state = 1;
      }
      break;

    case 1: // Enable
      if (nsetMinfreq)
      {
        delay(ModWait);
        if (Demo == 1)
        {

        }
        else
        {
          node.writeSingleRegister(0x010D, MinFreq);
        }
        delay(ModWait);
        Freq = MinFreq;
        nsetMinfreq = false;
      }
      delay(ModWait);
      if (Demo == 1)
      {

      }
      else
      {
        if (InvDir == 0)
        {

          node.writeSingleRegister(0x2000, 0x0001); //StartForward
        }
        else
        {
          node.writeSingleRegister(0x2000, 0x0002); //StartReverse
        }
      }
      delay(ModWait);
      if (iTimeout == 0)
      {
        delay(ModWait);
        if (Demo == 1)
        {

        }
        else
        {
          node.writeSingleRegister(0x2000, 0x0003); //DecStop
        }
        delay(ModWait);
        state = 0;
      }
      else if (Fader != Fader_1)
      {
        Fader_1 = Fader;
        if ((Fader >= MinFader) && (Fader <= Mid1Fader))
        {
          state = 2;
        }
        else if ((Fader >= Mid2Fader) && (Fader <= MaxFader))
        {
          delay(ModWait);
          if (Demo == 1)
          {

          }
          else
          {
            if (InvDir == 0)
            {
              node.writeSingleRegister(0x2000, 0x0002); //StartReverse
            }
            else
            {
              node.writeSingleRegister(0x2000, 0x0001); //StartForward
            }
            delay(ModWait);
          }
          state = 3;
        }
      }
      break;

    case 2: //Forward
      if ((iTimeout <= initTout - waitTout) || (Fader > Mid1Fader))
      {
        delay(ModWait);
        if (Demo == 1)
        {

        }
        else
        {
          node.writeSingleRegister(0x010D, MinFreq);
        }
        delay(ModWait);
        Freq = MinFreq;
        nsetMinfreq = false;
        state = 1;
      }
      else if (Fader != Fader_1)
      {
        Fader_1 = Fader;
        if ((Fader >= MinFader) && (Fader <= Mid1Fader))
        {
          Freq = Fader * gain1 + offset1;
          delay(ModWait);
          //node.writeSingleRegister(0x010D, 0x0FA0);
          if (Demo == 1)
          {

          }
          else
          {
            node.writeSingleRegister(0x010D, Freq);
          }
          delay(ModWait);
        }
        else if (Fader < 50 || Fader > 950) //dynamisch anpassen
        {
          delay(ModWait);
          if (Demo == 1)
          {

          }
          else
          {
            node.writeSingleRegister(0x010D, MinFreq);
          }
          delay(ModWait);
          Freq = MinFreq;
          nsetMinfreq = false;
          state = 1;
        }
      }
      break;
    case 3: //Reverse
      if ((iTimeout <= initTout - waitTout) || (Fader < Mid2Fader))
      {
        delay(ModWait);
        if (Demo == 1)
        {

        }
        else
        {
          node.writeSingleRegister(0x010D, MinFreq);
        }
        delay(ModWait);
        nsetMinfreq = false;
        state = 1;
      }
      else if (Fader != Fader_1)
      {
        Fader_1 = Fader;
        if ((Fader >= Mid2Fader) && (Fader <= MaxFader))
        {
          Freq = Fader * gain2 + offset2;
          delay(ModWait);
          if (Demo == 1)
          {

          }
          else
          {
            //node.writeSingleRegister(0x010D, 0x03E8);
            node.writeSingleRegister(0x010D, Freq);
          }
          delay(ModWait);
          nsetMinfreq = true;
        }
        else if (Fader < 50 || Fader > 950)// dynamische Anpassung
        {
          delay(ModWait);
          if (Demo == 1)
          {

          }
          else
          {
            node.writeSingleRegister(0x010D, MinFreq);
          }
          delay(ModWait);
          Freq = MinFreq;
          nsetMinfreq = false;
          state = 1;
        }
      }
      break;
  }

  if (digitalRead(LED0) == 0)
  {
    digitalWrite(LED0, 1);
    //digitalWrite(LED2, 1);
  }
  else
  {
    digitalWrite(LED0, 0);
    //digitalWrite(LED2, 0);
  }
  /*
    cnt++;
    if (cnt == 20) {
    cnt = 0;
    delay(ModWait);
    result = node.readHoldingRegisters(0x1000, 0x0008);
    delay(ModWait);
    if (result == node.ku8MBSuccess)
    {
      rFreq = node.getResponseBuffer(0x00);
      rVolt = node.getResponseBuffer(0x01);
      rCurr = node.getResponseBuffer(0x02);
      rVDC = node.getResponseBuffer(0x04);
      rStat = node.getResponseBuffer(0x05);
      rTorq = node.getResponseBuffer(0x06);
      rTHS = //node.getResponseBuffer(0x07);
    }

    sprintf(statDrive, "%*d,%*d,%*d,%*d,%*d,%*d,%*d", 4, rFreq, 4, rVolt, 4, rCurr, 4, rVDC, 6, rStat, 4, rTorq, 3, rTHS);

    //delay(299);
    Serial.println(statDrive);
    Serial.println(cnt);
    //sendMessage(statDrive);
    Udp.beginPacket(ipremote, remoteUdpPort);
    Udp.write(statDrive);
    Udp.endPacket();
    }
    /*
    Serial.print("Testzeit: ");
    Serial.println(sec);
    Serial.printf("State: %d, iTimeout: %d\n", state, iTimeout);
    Serial.printf("Fmin: %d, Fmax: %d, Fader: %d, Freq: %d\n", MinFreq, MaxFreq, Fader, Freq);
    Serial.printf("gain1: %d, offset1: %d, gain2: %d, offset2: %d\n", gain1, offset1, gain2, offset2);
    //delay(ModWait);
  */


  //delay(100);
}
//====================================================================================

//     remote=(192,168,4,100)
void sendMessage(char data[]) {
  /*
    WiFiUDP port2;
    port2.beginPacket(remote, remoteUdpPort);
    port2.write(data);
    port2.endPacket();
  */
  Udp.beginPacket(ipremote, remoteUdpPort);
  Udp.write(data);
  Udp.endPacket();
}
