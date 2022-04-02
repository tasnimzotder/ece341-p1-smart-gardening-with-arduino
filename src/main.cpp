/**
 * @file main.cpp
 * @author ES, TZ
 * @brief
 * @version 0.1
 * @date 2022-03-31
 *
 * @copyright TZ (c) 2022
 */

#include <Arduino.h>
#include <DHT.h>

#include "project-configs.h"

// define pins
const int dhtPin = 5;
const int irSensorPin = 6;
const int lightSensorPin = A0;
const int moistureSensorPin = A1;

const int pumpPin = 10;
const int ledPinRed = 11;
const int ledPinBlue = 12;
const int ledPinGreen = 13;

// global variables
const int dhtType = DHT11;

const int moistureMinThreshold = 70;
const int moistureMaxThreshold = 90;

// define instances
DHT dht(dhtPin, dhtType);

// define functions
void printProjectDetails();
void printReadings();

void setPinModes();
void setLEDs(int red, int green, int blue);
float getTemperature();
float getHumidity();
float getLightLevel();
float getMoistureLevel();
int getIRValue();

void controlPump(float moistureLevel);

int sendValuesToCloud(float temperature, float humidity, float lightLevel, float moistureLevel, int irValue, bool pumpState);

// states
bool pumpState = false;

void setup() {
    // initialize serial communication
    Serial.begin(115200);
    printProjectDetails();

    // initialize DHT sensor
    dht.begin();
}

void loop() {
    float moistureLevel = getMoistureLevel();
    float temperature = getTemperature();
    float humidity = getHumidity();
    float lightLevel = getLightLevel();
    int irValue = getIRValue();

    // print readings
    printReadings();

    // control the system
    controlPump(moistureLevel);

    // send values to cloud
    sendValuesToCloud(temperature, humidity, lightLevel, moistureLevel, irValue, pumpState);

    // wait for next iteration
    delay(666);
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
    String irValueStr = String("IR Value:\t" + String(getIRValue()) + "\n");
    String pumpStateStr = String("Pump:\t" + String(pumpState) + "\n");

    String printStr = String(moistureStr + temperatureStr + humidityStr + lightLevelStr + irValueStr + pumpStateStr);

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
    // Todo: implement moisture sensor reading
    return 0;
}

int getIRValue() {
    return digitalRead(irSensorPin);
}

// control functions
void setPinModes() {
    // inputs
    pinMode(dhtPin, INPUT);
    pinMode(irSensorPin, INPUT);
    pinMode(lightSensorPin, INPUT);
    pinMode(moistureSensorPin, INPUT);

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
        if (pumpState == true) {
            digitalWrite(pumpPin, 1);
            setLEDs(0, 1, 0);
        }
    } else if (moistureLevel >= moistureMaxThreshold) {
        digitalWrite(pumpPin, 0);
        setLEDs(0, 0, 1);
        pumpState = false;
    }
}

// cloud functions
int sendValuesToCloud(float temperature, float humidity, float lightLevel, float moistureLevel, int irValue, bool pumpState) {
    // Todo: implement cloud communication

    return 0;
}