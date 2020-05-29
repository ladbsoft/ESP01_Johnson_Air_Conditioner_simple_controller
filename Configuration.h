// +------------------------------------------------------------------+
// |                       C O N S T A N T S                          |
// +------------------------------------------------------------------+

//MQTT
const char* mqttServer            = "<YourMQTTServerIP>";
const int   mqttPort              = 1883;
const char* mqttUser              = "<YourMQTTUser>";
const char* mqttPassword          = "<YourMQTTPassword>";
const char* mqttClientId          = "<ANewMQTTClientID>";
const char* mqttPowerStateTopic   = "<ANewMQTTTopic>";
const char* mqttPowerCommandTopic = "<ANewMQTTTopic2>";
const char* mqttSwingStateTopic   = "<ANewMQTTTopic3>";
const char* mqttSwingCommandTopic = "<ANewMQTTTopic4>";
const char* mqttModeStateTopic    = "<ANewMQTTTopic5>";
const char* mqttModeCommandTopic  = "<ANewMQTTTopic6>";
const char* mqttTempStateTopic    = "<ANewMQTTTopic7>";
const char* mqttTempCommandTopic  = "<ANewMQTTTopic8>";
const char* mqttSpeedStateTopic   = "<ANewMQTTTopic9>";
const char* mqttSpeedCommandTopic = "<ANewMQTTTopic10>";

//GPIO
const byte  IRSenderPin   = 3; //RX

// WI-FI CONFIGURATION NETWORK. IF THE ESP01 CANNOT CONNECT TO THE
// LAST WI-FI SPOT USED, A NEW CONNECTION WITH THIS SSID AND
// PASSWORD IS CREATED ON THE ESP01, SO YOU CAN CONNECT TO IT AND
// CONFIGURE THE NEW CONNECTION
const char* wifiSsid     = "<NewSSID>";
const char* wifiPassword = "<NewPassword>";
