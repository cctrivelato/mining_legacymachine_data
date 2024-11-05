// Code to publish the data coming from the ReadOuts Newall DP900.
// Caique Trivelato 
// 11/5/2024

#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>

const char* ssid = "wifi";
const char* password = "*******";

const char* server = "localhost/TestPublishing/publish_readout.php";

void setup() {
  Serial.begin(9600);
  Serial.swap();

  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  if (Serial.available() > 0) {
    String dp900Data = Serial.readStringUntil("\n");
    Serial.println("DP900 Data: " + dp900Data);
    
    publishToMySQL(dp900Data); 
  }
}

void publishToMySQL(String data_value) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverURL);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String postData = "data_value=" + data_value;
    int httpResponseCode = http.POST(postData);

    if (httpResponseCode > 0) {
      Serial.println("Data sent to MySQL successfully");
    } else {
      Serial.print("Error sending data: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  }
}