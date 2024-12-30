#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"
#include "Adafruit_SSD1306.h"
#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"

// WiFi and MQTT Configuration
const char* ssid = "YourSSID";
const char* password = "YourPassword";
const char* mqttServer = "broker.hivemq.com";
const int mqttPort = 1883;
const char* mqttTopic = "iot/ai-predictions";

// DHT Configuration
#define DHTPIN 4  // GPIO for DHT sensor
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// Sensor Pin Definitions
#define MQ2_PIN 34    // Gas sensor (analog)
#define LDR_PIN 35    // Light sensor (analog)
#define TRIG_PIN 13   // Ultrasonic sensor trigger
#define ECHO_PIN 12   // Ultrasonic sensor echo

// OLED Display Configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// TensorFlow Lite Model
extern const unsigned char tflite_model[];
extern const int tflite_model_len;

constexpr int kTensorArenaSize = 2 * 1024;
uint8_t tensor_arena[kTensorArenaSize];

WiFiClient espClient;
PubSubClient client(espClient);

void connectToWiFi() {
  Serial.print("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connected to Wi-Fi.");
}

void connectToMQTT() {
  Serial.print("Connecting to MQTT...");
  while (!client.connected()) {
    if (client.connect("ESP32Client")) {
      Serial.println("Connected to MQTT Broker.");
    } else {
      Serial.print("Failed. Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

float getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH);
  float distance = duration * 0.034 / 2; // cm
  return distance;
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(MQ2_PIN, INPUT);
  pinMode(LDR_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  connectToWiFi();
  client.setServer(mqttServer, mqttPort);
  connectToMQTT();

  if (!display.begin(SSD1306_I2C_ADDRESS, SCREEN_WIDTH, SCREEN_HEIGHT)) {
    Serial.println("OLED initialization failed!");
    for (;;);
  }

  display.display();
}

void loop() {
  if (!client.connected()) {
    connectToMQTT();
  }
  client.loop();

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  int gas = analogRead(MQ2_PIN);
  int light = analogRead(LDR_PIN);
  float distance = getDistance();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // TensorFlow Lite Inference (use temperature and humidity)
  float input_data[2] = {temperature, humidity};
  TfLiteTensor* input_tensor = interpreter.input(0);
  input_tensor->data.f[0] = input_data[0];
  input_tensor->data.f[1] = input_data[1];
  interpreter.Invoke();
  float prediction = interpreter.output(0)->data.f[0];

  // Publish data and predictions
  String payload = "{\"temperature\":" + String(temperature) +
                   ",\"humidity\":" + String(humidity) +
                   ",\"gas\":" + String(gas) +
                   ",\"light\":" + String(light) +
                   ",\"distance\":" + String(distance) +
                   ",\"prediction\":" + String(prediction) + "}";
  client.publish(mqttTopic, payload.c_str());
  Serial.println(payload);

  // Display on OLED
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Temp: "); display.print(temperature); display.println(" C");
  display.print("Humidity: "); display.print(humidity); display.println(" %");
  display.print("Gas: "); display.print(gas);
  display.print("Light: "); display.print(light);
  display.print("Distance: "); display.print(distance); display.println(" cm");
  display.display();

  delay(5000); // Delay for 5 seconds
}
