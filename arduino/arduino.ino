#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050.h"
#include <WiFiS3.h>
#include <MQTTClient.h>
#include <math.h>

MPU6050 mpu;
int16_t ax, ay, az;
int16_t gx, gy, gz;

double start_degrees = -20;
double end_degrees = 20;
double degrees = 0;
int count = 0;
bool goingUp = false;

// 0 = dumbbell | 1 = Barbell
int mode = 1;

// mqtt setup from chatgpt
// ----------------- WiFi + MQTT -----------------
const char WIFI_SSID[] = "TestLab";     
const char WIFI_PASSWORD[] = "11223344";  

const char MQTT_BROKER_ADDRESS[] = "mqtt-dashboard.com";  
const int MQTT_PORT = 1883;
const char MQTT_CLIENT_ID[] = "arduino-uno-r4-client";
const char MQTT_USERNAME[] = "";
const char MQTT_PASSWORD[] = "";

const char PUBLISH_TOPIC[] = "testtopic/Peemza";
const char SUBSCRIBE_TOPIC[] = "testtopic/Peemza";

WiFiClient wifiClient;
MQTTClient client(256);

unsigned long lastMsg = 0;

// ----------------- ฟังก์ชันเชื่อมต่อ Wi-Fi -----------------
void connectWiFi() {
  Serial.print("🔌 Connecting to WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n✅ WiFi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

// ----------------- ฟังก์ชันเชื่อมต่อ MQTT -----------------
void connectMQTT() {
  Serial.print("🔗 Connecting to MQTT broker... ");
  while (!client.connect(MQTT_CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD)) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\n✅ Connected to MQTT broker!");

  client.subscribe(SUBSCRIBE_TOPIC);
}
// from website
void messageReceived(String &topic, String &payload) {
  Serial.println("Arduino UNO R4 - received from MQTT:");
  Serial.println("- topic: " + topic);
  Serial.println("- payload:");
  Serial.println(payload);

  if (payload == "mode=0") {
    mode = 0;
    Serial.println("switch to dumbbell");
  } else if (payload == "mode=1") {
    mode = 1;
    Serial.println("switch to barbell");
  }
}

// ----------------- setup -----------------
void setup() {
  Serial.begin(9600);
  Wire.begin();
  mpu.initialize();

  if (!mpu.testConnection()) {
    Serial.println("❌ MPU6050 connection failed!");
    while (1);
  }

  Serial.println("✅ MPU6050 Ready...");

  connectWiFi();

  client.begin(MQTT_BROKER_ADDRESS, MQTT_PORT, wifiClient);
  client.onMessage(messageReceived);

  connectMQTT();
  delay(1000);
}

// ----------------- loop -----------------
void loop() {
  client.loop(); 
  if (!client.connected()) {
    connectMQTT();
  }

  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  double pitch = atan2(-ax, sqrt((ay * ay) + (az * az))) * 180.0 / PI;
  double roll  = atan2(ay, sqrt((ax * ax) + (az * az))) * 180.0 / PI;
  // https://samselectronicsprojects.blogspot.com/2014/07/getting-roll-pitch-and-yaw-from-mpu-6050.html

  // Dumbbell Mode
  if (mode == 0) {
    degrees = pitch;
    if (degrees > end_degrees && !goingUp) {
      goingUp = true;
    }
    if (degrees < start_degrees && goingUp) {
      count++;
      goingUp = false;
      sendMQTT();
    }
  }

  // Barbell Mode
  else if (mode == 1) {
    if (az > 10000 && !goingUp) {
      goingUp = true;
    }
    if (az < 7500 && goingUp) {
      count++;
      goingUp = false;
      sendMQTT();
    }
  }

  Serial.print("Mode: ");
  if (mode == 0) {
    Serial.print("Dumbbell");
  } else {
    Serial.print("Barbell");
  }
  Serial.print(" , Pitch: ");
  Serial.print(pitch, 2);
  Serial.print(" , Roll: ");
  Serial.print(roll, 2);
  Serial.print(" , az: ");
  Serial.print(az);
  Serial.print(" , Count: ");
  Serial.println(count);
  delay(200);
}

void sendMQTT() {
  String payload = "{\"mode\":" + String(mode) +
                   ",\"count\":" + String(count) +
                   ",\"az\":" + String(az) + "}";
  client.publish(PUBLISH_TOPIC, payload);
  Serial.print("Sent to MQTT: ");
  Serial.println(payload);
}