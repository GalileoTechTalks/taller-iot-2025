/*********
Taller IoT - Ing. Electrónica - Tesla Lab - 2025
Código realizado para prueba de MQTT - Agosto 2025

Código basado en:  
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

#include <WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>            // Para DS18B20
#include <DallasTemperature.h>  // Para DS18B20


// Replace the next variables with your SSID/Password combination
const char* ssid = "galileo";
const char* password = "";

// Add your MQTT Broker IP address, example:
//const char* mqtt_server = "";
const char* mqtt_server = "test.mosquitto.org";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

// Sensor DS18B20 functions
const int pinDatosDQ = 32;
OneWire oneWireObjeto(pinDatosDQ);
DallasTemperature sensorDS18B20(&oneWireObjeto);

// Dato a obtener
float temperature = 0;

void setup() {
  Serial.begin(115200);
  // default settings
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  sensorDS18B20.begin();
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("esp32/output");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 15000) {
    lastMsg = now;

    // Temperature in Celsius
    Serial.print("Requesting temperatures...");
    sensorDS18B20.requestTemperatures();
    Serial.println("DONE");
    delay(1500);
    
    temperature = sensorDS18B20.getTempCByIndex(0);
    // Check if reading was successful
      if (temperature != DEVICE_DISCONNECTED_C)
      {
        Serial.print("Temperature for the device 1 (index 0) is: ");
        Serial.println(temperature);
      }
      else
      {
        Serial.println("Error: Could not read temperature data");
      }
    
    
    // Convert temp to string and publish
    String str13(temperature);
    str13.toCharArray(msg, 50);
    client.publish("tallerIoT/e41/temp", msg);

    //Delay between next data
    delay(1000);
  }
}

