/** @brief Metoder.
*
*  Metoder der beregner temperatur i high resolution.
*
*  @note Nedentående script er modificeret kode fra denne kilde: https://bitbucket.org/robert_vancuren_jr/ds1621-digital-thermometer/src/master/DS1621-temp.ino
*  @warning Ved: Your connection was interrupted. Prøv at upload igen.
*/

///Commands
#define READ_TEMPERATURE 0xAA
#define ACCESS_TH 0xAa1
#define ACCESS_TL 0xA2
#define ACCESS_CONFIG 0xAC
#define READ_COUNTER 0xA8
#define READ_SLOPE 0xA9
#define START_CONVERT 0xEE
#define STOP_CONVERT 0x22

//DS1621 4-bit control code is 1001
//The next 3 bits are the device select bits A2 A1 A0
//The Wire library handles setting the read/write bit
//which means we only need the 7 bits
#define SENSOR B1001000

/// Listener er initialiseret
byte oneShotConfig = B00000001;

/// Setup metode
void tempSetup() {       
  Wire.begin();   
  writeConfig(SENSOR, oneShotConfig);
}

/** @brief Metode tempLoop.
 *
 *  Retunerer high resolution temperatur.
 *
 *  @return float.   
 */
float tempLoop() {       
  return printTempToSerial(SENSOR);
}

 /** @brief Metode printTempToSerial.
 *
 *  Retunerer high resolution temperatur.
 *
 *  @param  deviceId 
 *  @return float
 */
float printTempToSerial(int deviceId) {
    oneShotConversion(deviceId);    
    float highRes = getHighResolutionTemp(deviceId);
    return highRes;
}

 /** @brief Metode isConversionDone.
 *
 *  Registrerer adgangskonfiguration
 *
 *  @param  deviceId 
 *  @return bool
 */
bool isConversionDone(int deviceId) {
    Wire.beginTransmission(deviceId);
    Wire.write(ACCESS_CONFIG);
    Wire.endTransmission();
    Wire.requestFrom(deviceId, 1);

    byte config = Wire.read();
    return config;
}

 /** @brief Metode oneShotConversion.
 *
 *  Initialisere oneShot.
 *
 *  @param  deviceId 
 */
void oneShotConversion(int deviceId) {
    requestConversion(deviceId);
    waitForConversionToFinish(deviceId);
}

 /** @brief Metode oneShotConversion.
 *
 *  Vent på at konvertering er færdiggjort.
 *
 *  @param  deviceId 
 */
void waitForConversionToFinish(int deviceId) {
    //This method just blocks until the conversion
    //is complete
    while (!isConversionDone(deviceId)) {
    } 
}

 /** @brief Metode readTemperature.
 *
 *  Registrerer read konfiguration
 *
 *  @param  deviceId 
 */
 
void readTemperature(int deviceId) {
    Wire.beginTransmission(deviceId);
    Wire.write(READ_TEMPERATURE);
    Wire.endTransmission();
}
 /** @brief Metode getOneByteTemp.
 *
 *  Værdien returneret fra DS1621 er en signeret byte.
    castet som en char, fordi en char er en signeret byte.
 *
 *  @param  deviceId 
 *  @return char
 */

char getOneByteTemp(int deviceId) {
    readTemperature(deviceId);
    Wire.requestFrom(deviceId, 1);   
    return (char)Wire.read();
}

 /** @brief Metode getHighResolutionTemp.
 *
 *  Konvertere temperaturen til high resoulution.
 *
 *  @param  deviceId 
 *  @return char
 */
float getHighResolutionTemp(int deviceId) {
    char baseTemp = getOneByteTemp(deviceId);

    Wire.beginTransmission(deviceId);
    Wire.write(READ_COUNTER);
    Wire.endTransmission();
    
    Wire.requestFrom(deviceId, 1);
    float countRemain = Wire.read();

    Wire.beginTransmission(deviceId);
    Wire.write(READ_SLOPE);
    Wire.endTransmission();
    
    Wire.requestFrom(deviceId, 1);
    float countPerC = Wire.read();
 
    return (float(baseTemp) - 0.25) + ((countPerC - countRemain) / countPerC);
}

 /** @brief Metode writeConfig.
 *
 *  Setup konfiguration registrering.
 *
 *  @param  deviceId 
 *  @param  config 
 */
void writeConfig(int deviceId, byte config) {
    Wire.beginTransmission(deviceId);
    Wire.write(ACCESS_CONFIG);
    Wire.write(config);
    Wire.endTransmission();
}

 /** @brief Metode requestConversion.
 *
 *  start temperatur konvertering
 *
 *  @param  deviceId 
 */ 
void requestConversion(int deviceId) {
    Wire.beginTransmission(deviceId);
    Wire.write(START_CONVERT);
    Wire.endTransmission();
}
