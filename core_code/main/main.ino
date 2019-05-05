//#define ARDUINO_ARCH_ESP8266 1x
#include <OneWire.h>
#include <SPI.h>
#include <WiFi101.h>
#include <PubSubClient.h>

#include "arduino_secrets.h"


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
  ds.write(0x44, 1);        // start conversion, with parasite power on at the end

  delay(1000);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.

  present = ds.reset();
  ds.select(addr);
  ds.write(0xBE);         // Read Scratchpad

  Serial.print("  Data = ");
  Serial.print(present, HEX);
  Serial.print(" ");
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
  Serial.print(" CRC=");
  Serial.print(OneWire::crc8(data, 8), HEX);
  Serial.println();

  // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.
  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // "count remain" gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  }
  celsius = (float)raw / 16.0;
  Serial.print("  Temperature = ");
  Serial.print(celsius);
  Serial.print(" Celsius, ");
  Serial.println();
  ds.reset_search();
  return celsius;
}

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int status = WL_IDLE_STATUS;     // the WiFi radio's status
int val = 0;
String value = "";

void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
}
WiFiClient wificlient;
PubSubClient client(wificlient);

void setup() {
  client.setServer(SERVER_URL, PORT);
  client.setCallback(callback);
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
//  while (!Serial) {
//    ; // wait for serial port to connect. Needed for native USB port only
//  }

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  // attempt to connect to WiFi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  // you're connected now, so print out the data:
  Serial.print("You're connected to the network");
  printCurrentNet();
  printWiFiData();

}

void loop() {
  float temp =  temperature();
  temperatureUpdate(temp);
  float ph = analogRead(A0);
  Serial.print("\tph: ");
  Serial.print(ph);
  Serial.println();
  PhUpdate(ph);

  int ppm = analogRead(A1);
  Serial.print("\tppm: ");
  Serial.print(ppm);
  Serial.println();
  PpmUpdate(ppm);

  int czyst = analogRead(A2);
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
  
//  itoa(czyst, cstr, 10);
    sprintf(cstr, "%.2f", fczyst);
  if (client.connect(TOKEN, USER, PASSWORD)) //update temperatury
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
//    sprintf(cstr, "%.2f", ppm);
  if (client.connect(TOKEN, USER, PASSWORD)) //update temperatury
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
//  itoa(mV, cstr, 10);
    sprintf(cstr, "%.2f", mV);
  if (client.connect(TOKEN, USER, PASSWORD)) //update temperatury
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
  if (client.connect(TOKEN, USER, PASSWORD)) //update temperatury
  {
    client.publish(TOPIC_TEMP , cstr);
  }
  else
  {
    reconnect();
  }
}

void printWiFiData() {
  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.println(ip);

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  printMacAddress(mac);

}

void printCurrentNet() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the MAC address of the router you're attached to:
  byte bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("BSSID: ");
  printMacAddress(bssid);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);

  // print the encryption type:
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
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(TOKEN, USER, PASSWORD)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
