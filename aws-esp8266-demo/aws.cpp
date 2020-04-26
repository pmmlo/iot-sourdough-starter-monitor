#include "aws.h"

#include <time.h>
#include <ArduinoJson.h>
#include "FS.h"

//Follow instructions from https://github.com/debsahu/ESP-MQTT-AWS-IoT-Core/blob/master/doc/README.md
//Enter values in secrets.h ▼
#include "secrets.h"

const int MQTT_PORT = 8883;
const char MQTT_SUB_TOPIC[] = "$aws/things/" THINGNAME "/shadow/update";
// const char MQTT_PUB_TOPIC[] = "$aws/things/" THINGNAME "/shadow/update";
const char MQTT_PUB_TOPIC[] = "esp8266/data";

AwsIot::AwsIot()
{
    net = new WiFiClientSecure();
    client = new PubSubClient(*net);
}

void AwsIot::pubSubErr(int8_t MQTTErr)
{
  if (MQTTErr == MQTT_CONNECTION_TIMEOUT)
    Serial.print("Connection tiemout");
  else if (MQTTErr == MQTT_CONNECTION_LOST)
    Serial.print("Connection lost");
  else if (MQTTErr == MQTT_CONNECT_FAILED)
    Serial.print("Connect failed");
  else if (MQTTErr == MQTT_DISCONNECTED)
    Serial.print("Disconnected");
  else if (MQTTErr == MQTT_CONNECTED)
    Serial.print("Connected");
  else if (MQTTErr == MQTT_CONNECT_BAD_PROTOCOL)
    Serial.print("Connect bad protocol");
  else if (MQTTErr == MQTT_CONNECT_BAD_CLIENT_ID)
    Serial.print("Connect bad Client-ID");
  else if (MQTTErr == MQTT_CONNECT_UNAVAILABLE)
    Serial.print("Connect unavailable");
  else if (MQTTErr == MQTT_CONNECT_BAD_CREDENTIALS)
    Serial.print("Connect bad credentials");
  else if (MQTTErr == MQTT_CONNECT_UNAUTHORIZED)
    Serial.print("Connect unauthorized");
}

void AwsIot::connect()
{
  Serial.print("MQTT connecting ");
  while (!client->connected())
  {
    if (client->connect(THINGNAME))
    {
      Serial.println("connected!");
      if (!client->subscribe(MQTT_SUB_TOPIC))
        pubSubErr(client->state());
    }
    else
    {
      Serial.print("failed, reason -> ");
      pubSubErr(client->state());
        Serial.println(" < try again in 5 seconds");
        delay(5000);
    }
  }
}


void AwsIot::loadCertificatesFromSPIFFS(void) {
  if (!SPIFFS.begin())
  {
    Serial.println("ERROR: Failed to mount file system");
    return;
  }

  // Load certificate file
  File cert = SPIFFS.open("/cert.der", "r"); //replace cert.crt eith your uploaded file name
  if (!cert)
  {
    Serial.print("ERROR: Failed to open cert file");
  }
  else
    Serial.print("Success to open cert file... ");

  delay(1000);

  if (net->loadCertificate(cert))
    Serial.println("cert loaded!");
  else
    Serial.println("ERROR: cert not loaded");

  // Load private key file
  File private_key = SPIFFS.open("/private.der", "r"); //replace private eith your uploaded file name
  if (!private_key)
  {
    Serial.print("ERROR: Failed to open private cert file");
  }
  else
    Serial.print("Success to open private cert file... ");

  delay(1000);

  if (net->loadPrivateKey(private_key))
    Serial.println("private key loaded!");
  else
    Serial.println("ERROR: private key not loaded");

  // Load CA file
  File ca = SPIFFS.open("/ca.der", "r"); //replace ca eith your uploaded file name
  if (!ca)
  {
    Serial.print("ERROR: Failed to open ca ");
  }
  else
    Serial.print("Success to open ca... ");

  delay(1000);

  if (net->loadCACert(ca))
    Serial.println("ca loaded!");
  else
    Serial.println("ERROR: ca failed");
}

void AwsIot::publishMessage()
{
  // Get current timestamp (ie. "Tue Apr 21 21:30:31 2020\n")
  struct tm timeinfo;
  time_t now = time(nullptr);
  gmtime_r(&now, &timeinfo);

  StaticJsonDocument<200> doc;
  doc["time"] = asctime(&timeinfo);
  doc["temperature"] = random(100);
  doc["humidity"] = random(100);
  doc["distance"] = random(100);

  char msg[measureJson(doc) + 1];
  serializeJson(doc, msg, sizeof(msg));

  Serial.printf("Sending  [%s]: ", MQTT_PUB_TOPIC);
  Serial.println(msg);

  if (!client->publish(MQTT_PUB_TOPIC, msg))
    pubSubErr(client->state());
}

void AwsIot::setHost(const char* name)
{
    client->setServer(name, MQTT_PORT);
}

// void setCallback()

void AwsIot::loop()
{
    client->loop();
}

bool AwsIot::connected()
{
    return client->connected();
}