#include <WiFi.h>
#include <Wire.h>
#include <MPU6050.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// ================= WIFI CONFIG =================
const char* ssid     = "wifi name";
const char* password = "password";

// ================= MQTT CONFIG =================
const char* mqttServer   = "172.20.10.2";
const int   mqttPort     = 1883;
const char* mqttUser     = "";       // leave empty if no auth
const char* mqttPassword = "";
const char* mqttTopic    = "esp32/sensors";

// ================= DS18B20 =================
#define ONE_WIRE_BUS 4
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature tempSensor(&oneWire);

// ================= MPU6050 =================
MPU6050 mpu;

// ================= VOLTAGE =================
#define VOLTAGE_PIN 34
#define VOLTAGE_DIVIDER_RATIO 5.128f   // adjust to your resistor divider

// ================= MQTT RECONNECT =================
#define MQTT_RETRY_LIMIT 5             // max attempts before giving up per loop

// ================= VARIABLES =================
WiFiClient   espClient;
PubSubClient client(espClient);

float        prevTemp         = 0;
unsigned long prevTime        = 0;
float        vibrationSmoothed = 0;
bool         firstLoop        = true;

// ================= WIFI =================
void setupWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (millis() - startTime > 20000) {
      Serial.println("\n[WARN] WiFi connection timed out. Running offline.");
      return;
    }
  }

  Serial.println("\nWiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

// ================= MQTT (non-blocking, limited retries) =================
bool reconnectMQTT() {
  int attempts = 0;
  while (!client.connected() && attempts < MQTT_RETRY_LIMIT) {
    Serial.print("Connecting to MQTT... attempt ");
    Serial.println(attempts + 1);

    if (client.connect("ESP32Client", mqttUser, mqttPassword)) {
      Serial.println("[OK] MQTT connected.");
      return true;
    } else {
      Serial.print("[FAIL] rc=");
      Serial.print(client.state());
      Serial.println(" — retrying in 500ms");
      delay(500);
      attempts++;
    }
  }

  if (!client.connected()) {
    Serial.println("[WARN] Could not connect to MQTT. Skipping publish this cycle.");
    return false;
  }
  return true;
}

// ================= SETUP =================
void setup() {
  Serial.begin(115200);

  setupWiFi();
  client.setServer(mqttServer, mqttPort);

  tempSensor.begin();
  Wire.begin();
  mpu.initialize();

  if (!mpu.testConnection()) {
    Serial.println("[WARN] MPU6050 connection failed!");
  }

  Serial.println("System Ready");
}

// ================= LOOP =================
void loop() {

  // -------- MQTT keep-alive --------
  if (WiFi.status() == WL_CONNECTED) {
    if (!client.connected()) {
      reconnectMQTT();
    }
    client.loop();
  }

  // ======== 1. TEMPERATURE ========
  tempSensor.requestTemperatures();
  float temperature = tempSensor.getTempCByIndex(0);
  bool  tempValid   = (temperature != DEVICE_DISCONNECTED_C && temperature != -127.0f);

  // ---- Temperature rate of change ----
  unsigned long currentTime = millis();
  float tempRate = 0;

  if (!firstLoop && prevTime != 0) {
    float dt = (currentTime - prevTime) / 1000.0f;
    if (dt > 0) {
      tempRate = (temperature - prevTemp) / dt;
    }
  }

  // Skip publishing on first loop — tempRate would be garbage
  if (firstLoop) {
    prevTemp  = temperature;
    prevTime  = currentTime;
    firstLoop = false;
    delay(2000);
    return;
  }

  prevTemp = temperature;
  prevTime = currentTime;

  // ======== 2. VIBRATION (RMS) ========
  const int samples = 50;
  float sumSq = 0;

  for (int i = 0; i < samples; i++) {
    int16_t ax, ay, az;
    mpu.getAcceleration(&ax, &ay, &az);

    float mag = sqrtf((float)ax * ax + (float)ay * ay + (float)az * az);
    float dyn = mag - 16384.0f;   // remove gravity (±2g mode)
    sumSq += dyn * dyn;
    delay(2);
  }

  float rms       = sqrtf(sumSq / samples);
  float vibration = rms / 16384.0f;   // convert to g

  // Exponential smoothing
  const float alpha    = 0.1f;
  vibrationSmoothed    = alpha * vibration + (1.0f - alpha) * vibrationSmoothed;

  // ======== 3. VOLTAGE ========
  int   rawValue   = analogRead(VOLTAGE_PIN);
  float pinVoltage = ((float)rawValue / 4095.0f) * 3.3f;
  float voltage    = pinVoltage * VOLTAGE_DIVIDER_RATIO;

  // ======== SERIAL DEBUG ========
  Serial.println("----- Sensor Data -----");

  if (!tempValid) {
    Serial.println("[ERROR] DS18B20 not detected or disconnected!");
  } else {
    Serial.printf("Temperature   : %.2f °C\n", temperature);
    Serial.printf("Temp Rate     : %.4f °C/s\n", tempRate);
  }

  Serial.printf("Vibration (g) : %.6f\n", vibrationSmoothed);
  Serial.printf("ADC Raw       : %d\n", rawValue);
  Serial.printf("Pin Voltage   : %.3f V\n", pinVoltage);
  Serial.printf("Real Voltage  : %.3f V\n", voltage);
  Serial.println("------------------------\n");

  // ======== MQTT PUBLISH ========
  // Only publish if temperature sensor is healthy
  if (!tempValid) {
    Serial.println("[SKIP] Not publishing — invalid temperature reading.");
    delay(2000);
    return;
  }

  if (WiFi.status() == WL_CONNECTED && client.connected()) {

    // Build JSON safely with ArduinoJson
    StaticJsonDocument<256> doc;
    doc["temperature"]   = temperature;
    doc["temp_rate"]     = tempRate;
    doc["vibration_g"]   = vibrationSmoothed;
    doc["voltage_raw"]   = rawValue;
    doc["voltage_pin"]   = pinVoltage;
    doc["voltage_real"]  = voltage;

    char payload[256];
    serializeJson(doc, payload);

    if (client.publish(mqttTopic, payload)) {
      Serial.println("[OK] Published to MQTT.");
    } else {
      Serial.println("[FAIL] MQTT publish failed.");
    }

  } else {
    Serial.println("[SKIP] WiFi or MQTT not connected — skipping publish.");
  }

  delay(2000);
}