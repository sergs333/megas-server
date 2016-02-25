/*
  Modul Temperature
  part of Arduino Mega Server project
*/

#ifdef TEMP_FEATURE

#include <DallasTemperature.h>
#include <OneWire.h>

#define ONE_WIRE_BUS 2

byte const MAX_TEMP_SENSORS = 3;

byte const MAX_ID_DIGITS = 7;

byte const FULL_HEALTH = 100;
byte const ZERO_HEALTH = 0;
byte tempSensorHealth[MAX_TEMP_SENSORS];
byte tempModulHealth;

char objSens[MAX_TEMP_SENSORS][12] = {"tempSTR", "tempHOM", "tempWRM"};

// Sensors ID's
byte idTempDat[MAX_TEMP_SENSORS][MAX_ID_DIGITS] = { 
  {40, 240, 72, 81, 3, 0, 0}, // STR
  {40, 221, 16, 81, 3, 0, 0}, // SRV
  {40,  23, 98, 87, 3, 0, 0}, // WRM
}; 

float current_temp[MAX_TEMP_SENSORS];
int temp_[MAX_TEMP_SENSORS];

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress tempSensor[MAX_TEMP_SENSORS];

void tempInit() {
  initStart("Temperature");
  sensors.begin();
  timeStamp();
  Serialprint(" Parasite power ");
  if (sensors.isParasitePowerMode()) {Serialprint("ON\n");}
                                else {Serialprint("OFF\n");}
  oneWire.reset_search();

  // locate devices on the bus
  timeStamp();
  Serialprint(" Found ");
  Serial.print(sensors.getDeviceCount());
  Serialprint(" devices\n");

  for (byte i = 0; i < MAX_TEMP_SENSORS; i++) {
    sensors.getAddress(tempSensor[i], i);
    tempSensorHealth[i] = FULL_HEALTH;
  }
  tempModulHealth = FULL_HEALTH;
  oneWire.reset_search();
  
  showSensorsId();
  modulTemp = MODUL_ENABLE;
  initDone();
}

void showSensorsId() {
  for (byte n = 0; n < MAX_TEMP_SENSORS; n++) {
    timeStamp();
    Serialprint("#");
    Serial.print(n);
    Serialprint(":");
    for (byte i = 0; i < MAX_ID_DIGITS; i++) {
      Serialprint(" ");
      Serial.print(tempSensor[n][i]);
    }
    Serialprint(" ");
    Serial.println(objSens[n]);
  }
}

void setModulTempHealth() {
  int sumSensorsHealth = 0;
  for (byte i = 0; i < MAX_TEMP_SENSORS; i++) {
    sumSensorsHealth += tempSensorHealth[i];
  }
  tempModulHealth = sumSensorsHealth / MAX_TEMP_SENSORS;
}

void tempWorks() {
  sensors.requestTemperatures();
  getTemperature(0); // STR
  getTemperature(1); // HOM (SRV)
  getTemperature(2); // WRM  
  #ifdef TEMP_DEBUG
    for (byte i = 0; i < MAX_TEMP_SENSORS; i++) {
      timeStamp();
      Serialprint("Sensor ");
      Serial.print(objSens[i]);
      Serialprint(" health: ");
      Serial.println(tempSensorHealth[i]);
    }
    setModulTempHealth();
    timeStamp();
    Serialprint("Modil Temperature Health: ");
    Serial.println(tempModulHealth);
  #endif // TEMP_DEBUG
}
  
void getTemperature(byte n) {
  if (tempSensor[n][1] == idTempDat[n][1] && 
      tempSensor[n][2] == idTempDat[n][2] && 
      tempSensor[n][3] == idTempDat[n][3]) {
    tempSensorHealth[n] = FULL_HEALTH;
    current_temp[n] = sensors.getTempC(tempSensor[n]);

    if (current_temp[n] != -127) {
      if (current_temp[n] != 85) {
        timeStamp();
        Serial.print(objSens[n]);
        Serialprint(": ");
        Serial.println(current_temp[n]);
         
        temp_[n] = (current_temp[n] - (int)current_temp[n]) * 100;

        if ((current_temp[n] > -1) && (current_temp[n] < 0)) {
          #ifdef MAJORDOMO_FEATURE
            sendRequestMinusM(objSens[n], "-", (int)current_temp[n], abs(temp_[n])); 
          #endif          
        } else {
            #ifdef MAJORDOMO_FEATURE
              sendRequestMinusM(objSens[n], "", (int)current_temp[n], abs(temp_[n])); 
            #endif            
          }  

      } else { // current_temp[n] = 85
          tempSensorHealth[n] = ZERO_HEALTH;
          timeStamp();
          Serial.print(objSens[n]);
          Serialprint(": ");
          Serialprint("error\n"); 
        }  
    } else { // current_temp[n] = -127
        tempSensorHealth[n] = ZERO_HEALTH;
        timeStamp();
        Serial.print(objSens[n]);
        Serialprint(": ");
        Serialprint("removed\n"); 
      }
  } else { // current Sensor ID != stored Sensor ID
      tempSensorHealth[n] = ZERO_HEALTH;
      timeStamp();
      Serial.print(objSens[n]);
      Serialprint(": ");
      Serialprint("ID problem or not present\n");
    }
} // getTemperature( )  

#endif // TEMP_FEATURE
