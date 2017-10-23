#include <Arduino.h>
#include <LoRa.h>
#include <SPI.h>
#include "SSD1306.h"
#include <WiFi.h>
#include <WiFiServer.h>

// GPIO5  -- SX1278's SCK
// GPIO19 -- SX1278's MISO
// GPIO27 -- SX1278's MOSI
// GPIO18 -- SX1278's CS
// GPIO14 -- SX1278's RESET
// GPIO26 -- SX1278's IRQ(Interrupt Request)

#define SS      18
#define RST     14
#define DI0     26
#define BAND    433E6  //915E6 -- 这里的模式选择中，检查一下是否可在中国实用915这个频段

SSD1306 display(0x3c, 4, 15);

WiFiServer server(2228);
WiFiClient client;

void setup() {  
  Serial.begin(115200);
  while (!Serial);
  
  uint8_t chipId[6];
  esp_efuse_read_mac(chipId);
  char ssid[128];
  sprintf(ssid, "VPU-%02x:%02x:%02x:%02x:%02x:%02x",chipId[0], chipId[1], chipId[2], chipId[3], chipId[4], chipId[5]);
  WiFi.softAP(ssid);
  
  SPI.begin(5, 19, 27, 18);
  LoRa.setPins(SS, RST, DI0);

  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("LoRa Initial OK!");

  server.begin();

  pinMode(16, OUTPUT);
  digitalWrite(16, LOW);
  delay(50); 
  digitalWrite(16, HIGH);

  display.init();

  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
}

void loop() {
  int packetSize;
  
  WiFiClient newClient = server.available();
  if (newClient) {
    client = newClient;
  }
  
  if (client)
  {
    packetSize = client.available();
    if (packetSize) {
      uint8_t packetBuffer[1024];
      int len = client.read(packetBuffer, packetSize);
      packetBuffer[len] = 0;
      
      LoRa.beginPacket();
      LoRa.print((const char *)packetBuffer);
      LoRa.endPacket();
      
      display.clear();
      display.drawString(0, 0, String("Wi-Fi: "));
      display.drawString(0, 10, String((const char *)packetBuffer));
      display.display();
    }
  }

  packetSize = LoRa.parsePacket();
  if (packetSize) {
    uint8_t packetBuffer[1024];
    int len = 0;
    while (LoRa.available()) {
      packetBuffer[len++] = (uint8_t)LoRa.read();
    }
    packetBuffer[len] = 0;

    if (client) {
      client.write(packetBuffer, len);
    }

    display.clear();
    display.drawString(0, 0, String("Radio: "));
    display.drawString(0, 10, String((const char *)packetBuffer));
    display.display();
  }
}

