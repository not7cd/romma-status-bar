#include <Arduino.h>
#include <LeifHomieLib.h>

#if defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#else
#include "WiFi.h"
#endif

#include <ArduinoOTA.h>
#include <initializer_list>
#include <string>

#include "pinDefs.h"
#include "secrets.h"
#include "config.h"
#include "commonFwUtils.h"

#include <Adafruit_NeoPixel.h>
  

HomieDevice homie;

#define NEO_PIN D1
#define NEO_NUM 6

void init_homie_led(HomieNode* pNode, int pin_num, String id){
  Button* pButton = new Button(pin_num, INPUT);
  HomieProperty *pProperty = pNode->NewProperty();

  pProperty->strFriendlyName = id;
  pProperty->strID = id;
  pProperty->datatype = homieBool;
  pProperty->SetBool(pButton->isPressed());
  pProperty->strFormat = "";
  pButton->onChange([pButton,pProperty]() {
    pProperty->SetBool(pButton->isPressed());
  });
}

void init_buzzer(HomieNode* pNode, HomieProperty* pProperty, String id){
  pProperty = pNode->NewProperty();
  pProperty->strFriendlyName = "Annoying " + id;
  pProperty->strID = id;
  pProperty->SetRetained(true);
  pProperty->SetSettable(true);
  pProperty->datatype = homieBool;
  pProperty->SetBool(false);
  pProperty->strFormat = "";
  //callback is actually not needed cause buzzer is handled in loop
  // pPropBuzzer->AddCallback([](HomieProperty *pSource) {
  // 	Serial.printf("%s is now %s\n",pSource->strFriendlyName.c_str(),pSource->GetValue().c_str()); 
  // });
}

void init_homie_stuff(HomieDevice* pHomie){
  pHomie->strFriendlyName = friendlyName;
  #if defined(APPEND_MAC_TO_HOSTNAME)
    char out[20];
    sprintf(out, "%s-%X",hostname, ESP.getChipId());
    pHomie->strID = out;
  #else
    pHomie->strID = hostname;
  #endif
  pHomie->strID.toLowerCase();

  pHomie->strMqttServerIP = "192.168.88.170";
	pHomie->strMqttUserName = MQTT_USERNAME;
	pHomie->strMqttPassword = MQTT_PASSWD;
  pHomie->Init();
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_LED_BUILTIN, OUTPUT);

  WiFi.mode(WIFI_STA);
  WiFi.begin(IOT_WIFI_NAME, IOT_WIFI_PASSWD);
  while (WiFi.waitForConnectResult(3000) != WL_CONNECTED) {
    static bool flag = false;
    digitalWrite(PIN_LED_BUILTIN,flag);
    flag = !flag;
  }

  begin_hspota();    
  HomieNode *pNode = homie.NewNode();
  pNode->strID = "properties";
  pNode->strFriendlyName = "Properties";
  init_homie_stuff(&homie);  

  pixels.begin(); 
}

void loop() {
  if(homie.IsConnected()){
    handle_io_pattern(PIN_LED_BUILTIN, PATTERN_HBEAT);
  } else {
    handle_io_pattern(PIN_LED_BUILTIN, PATTERN_ERR);
  }

  ArduinoOTA.handle();
  homie.Loop();
  pixels.show();
  delay(100);
}
