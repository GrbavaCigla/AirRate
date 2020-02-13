#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>
#include <WiFi.h>
#include <Wire.h>

#include "SSD1306Wire.h"

#define BAR_HEIGHT 12
#define SEALEVELPRESSURE_HPA (1013.25)
#define BAUD_RATE 115200
#define CONTRAST 255
#define BME_LEN 4
#define AIR_LEN 4
#define SPLITTER_WIDTH 3

const char* ssid = "";      // WiFi SSID here
const char* password = "";  // Password here

// WiFiServer server(80);  // Server on port 80

SSD1306Wire display(0x3c, 5, 4);  // Display on pin 5 and 4
Adafruit_BME280 bme;

String air_labels[AIR_LEN] = {"1:", "2.5:", "10:"};
int air_values[AIR_LEN] = {0, 0, 0};

String bme_labels[BME_LEN] = {"Tempera.:", "Pritisak:", "Vlaznost:", "Visina:"};
String bme_unit[BME_LEN] = {"C", "hPa", "%", "m"};
int bme_values[BME_LEN] = {0, 0, 0, 0};

void setup() {
    // Setting up serial at BAUD_RATE
    Serial.begin(BAUD_RATE);

    // Setting up display
    display.init();
    display.setContrast(CONTRAST);

    // Setting up BME280 sensor
    bme.begin(0x76);

    // Setting up WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    setup_draw();

    display.display();
}

// Writing lines and ip address to display
void setup_draw() {
    display.drawString(0, 0, WiFi.localIP().toString());
    display.drawLine(0, BAR_HEIGHT, display.getWidth() - 1, BAR_HEIGHT);
    display.drawLine(display.getWidth() / SPLITTER_WIDTH, BAR_HEIGHT, display.getWidth() / 3, display.getHeight() - 1);
}

// Draw BME sensor values to the display
void draw_bme() {
    for (int i = 0; i < BME_LEN; i++) {
        display.drawString(display.getWidth() / SPLITTER_WIDTH + 2, BAR_HEIGHT + i * 12 + 2, bme_labels[i] + String(bme_values[i]) + bme_unit[i]);
    }
}

// Draw PMS7003 sensor values to the display
void draw_air() {
    display.drawString(0, 14, "Vazduh");
    display.drawLine(0, 26, display.getWidth() / SPLITTER_WIDTH, 26);
    for (int i = 1; i < AIR_LEN + 1; i++) {
        display.drawString(0, BAR_HEIGHT + i * 12 + 2, air_labels[i - 1] + String(air_values[i - 1]));
    }
}

//Sets public variables temperature, humidity, pressure, altitude to sensors value.
void fetch_sensors() {
    bme_values[1] = bme.readPressure() / 100.0F;
    bme_values[0] = bme.readTemperature();
    bme_values[3] = bme.readAltitude(SEALEVELPRESSURE_HPA);
    bme_values[2] = bme.readHumidity();
}

void loop() {
    display.clear();
    fetch_sensors();
    setup_draw();
    draw_bme();
    draw_air();
    display.display();
}
