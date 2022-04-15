/**
 * @file main.cpp
 * @author ES, TZ, SK
 * @brief
 * @version 0.1
 * @date 2022-03-31
 *
 * @copyright TZ (c) 2022
 */

#include <Arduino.h>
#include <DHT.h>
#include <UnoESPWiFi.h>

#include "project-configs.h"
// #include "uno-esp-wifi.h"

// configs
String serverIP = "api.thingspeak.com";
int serverPort = 80;
String apiKey = SERVER_API_KEY;

String wifi_ssid = WIFI_SSID;
String wifi_password = WIFI_PASSWORD;

// define pins
const int rxPin = 2;
const int txPin = 3;

const int dhtPin = 11;
// const int irSensorPin = 6;
const int lightSensorPin = A4;
const int moistureSensorPin = A5;
const int moistureSensorPowerPin = 12;

const int pumpPin = 5;
const int ledPinRed = 8;
const int ledPinBlue = 9;
const int ledPinGreen = 10;

// global variables
const int dhtType = DHT11;

const int moistureMinThreshold = 70;
const int moistureMaxThreshold = 90;

// define instances
DHT dht(dhtPin, dhtType);
UnoESPWiFi esp(rxPin, txPin);

// define functions
void printProjectDetails();
void printReadings();

void setPinModes();
void setLEDs(int red, int green, int blue);
float getTemperature();
float getHumidity();
float getLightLevel();
float getMoistureLevel();

void controlPump(float moistureLevel);

String sendValuesToCloud(float temperature, float humidity, float lightLevel, float moistureLevel, bool pumpState);

// states
bool pumpState = false;

void setup() {
    // initialize serial communication
    Serial.begin(9600);
    setPinModes();
    printProjectDetails();

    // initialize esp shield serial communication
    esp.serialBegin(115200);
    esp.serverConfig(serverIP, serverPort);

    // initialize DHT sensor
    dht.begin();

    // connect to wifi
    String wifi_response = esp.connect(wifi_ssid, wifi_password);
    Serial.println(wifi_response);

    digitalWrite(moistureSensorPowerPin, 0);
}

void loop() {
    float moistureLevel = getMoistureLevel();
    float temperature = getTemperature();
    float humidity = getHumidity();
    float lightLevel = getLightLevel();

    // print readings
    printReadings();

    // control the system
    controlPump(moistureLevel);

    // send values to cloud
    String response = sendValuesToCloud(temperature, humidity, lightLevel, moistureLevel, pumpState);
    Serial.println(response);

    // wait for next iteration
    delay(10);
}

void printProjectDetails() {
    Serial.print("Project:\t");
    Serial.println(PROJECT_NAME);

    Serial.print("\nBy:\n");
    Serial.println(String("-\t" + String(ESHIKA_NAME) + "\t" + String(ESHIKA_REGISTRATION_NO)));
    Serial.println(String("-\t" + String(TASNIM_NAME) + "\t" + String(TASNIM_REGISTRATION_NO)));
    Serial.println(String("-\t" + String(SOHAN_NAME) + "\t" + String(SOHAN_REGISTRATION_NO)));
}

void printReadings() {
    String moistureStr = String("Moisture:\t" + String(getMoistureLevel()) + "\n");
    String temperatureStr = String("Temperature:\t" + String(getTemperature()) + "\n");
    String humidityStr = String("Humidity:\t" + String(getHumidity()) + "\n");
    String lightLevelStr = String("Light Level:\t" + String(getLightLevel()) + "\n");
    // String irValueStr = String("IR Value:\t" + String(getIRValue()) + "\n");
    String pumpStateStr = String("Pump:\t" + String(pumpState) + "\n");

    String printStr = String(moistureStr + temperatureStr + humidityStr + lightLevelStr + pumpStateStr);

    Serial.println("\n");
    Serial.println(printStr);
}

// get value functions
float getTemperature() {
    return dht.readTemperature();
}

float getHumidity() {
    return dht.readHumidity();
}

float getLightLevel() {
    // Todo: implement light sensor reading
    return 0;
}

float getMoistureLevel() {
    digitalWrite(moistureSensorPowerPin, HIGH);
    delay(100);

    float moisture = 0;

    for (int i = 0; i < 20; i++) {
        moisture += analogRead(moistureSensorPin);
        delay(25);
    }

    digitalWrite(moistureSensorPowerPin, LOW);

    moisture = (1023 * 20 - moisture) / 20;
    moisture = map(moisture, 0, 1023, 0, 100);

    return moisture;
}

// control functions
void setPinModes() {
    // inputs
    pinMode(dhtPin, INPUT);
    // pinMode(irSensorPin, INPUT);
    pinMode(lightSensorPin, INPUT);
    // pinMode(moistureSensorPin, INPUT_PULLUP);
    pinMode(moistureSensorPowerPin, OUTPUT);

    // outputs
    pinMode(pumpPin, OUTPUT);
    pinMode(ledPinRed, OUTPUT);
    pinMode(ledPinGreen, OUTPUT);
    pinMode(ledPinBlue, OUTPUT);
}

void setLEDs(int red, int green, int blue) {
    digitalWrite(ledPinRed, red);
    digitalWrite(ledPinGreen, green);
    digitalWrite(ledPinBlue, blue);
}

void controlPump(float moistureLevel) {
    if (moistureLevel < moistureMinThreshold) {
        digitalWrite(pumpPin, 1);
        setLEDs(1, 0, 0);
        pumpState = true;
    } else if ((moistureLevel >= moistureMinThreshold) && (moistureLevel < moistureMaxThreshold)) {
        // if (pumpState == true) {
        if (true) {
            digitalWrite(pumpPin, 1);
            // setLEDs(0, 1, 0);
        }
        setLEDs(0, 1, 1);
        pumpState = true;
    } else if (moistureLevel >= moistureMaxThreshold) {
        digitalWrite(pumpPin, 0);
        setLEDs(0, 0, 1);
        pumpState = false;
    }
}

// cloud functions
String sendValuesToCloud(float temperature, float humidity, float lightLevel, float moistureLevel, bool pumpState) {
    String cmdString = "GET /update?api_key=";
    cmdString += apiKey;
    cmdString += "&field1=";
    cmdString += String(temperature);
    cmdString += "&field2=";
    cmdString += String(humidity);
    cmdString += "&field3=";
    cmdString += String(lightLevel);
    cmdString += "&field4=";
    cmdString += String(moistureLevel);
    cmdString += "&field5=";
    cmdString += String(pumpState);

    cmdString += "\r\n\r\n";

    String server_resp = esp.writeToServer(cmdString);

    return server_resp;
}