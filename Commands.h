// #==================================================================#
// ‖ After a bit of decoding, the data packets seem to have this      ‖
// ‖ structure:                                                       ‖
// ‖                                                                  ‖
// ‖ Bits 0 - 2 (counting from MSB to LSB): Mode                      ‖
// ‖ 100 - COLD                                                       ‖
// ‖ 010 - DRY                                                        ‖
// ‖ 110 - FAN                                                        ‖
// ‖ 001 - HEAT                                                       ‖
// ‖ 000 - AUTO                                                       ‖
// ‖                                                                  ‖
// ‖ Bit 3: On/Off                                                    ‖
// ‖ 1 - ON                                                           ‖
// ‖ 0 - OFF                                                          ‖
// ‖                                                                  ‖
// ‖ Bits 4 and 5: Speed                                              ‖
// ‖ 10 - LOW                                                         ‖
// ‖ 01 - MEDIUM                                                      ‖
// ‖ 11 - HIGH                                                        ‖
// ‖ 00 - AUTO                                                        ‖
// ‖                                                                  ‖
// ‖ Bit 6: Swing                                                     ‖
// ‖ 1 - ENABLED                                                      ‖
// ‖ 0 - DISABLED                                                     ‖
// ‖                                                                  ‖
// ‖ Bits 8 - 11: Temperature                                         ‖
// ‖ 0000 - 16ºC                                                      ‖
// ‖ 1000 - 17ºC                                                      ‖
// ‖ 0100 - 18ºC                                                      ‖
// ‖ 1100 - 19ºC                                                      ‖
// ‖ 0010 - 20ºC                                                      ‖
// ‖ 1010 - 21ºC                                                      ‖
// ‖ 0110 - 22ºC                                                      ‖
// ‖ 1110 - 23ºC                                                      ‖
// ‖ 0001 - 24ºC                                                      ‖
// ‖ 1001 - 25ºC                                                      ‖
// ‖ 0101 - 26ºC                                                      ‖
// ‖ 1101 - 27ºC                                                      ‖
// ‖ 0011 - 28ºC                                                      ‖
// ‖ 1011 - 29ºC                                                      ‖
// ‖ 0111 - 30ºC                                                      ‖
// ‖                                                                  ‖
// ‖ Bits 28 - 31: End of command                                     ‖
// ‖ 1010 - Fixed value                                               ‖
// ‖                                                                  ‖
// ‖ The packet size is 32 bits, and the protocol is NEC, so we can   ‖
// ‖ directly use IRremote's sendNEC function with the 32 bit value.  ‖
// #==================================================================#

// +------------------------------------------------------------------+
// |                       C O N S T A N T S                          |
// +------------------------------------------------------------------+

/**
 * Command frecuency seems to be 38KHz
 */
#define KHZ 38

/**
 * Another weird point here is the value for COOL here is the same as the value for OFF mode. Oh well...
 */
const byte modeCommandValues[] { 0b00000000,    //AUTO -> 000
                                 0b00000100,    //COOL -> 100
                                 0b00000010,    //DRY  -> 010
                                 0b00000110,    //FAN  -> 110
                                 0b00000001 };  //HEAT -> 001

/**
 * Speed values follow a reverse binary count, being 00 = AUTO and counting up 01 LOW,
 * which once reversed gives 10.
 */
const byte speedCommandValues[] { 0b00000000,   //AUTO -> 00
                                  0b00000010,   //LOW  -> 10
                                  0b00000001,   //MED  -> 01
                                  0b00000011 }; //HIGH -> 11

/**
 * Temperatures follow a reverse binary count, being 0000 = 16ºC and counting up 0001,
 * which once reversed gives 1000.
 */
const byte temperatureCommandValues[] { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  //Filler positions
                                        0b00000000,                       //16ºC -> 0000
                                        0b00001000,                       //17ºC -> 1000
                                        0b00000100,                       //18ºC -> 0100
                                        0b00001100,                       //19ºC -> 1100
                                        0b00000010,                       //20ºC -> 0010
                                        0b00001010,                       //21ºC -> 1010
                                        0b00000110,                       //22ºC -> 0110
                                        0b00001110,                       //23ºC -> 1110
                                        0b00000001,                       //24ºC -> 0001
                                        0b00001001,                       //25ºC -> 1001
                                        0b00000101,                       //26ºC -> 0101
                                        0b00001101,                       //27ºC -> 1101
                                        0b00000011,                       //28ºC -> 0011
                                        0b00001011,                       //29ºC -> 1011
                                        0b00000111 };                     //30ºC -> 0111

// +------------------------------------------------------------------+
// |                         G L O B A L S                            |
// +------------------------------------------------------------------+
boolean powerState;
boolean swingState;
byte tempState;
byte modeState;
byte speedState;
IRsend irsend(IRSenderPin);

// +------------------------------------------------------------------+
// |                         H E A D E R S                            |
// +------------------------------------------------------------------+
void setPowerState(boolean newPowerState);
void setSwingState(boolean newSwingState);
void setTempState(byte newTemperature);
void setSpeedState(byte newSpeed);
void setModeState(byte newMode);
void sendCommand();
void statesInit();

// +------------------------------------------------------------------+
// |                     S U B R O U T I N E S                        |
// +------------------------------------------------------------------+
void setPowerState(boolean newPowerState) {
  powerState = newPowerState;

  sendCommand();
}

void setSwingState(boolean newSwingState) {
  swingState = newSwingState;

  if (powerState) {
    sendCommand();
  }
}

void setTempState(byte newTemperature) {
  tempState = newTemperature;

  if (powerState) {
    sendCommand();
  }
}

void setSpeedState(byte newSpeed) {
  speedState = newSpeed;

  if (powerState) {
    sendCommand();
  }
}

void setModeState(byte newMode) {
  modeState = newMode;

  if (powerState) {
    sendCommand();
  }
}

void sendCommand() {
  uint64_t command;
  command = modeCommandValues[modeState];             //Mode
  command <<= 1;
  command |= (powerState?1:0);                        //On-Off
  command <<= 2;
  command |= speedCommandValues[speedState];          //Speed
  command <<= 1;
  command |= (swingState?1:0);                        //Swing enabled/disabled
  command <<= 5;
  if (modeState != 0) {
    command |= temperatureCommandValues[tempState];   //Temperature
  } else {
    command |= temperatureCommandValues[25];          //25ºC for auto mode
  }
  command <<= 20;
  command |= 0b1010;                                  //End of command
  command <<= 3;

  irsend.sendNEC(command, 35);
  delay(40);
}

// +------------------------------------------------------------------+
// |                           S E T U P                              |
// +------------------------------------------------------------------+

void statesInit() {
  powerState = false; //OFF
  swingState = false; //OFF
  tempState  = 23;    //23ºC
  modeState  = 0;     //AUTO
  speedState = 0;     //AUTO
}
