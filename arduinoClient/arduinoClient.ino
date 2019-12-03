#include <WiFi.h>

void setup() {
  pinMode(13, OUTPUT);
  const char* ssid     = "Gaddafis internet";
  const char* password = "vfsrulez13";
  WiFi.begin(ssid, password);
  
}

void loop() {
  digitalWrite(13, HIGH);
  delay(500);
  digitalWrite(13, LOW);
  delay(500);
}
