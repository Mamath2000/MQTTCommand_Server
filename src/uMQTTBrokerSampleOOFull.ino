/*
 * uMQTTBroker demo for Arduino (C++-style)
 * 
 * The program defines a custom broker class with callbacks, 
 * starts it, subscribes locally to anything, and publishs a topic every second.
 * Try to connect from a remote client and publish something - the console will show this as well.
 */

#include <ESP8266WiFi.h>
#include "uMQTTBroker.h"
#include "Servo.h"
#include <ArduinoJson.h>

/*
 * Your WiFi config here
 */
char ssid[] = "Livebox-19E2";       // your network SSID (name)
char pass[] = "hcTzdCTZAqgVLxmKbM"; // your network password
bool WiFiAP = false;                // Do yo want the ESP as AP?

String mainTopic = "proto1";


/*
 * Servo conf
 */
int servoPin = D7;
Servo Servo1;
int servoVal1;

StaticJsonDocument<200> jsonDoc;

void manageMove(int value);

/*
 * Custom broker class with overwritten callback functions
 */
class myMQTTBroker : public uMQTTBroker
{
public:
  virtual bool onConnect(IPAddress addr, uint16_t client_count)
  {
    Serial.println(addr.toString() + " connected");
    return true;
  }

  virtual bool onAuth(String username, String password)
  {
    Serial.println("Username/Password: " + username + "/" + password);
    return true;
  }

  virtual void onData(String topic, const char *data, uint32_t length)
  {
    char data_str[length + 1];
    os_memcpy(data_str, data, length);
    data_str[length] = '\0';

//    Serial.println("received topic '" + topic + "' with data '" + (String)data_str + "'");
    deserializeJson(jsonDoc, (String)data_str);

      int x1 = jsonDoc["x1"];
      manageMove(x1);

  }
};

myMQTTBroker myBroker;

/*
 * Manage movement of servos
 */
void manageMove(int value){
  digitalWrite(LED_BUILTIN, HIGH);
  servoVal1 = (long)value; //map(value, 0 , 1023, 0, 180);
  //Serial.println("value : " + (String)servoVal1);
  Servo1.write(servoVal1);
  //myBroker.publish("stat/" + mainTopic + "/move", (String)servoVal1);
  digitalWrite(LED_BUILTIN, LOW);

}


/*
 * WiFi init stuff
 */
void startWiFiClient()
{
  Serial.println("Connecting to " + (String)ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");

  Serial.println("WiFi connected");
  Serial.println("IP address: " + WiFi.localIP().toString());
}

void startWiFiAP()
{
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, pass);
  Serial.println("AP started");
  Serial.println("IP address: " + WiFi.softAPIP().toString());
}

void initServo()
{
  Servo1.attach(servoPin);
  Servo1.write(0);
  delay(500);
  Servo1.write(180);
  delay(500);
  Servo1.write(90);
}

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT); // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  Serial.println();
  Serial.println();

  // Start WiFi
  if (WiFiAP)
    startWiFiAP();
  else
    startWiFiClient();

  // Start the broker
  Serial.println("Starting MQTT broker");
  myBroker.init();
  
  delay(2000);
  
  myBroker.publish("debug/txt", "Démarrage du serveur");

  initServo();

/*
 * Subscribe to anything
 */
  myBroker.subscribe("cmnd/" + mainTopic + "/move");
}

void loop()
{
  /*
 * Publish the counter value as String
 */

}
