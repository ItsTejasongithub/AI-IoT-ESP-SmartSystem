# AI-Driven IoT System Using ESP

## Overview
This repository demonstrates an advanced IoT project integrating AI/ML capabilities with ESP32. The system collects sensor data, processes it using AI/ML models, and performs automated actions based on predictions.

## Features
- Real-time data collection using ESP32 and multiple sensors.
- MQTT-based communication for data transfer.
- Integration with TensorFlow Lite for edge AI.
- OLED display for local monitoring.

## Repository Structure
```
AI-IoT-ESP-SmartSystem/
├── Code/
│   └── main.ino           # Arduino code
├── Schematics/
│   └── PinDiagram.PNG     # Wiring diagram
├── README.md              # Project documentation
└── LICENSE                # Project license
```

## Pin Mapping
- **DHT22 Sensor**:
  - VCC: 3.3V
  - GND: GND
  - Data: GPIO4
- **Relay Module**:
  - IN: GPIO5
  - VCC: 3.3V
  - GND: GND
- **OLED Display (I2C)**:
  - SDA: GPIO21
  - SCL: GPIO22
- **MQ-2 Gas Sensor**:
  - Analog: GPIO34
- **LDR**:
  - Analog: GPIO35
- **Ultrasonic Sensor (HC-SR04)**:
  - Trig: GPIO13
  - Echo: GPIO12

## Installation
1. Clone the repository:
   ```bash
   git clone https://github.com/TejasEmbeddedEngg/AI-IoT-ESP-SmartSystem.git
   ```
2. Open `Code/main.ino` in the Arduino IDE.
3. Upload the code to your ESP32.

## Usage
1. Connect the ESP32 to your Wi-Fi.
2. Monitor sensor data on the OLED display or via the MQTT broker.
3. Observe AI predictions and automated actions.

## License
This project is licensed under the MIT License.

