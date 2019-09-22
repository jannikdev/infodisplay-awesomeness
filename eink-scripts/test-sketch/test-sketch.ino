#include <AsyncTCP.h>
#include <WiFi.h>

#include <GxEPD.h>

#include <GxGDEW075T8/GxGDEW075T8.h>      // 7.5" b/w
//#include <GxGDEW075Z09/GxGDEW075Z09.h>    // 7.5" b/w/r
//#include <GxGDEW075Z08/GxGDEW075Z08.h>    // 7.5" b/w/r 800x480
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>

#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>

#if defined(ESP8266)

// for SPI pin definitions see e.g.:
// C:\Users\xxx\AppData\Local\Arduino15\packages\esp8266\hardware\esp8266\2.4.2\variants\generic\common.h

GxIO_Class io(SPI, /*CS=D8*/ SS, /*DC=D3*/ 0, /*RST=D4*/ 2); // arbitrary selection of D3(=0), D4(=2), selected for default of GxEPD_Class
GxEPD_Class display(io, /*RST=D4*/ 2, /*BUSY=D2*/ 4); // default selection of D4(=2), D2(=4)
// Heltec E-Paper 1.54" b/w without RST, BUSY
//GxEPD_Class display(io, /*RST=D4*/ -1, /*BUSY=D2*/ -1); // no RST, no BUSY

#elif defined(ESP32)

// for SPI pin definitions see e.g.:
// C:\Users\xxx\Documents\Arduino\hardware\espressif\esp32\variants\lolin32\pins_arduino.h

GxIO_Class io(SPI, /*CS=5*/ SS, /*DC=*/ 17, /*RST=*/ 16); // arbitrary selection of 17, 16
GxEPD_Class display(io, /*RST=*/ 16, /*BUSY=*/ 4); // arbitrary selection of (16), 4

#else

// for SPI pin definitions see e.g.:
// C:\Users\xxx\AppData\Local\Arduino15\packages\arduino\hardware\avr\1.6.21\variants\standard\pins_arduino.h

GxIO_Class io(SPI, /*CS=*/ SS, /*DC=*/ 8, /*RST=*/ 9); // arbitrary selection of 8, 9 selected for default of GxEPD_Class
GxEPD_Class display(io, /*RST=*/ 9, /*BUSY=*/ 7); // default selection of (9), 7

#endif

#define FactorSeconds 1000000LL

AsyncClient client;

int counter;
char* serverUrl = "172.16.1.60";
char* dataPath = "/static/outputs/content.txt";

int touchPinLeft = 2;
int touchPinRight = 27;
int touchPinBottom = 13;
int touchValueLeft = 100;
int touchValueBottom = 100;
int touchValueRight = 100;

int clockWebUpdate = 60000;
int baseClock = 0;

String displayValue = "0;";
int lengthPixel = 0;
boolean loadedAll = false;

void setup() {
  Serial.begin(115200);
  display.init(); // enable diagnostic output on Serial

  WiFi.begin("Forum", "Hack2019");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("Started");
  counter = 0;

  client.setRxTimeout(10);            // 5 sec timeout
  client.onConnect(&onConnectHandler);
  client.onData(&onDataHandler);
  client.onDisconnect(&onDisconnectHandler);
  client.onTimeout(&onTimeoutHandler);
  client.onError(&onErrorHandler);
}

void loop() {
  touchValueLeft = touchRead(touchPinLeft);
  if(touchValueLeft < 50 && touchValueLeft != 0) {
    Serial.println(String("Left Below 50: ") + touchValueLeft);
  }
  touchValueRight = touchRead(touchPinRight);
  if(touchValueRight < 50 && touchValueRight != 0) {
    Serial.println(String("Right Below 50: ") + touchValueRight);
  }
  touchValueBottom = touchRead(touchPinBottom);
  if(touchValueBottom < 50 && touchValueBottom != 0) {
    Serial.println(String("Bottom Below 50: ") + touchValueBottom);
  }

  if( (baseClock % clockWebUpdate) == 0) {
    connectToRaspi();
    while(!loadedAll) {
      delay(500);
      baseClock += 500;
    }
    drawBlackPixelOld(displayValue);
  }

  baseClock = baseClock + 500;
  delay(500);
}

void connectToRaspi() {
  if (WiFi.status() == WL_CONNECTED) {
      const int httpPort = 5000;
      const char* host = serverUrl;

      if (!client.connect(host, httpPort)) {
        Serial.println("connection failed");
      } else {
        Serial.println("Wait till the client is connected");
      }
    } else {
      Serial.println("WLAN not conntected");
    }
}

/* - - - - AsyncClient Handler - - - - - - - */
void onConnectHandler(void *r, AsyncClient *client){
  Serial.println("OnConnect\n");

  String query = String("GET ") + dataPath + " HTTP/1.1\r\n" +
                 "Host: " + serverUrl + "\r\n" +
                 "Connection: close\r\n\r\n";
  client->write(query.c_str());
}

void onDataHandler(void *r, AsyncClient *client, void *data, size_t len){
  String text = String((char*) data);
  Serial.println(String("New data: ") + text.length() + "; Len: " + len);
  text = text.substring(0, len);
  int indexContentStart = text.indexOf("Lorem");
  if(indexContentStart != -1) {
    displayValue = "";
    loadedAll = false;
    text = text.substring(indexContentStart + 5);
  }
  int indexContentEnd = text.indexOf("ipsum");
  if(indexContentEnd != -1) {
    loadedAll = true;
    text = text.substring(0, indexContentEnd);
    Serial.println(displayValue + text);
  }
  if(!text.startsWith("HTTP")) {
    displayValue += text;
  } else {
    Serial.println(text);
  }
}

void onTimeoutHandler(void *r, AsyncClient *client, uint32_t timeout){
  Serial.println(String("Timeout ") + timeout);
}

void onDisconnectHandler(void *r, AsyncClient *client){
  Serial.println("OnDisconnect");
}

/*
void drawBlackPixelSimple() {
  Serial.println("black pixel simple");
  display.fillScreen(GxEPD_WHITE);
  for(int i = 0; i < 300; i += 1 ) {
    display.drawPixel(i, 1, GxEPD_BLACK);
  }
  display.update();
}*/

void drawBlackPixelOld(String dataLeft) {
  int x = 0;
  int y = 0;
  boolean isBlack = false;
  int pixelCounter = 0;
  String currentCounter = "";
  dataLeft.trim();

  Serial.println(String("Start draw black pixel old: ") + dataLeft.length());
  
  display.fillScreen(GxEPD_WHITE);
  display.setCursor(0,0);
  
  while(dataLeft.length() > 0) {
    int indexNextValue = dataLeft.indexOf(";");
    
    if(indexNextValue < dataLeft.length()) {
      currentCounter = dataLeft.substring(0, indexNextValue);
      pixelCounter = currentCounter.toInt();
      dataLeft = dataLeft.substring(indexNextValue + 1);
        for(int i = 0; i < pixelCounter; i++) {
          if(isBlack == true) {
            display.drawPixel(x, y, GxEPD_BLACK);
          }
          x++;
          if(x == 640) {
            y++;
            x = 0;
          }
        }
        isBlack = !isBlack;
      }
  }
  display.update();
  Serial.println("Updated screen");
}

void onErrorHandler(void *r, AsyncClient *client, int8_t error){
  Serial.println(String("Error:") + error);
}
