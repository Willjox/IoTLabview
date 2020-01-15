//Uses a adafruit esp32 feather and Sparkfun si7021 temperature/humidty sensor
#include <SparkFun_Si7021_Breakout_Library.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <WiFi.h>

// INTERNET
const char* ssid = "jox";
const char* password = "jox12345";
const char* mqtt_server = "mqtt.jox.nu";
#define mqtt_port  1883
#define MQTT_USER "jjxnaifl"
#define MQTT_PASSWORD "bd5eXyqRYirm"
#define MQTT_READINGS_CH "/tmp"
#define MQTT_MEASURE_FREQENCY_CH "/frq"
#define LED 13
WiFiClient wifiClient;
PubSubClient client(wifiClient);

//Measurment
float humidity = 0;
float temp = 0;
Weather sensor;
float measurmentFrq = 0.1;


void setup() {
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
  sensor.begin();
  Serial.println("ending setupSpark");
}

void setupWifi() {
 Serial.println("Starting setupWifi ");
 WiFi.begin(ssid, password);
 while (WiFi.status() != WL_CONNECTED) {
  delay(500);
  Serial.print(" . ");
 }
 Serial.println(" ");
 Serial.println("ending setupWifi");
}
//Handles recieved messages(poorly)
void callback(char* topic, byte *payload, unsigned int length) {
    Serial.println("-------new message from broker-----");
    Serial.print("channel:");
    Serial.println(topic);
    Serial.print("data:");
    char strPayload[8] = {0};
    //Copies the payload byte by byte into the char array, will result in garbage if the bytes are not ascii code, Beware of overflow edge cases!
    for (int i = 0; i < length && i < sizeof(strPayload) - 1; i++ ) {
      strPayload[i] = (char) payload[i];
    }
    strcat(strPayload,"\0");
    Serial.println(strPayload);
    measurmentFrq = atof(strPayload);
    //avoids negative frequency and divide by zero due to human error or garbage in payload
    if (measurmentFrq <= 0) {
      Serial.println("Error: invalid frequency");
      measurmentFrq = 0.1;
    }
}


void setupMqtt() {
  Serial.println("Starting setupMqtt");
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  Serial.println("ending setupMqtt");
}


void loop() {
  client.loop();
  measure();
  Serial.print("temp: ");
  Serial.println(temp);
  Serial.print("Humidity: ");
  Serial.println(humidity);
  publishPayload();
  digitalWrite(LED,LOW);
  delay(1/measurmentFrq * 1000);
  digitalWrite(LED, HIGH);

}
//reads and stores humidty and temperature values.
void measure() {
    humidity = sensor.getRH();
    temp = sensor.readTemp();
}
//reconnect() is copied from the pubsub example library
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
     //Create a random client ID
    char clientId[] = "labviewIot";
     //Attempt to connect
    if (client.connect(clientId,MQTT_USER,MQTT_PASSWORD)) {
      Serial.println("connected");
      //Once connected, publish an announcement...
      client.publish("/hello/world", "hello world");
      // ... and resubscribe
      client.subscribe(MQTT_MEASURE_FREQENCY_CH);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
//publishes readings to broker.
void publishPayload(){
  if (!client.connected()) {
    reconnect();
  }
  //Sweet sweet overflow errors
  char strTmp[16];
  char strHum[16];
  char strPayload[33] = {0};
  dtostrf(temp,5,2,strTmp);
  dtostrf(humidity,5,2,strHum);
  strcat(strPayload,strTmp);
  strcat(strPayload,":");
  strcat(strPayload,strHum);
  strcat(strPayload,"\0");
  client.publish(MQTT_READINGS_CH,strPayload);
}
