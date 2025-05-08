#include <FirebaseESP8266.h>
#include <ESP8266WiFi.h>
#include <DHT.h>

#define DHT_PIN D2
#define DHT_TYPE DHT11
#define LPG_SENSOR_PIN A0 // Analog pin connected to FC-22 sensor output
#define FIREBASE_HOST "air-monitoring-1262f-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "AIzaSyDC6KtCTp2jlmUD1SZcxgh9A6eGLrIKKow"
#define WIFI_SSID "EROVOUTIKA2G"
#define WIFI_PASSWORD "Erovoutika123!"

FirebaseData firebaseData;

DHT dht(DHT_PIN, DHT_TYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Connected to Wi-Fi");

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

void loop() {
  delay(2000);
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  int lpgValue = analogRead(LPG_SENSOR_PIN);

  if (!isnan(humidity) && !isnan(temperature)) {
    if (Firebase.setFloat(firebaseData, "humidity", humidity) &&
        Firebase.setFloat(firebaseData, "temperature", temperature) &&
        Firebase.setInt(firebaseData, "lpg_sensor_value", lpgValue) &&
        Firebase.pushFloat(firebaseData, "history/humidity_history", humidity) &&
        Firebase.pushFloat(firebaseData, "history/temperature_history", temperature) &&
        Firebase.pushInt(firebaseData, "history/lpg_sensor_history", lpgValue)) {
      Serial.print("Humidity: ");
      Serial.print(humidity);
      Serial.print(" %\t");
      Serial.print("Temperature: ");
      Serial.print(temperature);
      Serial.println(" °C");
      Serial.print("LPG Sensor Value: ");
      Serial.println(lpgValue);
    } else {
      Serial.println("Firebase operation failed!");
    }
  } else {
    Serial.println("Failed to read from DHT sensor!");
  }
}
