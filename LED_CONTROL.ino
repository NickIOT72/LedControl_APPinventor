#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#ifndef STASSID
#define STASSID "WiFi-Sys"
#define STAPSK  "gdfx6327"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

ESP8266WebServer server(80);

//Pins
#define PinRED D1
#define PinBLUE D2
#define PinGREEN D3

boolean AllowRGB = false;

//PWM LED
int REDPWM = 0;
int GREENPWM = 0;
int BLUEPWM = 0;

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup(void) {
  pinMode(PinGREEN, OUTPUT);
  pinMode(PinBLUE, OUTPUT);
  pinMode(PinRED, OUTPUT);
  digitalWrite(PinRED, 0);
  digitalWrite(PinBLUE, 0);
  digitalWrite(PinGREEN, 0);
  Serial.begin(115200);
  IPAddress ip(192,168,43,90);
  IPAddress gateway(192,168,43,1);
  IPAddress subnet(255,255,255,0);
  IPAddress dns(8,8,8,8);
  WiFi.disconnect();
  WiFi.hostname("ESP_AP");
  WiFi.config(ip, gateway, subnet, dns);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/LED", LED);
  server.on("/RGB", RGB);
  server.on("/SWITCH", SWITCH);

  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
}


void LED()
{
  //Serial.println("HOLA");
  DataReception();
  server.send(200, "text/plane","OK");
}

void RGB()
{
  DataReception();
  if (AllowRGB)
  {
    analogWrite(PinRED, REDPWM);
    analogWrite(PinGREEN, GREENPWM);
    analogWrite(PinBLUE, BLUEPWM);
  }
  server.send(200, "text/plane","OK");
}

void SWITCH()
{
  DataReception();
  server.send(200, "text/plane","OK");
}

void DataReception()
{
  Serial.println("Number of Arguments: ");
  Serial.println(server.args());
  for (int i = 0; i < server.args(); i++)
  {
    Serial.print("Arg num");
    Serial.print(i);
    Serial.print(" -> ");
    Serial.print(server.argName(i));
    Serial.print(": ");
    Serial.println(server.arg(i));
    //LED
    if (server.argName(i) == "Status")
    {
      if (server.arg(i) == "1")
      {
        digitalWrite(PinGREEN,HIGH);
      }
      else if (server.arg(i) == "0")
      {
        digitalWrite(PinGREEN,LOW);
      }
    }
    // Switch
    else if (server.argName(i) == "Sw")
    {
      if (server.arg(i) == "ON")
      {
        AllowRGB = true;
        REDPWM = 0;
        GREENPWM = 0;
        BLUEPWM = 0;
        analogWrite(PinRED, REDPWM);
        analogWrite(PinGREEN, GREENPWM);
        analogWrite(PinBLUE, BLUEPWM);
        digitalWrite(PinGREEN,LOW);
        
      }
      else if (server.arg(i) == "OFF")
      {
        AllowRGB = false;
        REDPWM = 0;
        GREENPWM = 0;
        BLUEPWM = 0;
        analogWrite(PinRED, REDPWM);
        analogWrite(PinGREEN, GREENPWM);
        analogWrite(PinBLUE, BLUEPWM);
      }
    }
    //LED RGB
    else if (server.argName(i) == "RED")
    {
        REDPWM = server.arg(i).toInt();
    }
    else if (server.argName(i) == "GREEN")
    {
        GREENPWM = server.arg(i).toInt();
    }
    else if (server.argName(i) == "BLUE")
    {
        BLUEPWM = server.arg(i).toInt();
    }
  }
}
