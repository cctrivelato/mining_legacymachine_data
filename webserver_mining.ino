// Code for the shock sensor.
// Caique Trivelato 
// 7/24/2024

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <BfButton.h>
#include <DHT.h>
#include <TimeLib.h>

#define DHTPIN D7     // Pin where the DHT sensor is connected
#define DHTTYPE DHT11 // Change to DHT11 if you are using a DHT11 sensor

const char* ssid = "WIFI";        // Your WiFi SSID
const char* password = "*******";  // Your WiFi Password

// IP of your local server (where PHP script is hosted)
const char* serverIP = "local";  // Replace with your server IP
const int serverPort = 80;  // HTTP port

unsigned long lastTriggerTime = 0;

int machine_id = 1;
int node_id = 1;

DHT dht(DHTPIN, DHTTYPE);

int btnPin = D3;
int dT = D4;
int cLK = D5;
BfButton btn(BfButton::STANDALONE_DIGITAL, btnPin, true, LOW);

int counting = 0;
int laps = 0;
int angle = 0;
int aState;
int aLastState;
bool lapChange;

int shock_pin = D1;
bool lastMove;
bool moving = false;
int counter = 0;

WiFiClient wifiClient;
String button_status = "No push";


void setup() {
  Serial.begin(115200);

  pinMode(shock_pin, INPUT);
  pinMode(cLK, INPUT_PULLUP);
  pinMode(dT, INPUT_PULLUP);
  aLastState = digitalRead(cLK);

  dht.begin();

  btn.onPress(pressHandler)
    .onDoublePress(pressHandler)
    .onPressFor(pressHandler, 1000);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi.");
}

// Button press handling function
void pressHandler(BfButton* btn, BfButton::press_pattern_t pattern) {
  switch (pattern) {
    case BfButton::SINGLE_PRESS:
      Serial.println("Single push");
      button_status = "Single";
      break;
    case BfButton::DOUBLE_PRESS:
      Serial.println("Double push");
      button_status = "Double";
      break;
    case BfButton::LONG_PRESS:
      Serial.println("Long push");
      button_status = "Long";
      break;
  }
}

void checkVibrationSensor() {
  int pin_state = digitalRead(shock_pin);
  if (pin_state == LOW && lastMove == HIGH) {
    counter++;
    Serial.println("The vibration is high.");
  }
  lastMove = pin_state;
}

void checkRotaryEncoder() {
  aState = digitalRead(cLK);
  if (aState != aLastState) {
    if (digitalRead(dT) != aState) {
      counting++;
      angle = 12 * counting;
    } else {
      counting--;
      angle = 12 * counting;
    }
    if (counting == 30 || counting == -30) {
      lapChange = true;
      counting = 0;
      laps++;
    }

    Serial.print("Angle: ");
    Serial.println(angle);

    if (lapChange) {
      lapChange = false;
      Serial.print("Laps: ");
      Serial.println(laps);
    }
  }
  aLastState = aState;
}

void checkDHTSensor(int &humidity, int &temperature) {
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
  
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
  }
}

void publish_php(int humidity, int temperature) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "http://" + String(serverIP) + "/TestPublishing/webserver_publishing.php";
    http.begin(wifiClient, url);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String postData = "vibration=" + String(counter) + "&laps=" + String(laps) + "&button=" + String(button_status)
                      + "&machine_id=" + String(machine_id) + "&node_id=" + String(node_id)
                      + "&temperature=" + String(temperature) + "&humidity=" + String(humidity);

    int httpResponseCode = http.POST(postData);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.print("Response: ");
      Serial.println(response);
    } else {
      Serial.println("Error on sending POST: " + String(httpResponseCode));
    }

    http.end();
    lastTriggerTime = now();  // Update the lastTriggerTime only after a successful publish
    counter = 0;
    button_status = "No push";  // Reset button status after sending data
  }
}

void loop() {
  checkVibrationSensor();
  checkRotaryEncoder();
  
  // Read button states
  btn.read();

  // Read temperature and humidity
  int humidity, temperature;
  checkDHTSensor(humidity, temperature);

  // Publish data every 60 seconds
  if (now() - lastTriggerTime >= 10) {
    publish_php(humidity, temperature);
  }

  delay(20);
}