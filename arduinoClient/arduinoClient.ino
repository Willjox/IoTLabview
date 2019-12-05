//D #include <SparkFun_Si7021_Breakout_Library.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <WiFi.h>

// INTERNET
const char* ssid = "oneonetwo fake";
const char* password = "bluedragon";
const char* mqtt_server = "farmer.cloudmqtt.com";
#define mqtt_port 11850
#define MQTT_USER "cbcsnhtj"
#define MQTT_PASSWORD "V_5lMKhBkM-p"
#define MQTT_SERIAL_PUBLISH_CH "/icircuit/ESP32/serialdata/tx"
#define MQTT_SERIAL_RECEIVER_CH "/icircuit/ESP32/serialdata/rx"
#define LED 13
WiFiClient wifiClient;
PubSubClient client(wifiClient);

//Measurment
float humidity = 0;
float temp = 0;
//D Weather sensor;
float measurmentFrq = 0.1;


void setup() {
  //debug
  Serial.begin(9600);
  Serial.println("starting setup");
  pinMode(LED, OUTPUT);
  setupSpark();
  setupWifi();
  setupMqtt();
  Serial.println("ending setup");
}
void setupSpark() {
  Serial.println("Starting setupSpark");
//D   sensor.begin();
  Serial.println("ending setupSpark");
}

void setupWifi() {
 Serial.println("Starting setupWifi ");
 WiFi.begin(ssid, password);
 while (WiFi.status() != WL_CONNECTED) {
  delay(500);
  Serial.print(" . ");
 }
 Serial.println("ending setupWifi");
}

void setupMqtt() {
  Serial.println("Starting setupMqtt");
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  Serial.println("ending setupMqtt");

}


void loop() {
  if(!client.loop()) {
    client.reconnect();
  }
  measure();
  Serial.print("temp: ");
  Serial.println(temp);
  Serial.print("Humidity: ");
  Serial.println(humidity);
  publishPayload();
  digitalWrite(LED,HIGH);
  delay(1/measurmentFrq * 10000);
  digitalWrite(LED, LOW);

}

void measure() {
//D   humidity = sensor.getRH();
//D   temp = sensor.readTemp();
}

//MQTTSTUFF
void publishPayload(char *data){
  if (!client.connected()) {
    reconnect();
  }
  //Sweet sweet overflow errors
  char strTmp[32];
  char strHum[32];
  dtostrf(temp,5,2,strTmp);
  dtostrf(humidity,5,2,strHum);
  client.beginPublish(MQTT_SERIAL_PUBLISH_CH,70,false);
  client.write(strtmp,32);
  client.write(":",1);
  client.write(strHum,32);
  client.endPublish();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
     Create a random client ID
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
     Attempt to connect
    if (client.connect(clientId.c_str(),MQTT_USER,MQTT_PASSWORD)) {
      Serial.println("connected");
      //Once connected, publish an announcement...
      client.publish("/icircuit/presence/ESP32/", "hello world");
      // ... and resubscribe
      client.subscribe(MQTT_SERIAL_RECEIVER_CH);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }

void callback,(char* topic, byte *payload, unsigned int length) {
    Serial.println("-------new message from broker-----");
    Serial.print("channel:");
    Serial.println(topic);
    Serial.print("data:");
    Serial.write(payload, length);
    Serial.println();
    //Very unsafe
    //measurmentFrq = atof(payload)
}
