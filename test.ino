#include <Basecamp.hpp>

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

Basecamp iot;
AsyncClient client;

int counter;
char* serverUrl = "172.16.1.60";
char* dataPath = "/static/content.txt";
int sleeptime = 120;
boolean wasUpdated = false;
int touchPinLeft = 2;
int touchPinRight = 27;
int touchPinBottom = 13;

int touchValueLeft = 100;
int touchValueBottom = 100;
int touchValueRight = 100;

int clockRendering = 120000;
int clockWebUpdate = 60000;
int baseClock = 0;

String displayValue = "";
int lengthPixel = 0;

void setup() {
  Serial.begin(115200);
  iot.begin();
  display.init(); // enable diagnostic output on Serial

  //Use the web object to add elements to the interface
  iot.web.addInterfaceElement("color", "input", "", "#configform", "LampColor");
  iot.web.setInterfaceElementAttribute("color", "type", "text");

  Serial.println("Hello world");
  counter = 0;

  client.setRxTimeout(10);            // 5 sec timeout
  client.onConnect(&onConnectHandler);
  client.onData(&onDataHandler);
  client.onDisconnect(&onDisconnectHandler);
  client.onTimeout(&onTimeoutHandler);
  client.onError(&onErrorHandler);

  // showFont("FreeMonoBold12pt7b", &FreeMonoBold12pt7b);

  delay(10000);

  drawBlackPixel("");

  //connectToRaspi();
}

void loop() {
  touchValueLeft = touchRead(touchPinLeft);
  if(touchValueLeft < 50) {
    Serial.println(String("Left Below 50: ") + touchValueLeft);
  }
  touchValueRight = touchRead(touchPinRight);
  if(touchValueRight < 50) {
    Serial.println(String("Right Below 50: ") + touchValueRight);
  }
  touchValueBottom = touchRead(touchPinBottom);
  if(touchValueBottom < 50) {
    Serial.println(String("Bottom Below 50: ") + touchValueBottom);
  }

  if( (baseClock % clockWebUpdate) == 0) {
    //connectToRaspi();
    //showFont("FreeMonoBold12pt7b", &FreeMonoBold12pt7b);
  }

  if( (baseClock % clockRendering) == 0) {
    counter = counter + 1;
    // showFont("FreeMonoBold12pt7b", &FreeMonoBold12pt7b);
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

void showFont(const char name[], const GFXfont* f) {
  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  display.setFont(f);
  display.setCursor(0, 0);
  display.println();
  display.println("Hello world");
  display.println(counter);
  display.println(name);
  display.update();
  delay(5000);
}

void showByteArrayWithLength(String data, size_t len, boolean start) {
  static int x = 0;
  static int y = 0;
  boolean isBlack = false;
  int pixelCounter;
  String currentCounter;
  String dataLeft = data;

  display.fillScreen(GxEPD_WHITE);

  while(dataLeft.length() > 0) {
    int indexNext = dataLeft.indexOf(";");
    if(indexNext < dataLeft.length()) {
      currentCounter = dataLeft.substring(0, indexNext);
      dataLeft = dataLeft.substring(indexNext + 1);
      pixelCounter = currentCounter.toInt();
      for(int i = 0; i < pixelCounter; i++) {
        lengthPixel += 1;
        if(isBlack == true) {
          display.drawPixel(x, y, GxEPD_BLACK);
        } else {
          // display.drawPixel(x, y, GxEPD_WHITE);
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
  Serial.println(String("New data: ") + len + String(" Bytes (Länge der übertragenen Daten)"));

  String text = String((char*) data);
  int indexContentStart = text.indexOf("Lorem");
  if(indexContentStart != -1) {
    text = text.substring(indexContentStart + 5);
  }
  int indexContentEnd = text.indexOf("ipsum");
  if(indexContentEnd != -1) {
    text = text.substring(0, indexContentEnd);
  }

  displayValue += text;
}

void onTimeoutHandler(void *r, AsyncClient *client, uint32_t timeout){
  Serial.println(String("Timeout ") + timeout);
}

void onDisconnectHandler(void *r, AsyncClient *client){
  // showByteArrayWithLength(displayValue, displayValue.length(), true);
  drawBlackPixelSimple();
  Serial.println("OnDisconnect");
  // showFont("FreeMonoBold12pt7b", &FreeMonoBold12pt7b);
}

void drawBlackPixelSimple() {
  display.fillScreen(GxEPD_WHITE);
  for(int i = 0; i < 300; i += 1 ) {
    display.drawPixel(i, 1, GxEPD_BLACK);
  }
  display.update();
}

void drawBlackPixel(String data) {
  static int x = 0;
  static int y = 0;
  boolean isBlack = false;
  int pixelCounter;
  String currentCounter;
  String dataLeft = "350;240;500;980;100;";
  dataLeft.trim();

  display.fillScreen(GxEPD_WHITE);

  while(dataLeft.length() > 0) {
    int indexNext = dataLeft.indexOf(";");
    if(indexNext < dataLeft.length()) {
      currentCounter = dataLeft.substring(0, indexNext);
      dataLeft = dataLeft.substring(indexNext + 1);
      pixelCounter = currentCounter.toInt();
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
      Serial.println(String("PixelCounter: ") + pixelCounter + "; Datalength: " + dataLeft.length());
    }
  }
  Serial.println("update now");
  display.update();
  Serial.println("updated");
}

void onErrorHandler(void *r, AsyncClient *client, int8_t error){
  Serial.println(String("Error:") + error);
}
