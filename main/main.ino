#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// i have to add publish section
/*

Add publish Callback Gadhiiiii

*/

// =========================
// Wi-Fi
// =========================
const char *ssid = "Jesse";
const char *password = "jesse123";

// =========================
// MQTT
// =========================
const char *mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;

const char *commandTopic = "smart_switch/command";

// =========================
// GPIO
// D1 = GPIO5
// =========================
const int switchPin = 5;

WiFiClient espClient;
PubSubClient client(espClient);

// =========================
// Function: Control Switch
// =========================
void controlSwitch(String command)
{
  command.trim();

  if (command == "ON")
  {
    digitalWrite(switchPin, HIGH);

    Serial.println("SWITCH = ON");
    Serial.println("GPIO = HIGH");
  }

  else if (command == "OFF")
  {
    digitalWrite(switchPin, LOW);

    Serial.println("SWITCH = OFF");
    Serial.println("GPIO = LOW");
  }

  else
  {
    Serial.println("Unknown command");
  }
}

// =========================
// MQTT Callback
// =========================
void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message received on topic: ");
  Serial.println(topic);

  String message = "";

  for (unsigned int i = 0; i < length; i++)
  {
    message += (char)payload[i];
  }

  Serial.print("Message: ");
  Serial.println(message);

  // Process MQTT command
  controlSwitch(message);
}

// =========================
// MQTT Reconnect
// =========================
void reconnectMQTT()
{
  while (!client.connected())
  {
    Serial.println("Connecting to MQTT...");

    if (client.connect("ESP8266_SmartSwitch"))
    {
      Serial.println("MQTT Connected!");

      client.subscribe(commandTopic);

      Serial.println("Subscribed to command topic");
    }

    else
    {
      Serial.print("MQTT connection failed, state = ");
      Serial.println(client.state());

      delay(2000);
    }
  }
}

// =========================
// Setup
// =========================
void setup()
{
  Serial.begin(115200);

  Serial.println();
  Serial.println("SMART IoT SWITCH PROJECT");

  // GPIO setup
  pinMode(switchPin, OUTPUT);

  digitalWrite(switchPin, LOW);

  Serial.println("Switch initialized: OFF");

  // =========================
  // Wi-Fi
  // =========================
  Serial.println("Connecting to WiFi...");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println();

  Serial.println("WiFi connected!");

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // =========================
  // MQTT
  // =========================
  client.setServer(mqtt_server, mqtt_port);

  client.setCallback(callback);
}

// =========================
// Main Loop
// =========================
void loop()
{
  // MQTT connection
  if (!client.connected())
  {
    Serial.print("MQTT disconnected. State = ");
    Serial.println(client.state());

    reconnectMQTT();
  }

  // Process MQTT messages
  client.loop();

  // =========================
  // Serial Monitor Control
  // =========================
  if (Serial.available())
  {
    String command = Serial.readStringUntil('\n');

    Serial.print("Serial command: ");
    Serial.println(command);

    controlSwitch(command);
  }
}