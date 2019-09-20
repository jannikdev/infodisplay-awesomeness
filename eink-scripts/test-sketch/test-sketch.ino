#include <Basecamp.hpp>
Basecamp iot;

void setup() {
  Serial.begin(115200);
  
  iot.begin();
  //The mqtt object is an instance of Async MQTT Client. See it's documentation for details.
  iot.mqtt.subscribe("test/lol",2);

  //Use the web object to add elements to the interface
  iot.web.addInterfaceElement("color", "input", "", "#configform", "LampColor");
  iot.web.setInterfaceElementAttribute("color", "type", "text");
  Serial.println("Hello world");

}

void loop() {
  //your code
}
