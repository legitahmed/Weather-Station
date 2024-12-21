#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DHT.h"

// Define DHT sensor type and pin
#define DHTPIN 2
#define DHTTYPE DHT22

// Define OLED display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1     // Reset pin # (or -1 if sharing Arduino reset pin)
#define OLED_I2C_ADDR 0x3C  // I2C address for most OLED displays

// Environmental comfort thresholds
const float TEMP_LOW = 18.0;
const float TEMP_HIGH = 25.0;
const float HUMIDITY_LOW = 40.0;
const float HUMIDITY_HIGH = 60.0;

// Reading interval in milliseconds
#define READING_INTERVAL 5000

// Initialize display and DHT sensor
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
DHT dht(DHTPIN, DHTTYPE);

void setup() {
    Serial.begin(115200);

    // Initialize the OLED display
    if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDR)) {
        Serial.println("SSD1306 allocation failed");
        while (1);  // Halt program if initialization fails
    }

    // Display initial message
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Temp & Humidity Monitor");
    display.display();

    // Initialize the DHT sensor
    dht.begin();
    delay(2000);  // Allow time for sensors to stabilize
}

void loop() {
    // Read temperature and humidity from DHT sensor
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    // Handle sensor read errors
    if (isnan(temperature) || isnan(humidity)) {
        displayError("Sensor Read Error");
    } else {
        // Display and log readings
        displayReadings(temperature, humidity);
        logToSerial(temperature, humidity);

        // Check environmental conditions
        checkEnvironmentalConditions(temperature, humidity);
    }

    // Wait before taking the next reading
    delay(READING_INTERVAL);
}

// Display temperature and humidity on OLED
void displayReadings(float temp, float humidity) {
    display.clearDisplay();
    display.setCursor(0, 0);

    // Display temperature
    display.print("Temp: ");
    display.print(temp, 1);
    display.print(" C ");
    display.print(isTempComfortable(temp) ? "!!" : "");  // Added false condition

    // Display humidity
    display.setCursor(0, 16);
    display.print("Humidity: ");
    display.print(humidity, 1);
    display.print(" % ");
    display.print(isHumidityComfortable(humidity) ? "!!" : "");  // Added false condition

    display.display();
}

// Log temperature and humidity to Serial Monitor
void logToSerial(float temp, float humidity) {
    Serial.print("Temperature: ");
    Serial.print(temp);
    Serial.print(" °C | Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");
}

// Display an error message on OLED and Serial Monitor
void displayError(const String& message) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println(message);
    display.display();
    Serial.println(message);
}

// Check if temperature is within the comfort range
bool isTempComfortable(float temp) {
    return temp >= TEMP_LOW && temp <= TEMP_HIGH;
}

// Check if humidity is within the comfort range
bool isHumidityComfortable(float humidity) {
    return humidity >= HUMIDITY_LOW && humidity <= HUMIDITY_HIGH;
}

// Notify if conditions are outside the comfort zone
void checkEnvironmentalConditions(float temp, float humidity) {
    if (!isTempComfortable(temp) || !isHumidityComfortable(humidity)) {
        Serial.println("Environmental conditions outside comfort zone!");
    }
}
