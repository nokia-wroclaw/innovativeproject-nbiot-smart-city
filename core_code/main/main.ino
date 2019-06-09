#include <OneWire.h>
#include <SPI.h>
#include <WiFi101.h>
#include <PubSubClient.h>

#include "arduino_secrets.h"

#define phPIN A0
#define ppmPIN A1
#define czystPIN A2
OneWire  ds(10);  // on pin 10 (a 4.7K resistor is necessary)

float celsius;

float temperature()
{
  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];


  if ( !ds.search(addr)) {
    Serial.println("No more addresses.");
    Serial.println();
    ds.reset_search();
    delay(250);
    //    return celsius;
  }

  if (OneWire::crc8(addr, 7) != addr[7]) {
    Serial.println("CRC is not valid!");
    return celsius;
  }
  Serial.println();

  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);

  delay(1000);
  present = ds.reset();
  ds.select(addr);
  ds.write(0xBE);

  Serial.print("  Data = ");
  Serial.print(present, HEX);
  Serial.print(" ");
  for ( i = 0; i < 9; i++) {
    data[i] = ds.read();
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
  Serial.print(" CRC=");
  Serial.print(OneWire::crc8(data, 8), HEX);
  Serial.println();

  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3;
    if (data[7] == 0x10) {
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);

    if (cfg == 0x00) raw = raw & ~7;
    else if (cfg == 0x20) raw = raw & ~3;
    else if (cfg == 0x40) raw = raw & ~1;
  }
  celsius = (float)raw / 16.0;
  Serial.print("  Temperature = ");
  Serial.print(celsius);
  Serial.print(" Celsius, ");
  Serial.println();
  ds.reset_search();
  return celsius;
}

//sensitive data in arduino_secrets.h
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
int status = WL_IDLE_STATUS;
int val = 0;
String value = "";

void callback(char* topic, byte* payload, unsigned int length) {
}
WiFiClient wificlient;
PubSubClient client(wificlient);

void setup() {
  client.setServer(SERVER_URL, PORT);
  client.setCallback(callback);
  Serial.begin(9600);

  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    while (true);
  }


  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(10000);
  }

  Serial.print("You're connected to the network");
  printCurrentNet();
  printWiFiData();
}

void loop() {
  float temp =  temperature();
  temperatureUpdate(temp);
  float ph = analogRead(phPIN);
  Serial.print("\tph: ");
  Serial.print(ph);
  Serial.println();
  PhUpdate(ph);

  int ppm = analogRead(ppmPIN);
  Serial.print("\tppm: ");
  Serial.print(ppm);
  Serial.println();
  PpmUpdate(ppm);

  int czyst = analogRead(czystPIN);
  float fczyst = czyst * (5.0 / 1024.0);
    Serial.print("\tczyst: ");
  Serial.print(fczyst);
  Serial.println();
  CzystUpdate(fczyst);
  
  delay(10000);

}

void CzystUpdate(float fczyst)
{
  char cstr[8];
  
  sprintf(cstr, "%.2f", fczyst);
  if (client.connect(TOKEN, USER, PASSWORD))
  {
    client.publish(TOPIC_CZYST , cstr);
  }
  else
  {
    reconnect();
  }
}

void PpmUpdate(int ppm)
{
  char cstr[8];
  itoa(ppm, cstr, 10);
  if (client.connect(TOKEN, USER, PASSWORD))
  {
    client.publish(TOPIC_PPM , cstr);
  }
  else
  {
    reconnect();
  }
  
}
void PhUpdate(float mV)
{
  char cstr[8];
  sprintf(cstr, "%.2f", mV);
  if (client.connect(TOKEN, USER, PASSWORD))
  {
    client.publish(TOPIC_PH , cstr);
  }
  else
  {
    reconnect();
  }
}

void temperatureUpdate(float temp)
{
  char cstr[8];
  sprintf(cstr, "%.2f", temp);
  if (client.connect(TOKEN, USER, PASSWORD)) 
  {
    client.publish(TOPIC_TEMP , cstr);
  }
  else
  {
    reconnect();
  }
}

void printWiFiData() {
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.println(ip);

  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  printMacAddress(mac);
}

void printCurrentNet() {
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  byte bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("BSSID: ");
  printMacAddress(bssid);

  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);

  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption, HEX);
  Serial.println();
}

void printMacAddress(byte mac[]) {
  for (int i = 5; i >= 0; i--) {
    if (mac[i] < 16) {
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);
    if (i > 0) {
      Serial.print(":");
    }
  }
  Serial.println();
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(TOKEN, USER, PASSWORD)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
