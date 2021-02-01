/*************************************************************************
Title:    WiFi-Enabled Triple AC Switch controller
Authors:  Michael Petersen <railfan@drgw.net>
File:     $Id: $
License:  GNU General Public License v3

LICENSE:
    Copyright (C) 2021 Michael Petersen (railfan@drgw.net)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

*************************************************************************/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Wire.h>
#include <SPI.h>
extern "C" {
  #include "user_interface.h"
}

#define CKT1    15
#define CKT2    13
#define CKT3    12

const char* ssid     = "YOUR-WIRELESS-NETWORK";
const char* password = "YOUR-WIRELESS-PASSWORD";
const char* nodeName = "YOUR-HOSTNAME";

ESP8266WebServer server(80);
WiFiClient espClient;

uint8_t lightState = 0;

void wifiSetup()
{
  WiFi.mode(WIFI_STA); // Set to station mode - disable broadcasting AP SSID offers

  // Connect to WiFi network
  wifi_station_set_hostname((char*)nodeName);
  WiFi.begin(ssid, password);
  Serial.print("\n\r \n\rWorking to connect");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
}

void handle_root() 
{
  for (int i = 0; i < server.args(); i++) 
    if(server.argName(i) == "on")
      lightState = ((lightState << 1) + 1) & 0x7;
    else if(server.argName(i) == "allon")
      lightState = 7;
    else if(server.argName(i) == "off")
      lightState = (lightState >> 1) & 0x7;
    else if(server.argName(i) == "alloff")
      lightState = 0;
  
  String html = "<html><title>ESP-ACSW3 : " + String(nodeName) + "</title><body>";

  html += "CKT #1: ";
  if(lightState & 0x01)
  {
    html += "ON";
  }
  else
  {
    html += "OFF";
  }
  
  html += "<br />";
  
  html += "CKT #2: ";
  if(lightState & 0x02)
  {
    html += "ON";
  }
  else
  {
    html += "OFF";
  }
  
  html += "<br />";
  
  html += "CKT #3: ";
  if(lightState & 0x04)
  {
    html += "ON";
  }
  else
  {
    html += "OFF";
  }
  
  html += "<br />";
  
  server.send(200, "text/html", html);
  delay(100);
}

void handle_json() 
{
  String html = "{";
  
  html += "ckt1:";
  if(lightState & 0x01)
  {
    html += "1,";
  }
  else
  {
    html += "0,";
  }
  
  html += "ckt2:";
  if(lightState & 0x02)
  {
    html += "1,";
  }
  else
  {
    html += "0,";
  }
  
  html += "ckt3:";
  if(lightState & 0x04)
  {
    html += "1,";
  }
  else
  {
    html += "0,";
  }
  
  html += "}";
  
  server.send(200, "text/html", html);
  delay(100);
}
void setup(void)
{
  // You can open the Arduino IDE Serial Monitor window to see what the code is doing
  
  Serial1.begin(9600);  // Serial connection from ESP-01 via 3.3v console cable
  wifiSetup(); 
  Serial1.print("ESP-ACSW3");

  
  server.on("/", handle_root);
  server.on("/json", handle_json);
 
  server.begin();

  Serial1.print("HTTP server started");

  pinMode(CKT1, OUTPUT);
  pinMode(CKT2, OUTPUT);
  pinMode(CKT3, OUTPUT);

  digitalWrite(CKT1, 0);
  digitalWrite(CKT2, 0);
  digitalWrite(CKT3, 0);
}

void loop(void)
{
  server.handleClient();

  if(lightState & 0x01)
  {
    digitalWrite(CKT1, 1);
  }
  else
  {
    digitalWrite(CKT1, 0);
  }
  
  if(lightState & 0x02)
  {
    digitalWrite(CKT2, 1);
  }
  else
  {
    digitalWrite(CKT2, 0);
  }
  
  if(lightState & 0x04)
  {
    digitalWrite(CKT3, 1);
  }
  else
  {
    digitalWrite(CKT3, 0);
  }

}
