#include <Arduino.h>
#include <DFRobot_ENS160.h>
#include <AHTxx.h>

const uint8_t ENS160_I2C_ADDRESS = 0x53;
const uint8_t AHT20_I2C_ADDRESS = 0x38;

const float DEFAULT_AMBIENT_TEMPERATURE = 20.0;
const float DEFAULT_AMBIENT_HUMIDITY = 50.0;

volatile uint16_t TVOC = 0;
volatile uint16_t CO2 = 0;
volatile uint8_t air_quality = 0;
volatile float humidity = 0;
volatile float temperature = 0;

AHTxx AHT20(AHT20_I2C_ADDRESS, AHT2x_SENSOR);
DFRobot_ENS160_I2C ENS160(&Wire, ENS160_I2C_ADDRESS);

TaskHandle_t Task1;
TaskHandle_t Task2;

void setupENS160() {
    while (ENS160.begin() != NO_ERR) {
        Serial.println("ENS160 not started");
        delay(1000);
    }
    ENS160.setPWRMode(ENS160_STANDARD_MODE);
    ENS160.setTempAndHum(DEFAULT_AMBIENT_TEMPERATURE, DEFAULT_AMBIENT_HUMIDITY);
}

void setupATH20() {
    while (!AHT20.begin()) {
        Serial.println("AHT20 not started");
        delay(1000);
    }
}

void checkATH20() {
    temperature = AHT20.readTemperature();
    humidity = AHT20.readHumidity();
    if (temperature == AHTXX_ERROR || humidity == AHTXX_ERROR) {
        if (temperature == AHTXX_ERROR && humidity == AHTXX_ERROR) {
            Serial.println("AHT20 temp error");
            Serial.println("AHT20 hum error");
        } else if (humidity == AHTXX_ERROR) {
            Serial.println("AHT20 hum error");
        } else if (temperature == AHTXX_ERROR) {
            Serial.println("AHT20 temp error");
        }
        AHT20.softReset();
        delay(1000);
        Serial.println("reset AHT20");
        delay(1000);
    } else {
        ENS160.setTempAndHum(temperature, humidity);
    }
}

void checkENS160() {
    TVOC = ENS160.getTVOC();
    CO2 = ENS160.getECO2();
    air_quality = ENS160.getAQI();
}

void task1(void *pvParameters) {
    while (true) {
        checkENS160();
        checkATH20();
    }
}

void task2(void *pvParameters) {
    while (true) {
        Serial.print("TVOC");
        Serial.println(TVOC);
        Serial.print("CO2");
        Serial.println(CO2);
        Serial.print("air_quality");
        Serial.println(air_quality);
        Serial.print("temperature");
        Serial.println(temperature);
        Serial.print("humidity");
        Serial.println(humidity);
        delay(1000);
    }
}

void setupTasks() {
    xTaskCreatePinnedToCore(
            task1,
            "task1",
            50000,
            NULL,
            1,
            &Task1,
            0);
    xTaskCreatePinnedToCore(
            task2,
            "task2",
            50000,
            NULL,
            1,
            &Task2,
            1);
}

void setup() {
    setupENS160();
    setupATH20();
//    setupTasks();
}

void loop() {

}