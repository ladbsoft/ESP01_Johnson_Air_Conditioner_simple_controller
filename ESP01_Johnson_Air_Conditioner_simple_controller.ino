// #==================================================================#
// ‖ Author: Luis Alejandro Domínguez Bueno (LADBSoft)                ‖
// ‖ Date: 2020-05-27                                    Version: 1.0 ‖
// #==================================================================#
// ‖ Name: ESP8266 MQTT johnson air conditioner simple controller     ‖
// ‖ Description: A sketch for the ESP8266 (ESP-01 to be exact) for   ‖
// ‖ adding IoT functionality to a johnson classic air conditioner.   ‖
// ‖ The desired functions are ON, OFF, set mode, set speed and set   ‖
// ‖ temperature.                                                     ‖
// ‖ The microcontroller will be connected to an MQTT server, for     ‖
// ‖ sending status and receiving orders.                             ‖
// ‖                                                                  ‖
// ‖ The victim machine is a Johnson MGE-30-BC, and I tested with a   ‖
// ‖ MGE-20-BC too with positive results :)                           ‖
// #==================================================================#
// ‖ Version history:                                                 ‖
// #==================================================================#
// ‖ 1.0:  First stable version released.                             ‖
// #==================================================================#

// +------------------------------------------------------------------+
// |                        I N C L U D E S                           |
// +------------------------------------------------------------------+
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include "Configuration.h"
#include "Commands.h"

// +------------------------------------------------------------------+
// |                         G L O B A L S                            |
// +------------------------------------------------------------------+

WiFiClient espClient;
PubSubClient client(espClient);
long lastCheck = 0;
unsigned long command;

// +------------------------------------------------------------------+
// |                           S E T U P                              |
// +------------------------------------------------------------------+

void setup() {
  //Disable Serial pins in order to use them as GPIO
  pinMode(1, FUNCTION_3); //TX
  pinMode(3, FUNCTION_3); //RX

  pinMode(IRSenderPin, OUTPUT);    // For sending commands

  setup_wifi();
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  statesInit();
}

// +------------------------------------------------------------------+
// |                            L O O P                               |
// +------------------------------------------------------------------+

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastCheck > 5000) {
    lastCheck = now;

    publishStates();
  }
}

// +------------------------------------------------------------------+
// |                     S U B R O U T I N E S                        |
// +------------------------------------------------------------------+

void setup_wifi() {
  WiFiManager wifiManager;
  wifiManager.setTimeout(180); //3 minutes

  if(!wifiManager.autoConnect(wifiSsid, wifiPassword)) {
    //Retry after 3 minutes with no WiFi connection
    ESP.reset();
    delay(5000);
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  String topicString = String(topic);
  byte* payloadZeroTerm = (byte*)malloc(length+1);
  String payloadString;
  byte payloadByte;

// Conversion of payload to String
  memcpy(payloadZeroTerm, payload, length);
  payloadZeroTerm[length] = '\0';
  payloadString = String((char*)payloadZeroTerm);

// Conversion of payload to Byte
  payloadByte = (byte)payloadString.toInt();

// Power Topic: Payload will be "ON" or "OFF"
  if(topicString.equals(String(mqttPowerCommandTopic))) {
    if (payloadString.equals("ON")) {
      setPowerState(true);
    } else if (payloadString.equals("OFF")) {
      setPowerState(false);
    }

// Swing Topic: Payload will be "ON" or "OFF"
  } else if(topicString.equals(String(mqttSwingCommandTopic))) {
    if (payloadString.equals("ON")) {
      setSwingState(true);
    } else if (payloadString.equals("OFF")) {
      setSwingState(false);
    }

// Speed Topic: Payload will be "AUTO", "LOW", "MED", or "HIGH"
  } else if(topicString.equals(String(mqttSpeedCommandTopic))) {
    if (payloadString.equals("AUTO")) {
      setSpeedState(0);
    } else if (payloadString.equals("LOW")) {
      setSpeedState(1);
    } else if (payloadString.equals("MED")) {
      setSpeedState(2);
    } else if (payloadString.equals("HIGH")) {
      setSpeedState(3);
    }

// Mode Topic: Payload will be "AUTO", "COOL", "FAN", "HEAT" or "DRY"
  } else if(topicString.equals(String(mqttModeCommandTopic))) {
    if (payloadString.equals("AUTO")) {
      setModeState(0);
    } else if (payloadString.equals("COOL")) {
      setModeState(1);
    } else if (payloadString.equals("DRY")) {
      setModeState(2);
    } else if (payloadString.equals("FAN")) {
      setModeState(3);
    } else if (payloadString.equals("HEAT")) {
      setModeState(4);
    }

// Temperature Topic: Payload will be a number corresponding to the desired temperature, between 16 and 30
  } else if(topicString.equals(String(mqttTempCommandTopic))) {
    if (payloadByte >= 16 && payloadByte <= 30) {
      setTempState(payloadByte);
    }
  }

  publishStates();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    // Attempt to connect
    if (client.connect(mqttClientId, mqttUser, mqttPassword)) {
      // Once connected, resubscribe
      client.subscribe(mqttPowerCommandTopic);
      client.subscribe(mqttSwingCommandTopic);
      client.subscribe(mqttModeCommandTopic);
      client.subscribe(mqttTempCommandTopic);
      client.subscribe(mqttSpeedCommandTopic);
    } else {
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void publishStates() {
//  Publish power state
  if (powerState) {
    client.publish(mqttPowerStateTopic, "ON");
  } else {
    client.publish(mqttPowerStateTopic, "OFF");
  }

//  Publish swing state
  if (swingState) {
    client.publish(mqttSwingStateTopic, "ON");
  } else {
    client.publish(mqttSwingStateTopic, "OFF");
  }

//  Publish mode
  if (modeState == 0) {
    client.publish(mqttModeStateTopic, "AUTO");
  } else if (modeState == 1) {
    client.publish(mqttModeStateTopic, "COOL");
  } else if (modeState == 2) {
    client.publish(mqttModeStateTopic, "DRY");
  } else if (modeState == 3) {
    client.publish(mqttModeStateTopic, "FAN");
  } else if (modeState == 4) {
    client.publish(mqttModeStateTopic, "HEAT");
  }

//  Publish temperature
  client.publish(mqttTempStateTopic, String(tempState).c_str());

//  Publish speed
  if (speedState == 0) {
    client.publish(mqttSpeedStateTopic, "AUTO");
  } else if (speedState == 1) {
    client.publish(mqttSpeedStateTopic, "LOW");
  } else if (speedState == 2) {
    client.publish(mqttSpeedStateTopic, "MED");
  } else if (speedState == 3) {
    client.publish(mqttSpeedStateTopic, "HIGH");
  }
}
