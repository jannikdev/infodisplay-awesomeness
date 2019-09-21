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
char* dataPath = "/static/sometext.txt";
int sleeptime = 120;
boolean wasUpdated = false;
int touchPinLeft = 2;  
int touchPinRight = 27;
int touchPinBottom = 15;

int touchValueLeft = 100;
int touchValueBottom = 100;
int touchValueRight = 100;

int clockRendering = 120000;
int clockWebUpdate = 60000;
int baseClock = 0;

void setup() {
  Serial.begin(115200);
  iot.begin();
  display.init(115200); // enable diagnostic output on Serial

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

  showFont("FreeMonoBold12pt7b", &FreeMonoBold12pt7b);
}

void loop() {
  // Serial.println("Woken up");
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
//  else {
//    Serial.println(String("Above 50: ") + touchValue); 
//  }

  if( (baseClock % clockWebUpdate) == 0) {
    if (WiFi.status() == WL_CONNECTED) {
      wasUpdated = false;
      const int httpPort = 5000;
      const char* host = serverUrl;
  
      if (!client.connect(host, httpPort)) {
        Serial.println("connection failed");
      } else {
        Serial.println("Wait till the client is connected");
      }
    } 
  }

  if( (baseClock % clockRendering) == 0) {
    counter = counter + 1;
    showFont("FreeMonoBold12pt7b", &FreeMonoBold12pt7b);
  }
  
  baseClock = baseClock + 500;
  delay(500);
  
//  esp_sleep_enable_timer_wakeup(FactorSeconds * (uint64_t) sleeptime);
//      
//  Serial.println("Going to deep sleep now...");
//  Serial.flush();
//  esp_deep_sleep_start();
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

void showByteArrayWithLength(char *data, size_t len) {
  static int x = 0; 
  static int y = 0;

  for (size_t i=0; i<len; i++){
    for (int b = 7; b >= 0; b--) {
      int bit = bitRead(data[i], b);
      if (bit == 1) {
        display.drawPixel(x, y, GxEPD_BLACK);
      } else {
        display.drawPixel(x, y, GxEPD_WHITE);
      }
      x++;
      if  (x == GxEPD_WIDTH) {
        y++;
        x = 0;
      }
    }
  }
}

/* - - - - AsyncClient Handler - - - - - - - */
void onConnectHandler(void *r, AsyncClient *client){
  Serial.println("OnConnect\n");
 
  // String url =  iot.configuration.get("ImageAddress") + "&display=" + displayType;
  String query = String("GET ") + dataPath + " HTTP/1.1\r\n" +
                 "Host: " + serverUrl + "\r\n" +
                 "Connection: close\r\n\r\n";
  Serial.println(query);
  client->write(query.c_str());
}

void onDataHandler(void *r, AsyncClient *client, void *data, size_t len){
  Serial.println(String("New data: ") + len + String(" Bytes (Länge der übertragenen Daten)"));
  Serial.println(" - - - - - - - - ");
  Serial.println((char*)data);

  String text = String((char*) data);
  int indexContentStart = text.indexOf("Lorem");
  Serial.println("Content starts at " + String(indexContentStart));
  text = text.substring(indexContentStart);
  Serial.println(" # # # TEXT # # # ");
  Serial.println(text);

  wasUpdated = true;
  // Serial.println(String("OnData: ") + len + " Bytes");
  // drawPixels((char*)data, len, false);
}

void onTimeoutHandler(void *r, AsyncClient *client, uint32_t timeout){
  Serial.println(String("Timeout ") + timeout);
}

void onDisconnectHandler(void *r, AsyncClient *client){
  Serial.println("OnDisconnect");
}

void onErrorHandler(void *r, AsyncClient *client, int8_t error){
  Serial.println(String("Error:") + error);
}