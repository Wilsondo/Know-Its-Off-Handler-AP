#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <WiFiClientSecure.h>
#include "LittleFS.h"

#define DEBUG_EN
#define SECURE_EN

/* WiFi Variables */
String router_ssid = "CasaGarcia"; // Router ssid for normal operation
String router_password = "P0rtlandTimberz!"; // Router password for normal operation

/* HTTPS Variables */
WiFiClientSecure httpsClient;
const uint16_t httpsPort = 443; // HTTPS port of host (default 443)
String host = "postman-echo.com"; // link to database/host
String trustRoot = R"EOF(
-----BEGIN CERTIFICATE-----
MIIFfjCCBGagAwIBAgIQDivd/iQtUtceNHMmaFc27TANBgkqhkiG9w0BAQsFADBG
MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRUwEwYDVQQLEwxTZXJ2ZXIg
Q0EgMUIxDzANBgNVBAMTBkFtYXpvbjAeFw0yMDA1MDcwMDAwMDBaFw0yMTA2MDcx
MjAwMDBaMBsxGTAXBgNVBAMTEHBvc3RtYW4tZWNoby5jb20wggEiMA0GCSqGSIb3
DQEBAQUAA4IBDwAwggEKAoIBAQDTYNFeARbSNWlGrFJQj33LJWyFRvs5YLhM3uXZ
lH92I/yRpGUIaZX79DHStxH38LI+MifkYpAtKhzq91+Q5IhY/uH7hv1mmyqaldIG
RxYhWlPtNCwHxr3vZmCrWkEtNjGpcVlYEz7WQXX1TAh5uur8Uz9pu7FMKwqU8MD/
CimcfK3mLpRYE+VjhEGb50dMOEKOu68kApXYVjWGI9TikHpACZe2rbIDKPaDG7zM
qkvVeELj5Ix9i+p6x3gv1DRd3DHCBifWvAdY+bVN3w9llGClXYOwLuHM2nvKL8eA
zYt4kSyUaq0f8GLMZpI5I5c5VOzjYkaQnoHuAq9yC0YV6QBTAgMBAAGjggKRMIIC
jTAfBgNVHSMEGDAWgBRZpGYGUqB7lZI8o5QHJ5Z0W/k90DAdBgNVHQ4EFgQUZZWc
Uq/uxWZ1LWqq8ozl4gg67OIwLwYDVR0RBCgwJoIQcG9zdG1hbi1lY2hvLmNvbYIS
Ki5wb3N0bWFuLWVjaG8uY29tMA4GA1UdDwEB/wQEAwIFoDAdBgNVHSUEFjAUBggr
BgEFBQcDAQYIKwYBBQUHAwIwOwYDVR0fBDQwMjAwoC6gLIYqaHR0cDovL2NybC5z
Y2ExYi5hbWF6b250cnVzdC5jb20vc2NhMWIuY3JsMCAGA1UdIAQZMBcwCwYJYIZI
AYb9bAECMAgGBmeBDAECATB1BggrBgEFBQcBAQRpMGcwLQYIKwYBBQUHMAGGIWh0
dHA6Ly9vY3NwLnNjYTFiLmFtYXpvbnRydXN0LmNvbTA2BggrBgEFBQcwAoYqaHR0
cDovL2NydC5zY2ExYi5hbWF6b250cnVzdC5jb20vc2NhMWIuY3J0MAwGA1UdEwEB
/wQCMAAwggEFBgorBgEEAdZ5AgQCBIH2BIHzAPEAdgD2XJQv0XcwIhRUGAgwlFaO
400TGTO/3wwvIAvMTvFk4wAAAXHuB/NKAAAEAwBHMEUCIQDT53eDqQF7O/nHFTO3
+Div0+LrPE9kWWnjm8AeVw3MjgIgaMnke/Vnf2m2SbUMEgJoiXtbfOBNyLxhiUIt
EirqvgQAdwBc3EOS/uarRUSxXprUVuYQN/vV+kfcoXOUsl7m9scOygAAAXHuB/Nm
AAAEAwBIMEYCIQDipZleDUEfvSoI6ezb+lOd4I5UjLf+0aPK8pV24YTGVgIhAOwk
qyApKJ6rEOn+cSSQr4Zszr8ca3YTITtes1v1QR/tMA0GCSqGSIb3DQEBCwUAA4IB
AQCK0cgm5BExJroZzjlmzb7kdeRo7mmfppXWBsnQSJRoVtcvv1nKQhsOuWz3Ouub
b3KWXzxcvAz14hZylrv5yS4XexWY7LmwhyCyvJWF3KDL4ZJRfIDMJHKCGg0eKakp
81lQ7lWLnkw/I5Bb3eqSfvpn19FKXFiD0uId7rTJ0LnACqHo81ObKyNof1UXHBk7
p7j4H+4mEJ4am+jsRT0kEl+ZUrBWAuhgoIpHDVZr+8wg8tprhcQwrgepJMXKB5ZS
ZCqo/OHYA3+TInTLFBP31QautGMtRWZ1N2tBe7B2Mv7kzT27sjHmfx4BB0FC6sXI
tOhB/0o+8lCaN2zdBNVXbVBv
-----END CERTIFICATE-----
)EOF";
X509List cert(trustRoot.c_str());

/* Device pinout Constants */
const int adcPin = A0;
const int statePin = 16;
const int modePin = 0;

/* Device AP and local Web Server Constants */
const char* ssid = "KIOSetup";  // Enter SSID here
const char* password = "letsgoducks";  //Enter Password here
IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);
const int webserverport = 80;
ESP8266WebServer server(webserverport);

/* Send Variables */
int deviceID = 1023;
int battery_voltage = 1023;
int device_state = 0;

/* Misc Variables */
int programmed = 1;

/* Setup Function, runs once on startup */
void setup() {

  #ifdef DEBUG_EN
  Serial.begin(115200);
  Serial.println("");
  Serial.println("");
  Serial.println("DEBUG MODE ENABLED\n");

  #ifdef SECURE_EN
  Serial.println("SECURE MODE ENABLED\n");
  #endif
  
  #endif

  //loadVariables();

  /* Check if user wants to enter in setup mode */
  
  pinMode(modePin, INPUT);
  programmed = digitalRead(modePin);

  #ifdef DEBUG_EN
  Serial.println("Read " + String(programmed) + " from modePin");
  #endif 
  
  if(programmed==0){
    /* Debug mode prints information to serial monitor */
    #ifdef DEBUG_EN
    Serial.println("Starting in Setup mode");
    #endif
  
    /* Open access point with specified SSID and password */
    WiFi.softAPConfig(local_ip, gateway, subnet);
    WiFi.softAP(ssid, password, 1, false, 4);

    #ifdef DEBUG_EN
    Serial.print("Accesspoint Opened @ ");
    Serial.print(WiFi.localIP());
    Serial.print(" gateway @ ");
    Serial.print(WiFi.gatewayIP());
    Serial.print(" subnet @ ");
    Serial.println(WiFi.subnetMask());
    #endif
    
    delay(100);
    server.on("/",handle_Root);
    server.on("/get",handle_Get);
    server.on("/reset",deepsleep);
    server.onNotFound(handle_NotFound);
    server.begin();
  
    #ifdef DEBUG_EN
    Serial.println("Web server started on port " + String(webserverport));
    #endif

    while(1){
      server.handleClient();
    }
  }
  else{

    #ifdef DEBUG_EN
    Serial.println("Starting in Normal mode");
    #endif

    pinMode(adcPin, INPUT);
    pinMode(statePin, INPUT);
    battery_voltage = analogRead(adcPin);
    device_state = digitalRead(statePin);

    #ifdef DEBUG_EN
    Serial.println("Read battery voltage as: " + String(battery_voltage));
    Serial.println("Read device state as: " + String(device_state));
    #endif

    #ifdef DEBUG_EN
    Serial.println("Initating connection to " + router_ssid);
    #endif
    
    WiFi.begin(router_ssid, router_password); // initate connection to specified router
    while (WiFi.status() != WL_CONNECTED) { 
      delay(100); // depending on connection times, this delay can be tuned to reduce power consumption
    }
    
    #ifdef DEBUG_EN
    Serial.print("Connected to " + router_ssid + " @");
    Serial.println(WiFi.localIP());
    #endif

    #ifdef SECURE_EN
    /* Need to update device time to current GMT as required for x.509 validation*/
    configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");
    
    #ifdef DEBUG_EN
    Serial.print("Waiting for NTP time sync: ");
    #endif 
    
    time_t now = time(nullptr);
    while (now < 8 * 3600 * 2) {
      delay(500);

      #ifdef DEBUG_EN
      Serial.print(".");
      #endif
      
      now = time(nullptr);
    }
    struct tm timeinfo;
    gmtime_r(&now, &timeinfo);

    #ifdef DEBUG_EN
    Serial.println("");
    Serial.print("Current time: ");
    Serial.print(asctime(&timeinfo));
    #endif
    
    httpsClient.setTrustAnchors(&cert);
    #else
    httpsClient.setInsecure();
    #endif
    
    #ifdef DEBUG_EN
    Serial.println("HTTPS Connecting to " + host);
    #endif

    int r=0; //retry counter
    while((!httpsClient.connect(host, httpsPort)) && (r < 30)){
      delay(100); // depending on connection times, this delay can be tuned to reduce power consumption
      r++;
    }
    
    #ifdef DEBUG_EN
    if(r==30) {
      Serial.println("Connection failed");
      deepsleep();
    }
    else {
      Serial.println("Connected to host");
    }
    #endif

    /* Send HTTP packet to secure host */
    httpsClient.print(generatePOST(host, deviceID, device_state, battery_voltage));

    #ifdef DEBUG_EN
    Serial.println("Request sent:");
    Serial.println(generatePOST(host, deviceID, device_state, battery_voltage));
    #endif

    #ifdef DEBUG_EN
    Serial.println("HTTPS Reply:");
    String line;
    while(httpsClient.available()){        
      line = httpsClient.readStringUntil('\n');  //Read Line by Line
      Serial.println(line); //Print response
    }
    #endif
  }
}

/* Loop function, runs continuously after startup */
void loop() {
  deepsleep();
}

/* Loads user configurable variables from LittleFS into dynamic memory */ 
void readVariables(){

}

/* Updates variables in LittleFS from dynamic memory */
void writeVariables(){
  
}

/* Handles a request for connection */
void handle_Root(){
  #ifdef DEBUG_EN
  Serial.println("Request for root");
  #endif
  server.send(200, "text/html", SendHTML(deviceID, router_ssid, router_password, host, trustRoot)); 
}

/* Handle a get request */
void handle_Get(){
  #ifdef DEBUG_EN
  Serial.println("Get request received ... server.args() == " + String(server.args()));
  Serial.println("Arg - Name - Value");
  for(int i = 0; i < server.args(); i++){
    Serial.println(String(i) + " - " + server.argName(i) + " - " + server.arg(i));
  }
  #endif
  if(server.args()>0){
    for(int i = 0; i < server.args(); i++){
      if(server.arg(i)!=""){;
        if(server.argName(i)=="routerSSID"){
          router_ssid = server.arg(i);
          #ifdef DEBUG_EN
          Serial.println("router_ssid updated to : " + router_ssid);
          #endif
        }
        else if(server.argName(i)=="routerPASS"){
          router_password = server.arg(i);
          #ifdef DEBUG_EN
          Serial.println("router_password updated to : " + router_password);
          #endif
        }
        else if(server.argName(i)=="hostID"){
          host = server.arg(i);
          #ifdef DEBUG_EN
          Serial.println("host updated to : " + host);
          #endif
        }
        else if(server.argName(i)=="sslCERT"){
          trustRoot = server.arg(i);
          #ifdef DEBUG_EN
          Serial.println("cert updated to : " + trustRoot);
          #endif
        }
      }
    }
  }
  server.sendHeader("Location","/");
  server.send(303);
}

/* Puts the ESP into deep sleep mode */
void deepsleep(){
  #ifdef DEBUG_EN
  Serial.println("");
  Serial.println("");
  Serial.println("ENTERING DEEP SLEEP MODE\n");
  delay(100);
  #endif
  ESP.deepSleep(0);
}

/* Handles a request for an invalid url */
void handle_NotFound(){
  #ifdef DEBUG_EN
  Serial.println("Invalid request");
  #endif
  server.send(404, "text/plain", "Page not found");
}

/* Creates a string that represents an HTML landing page for device setup */
String SendHTML(int devID, String routerSSID, String routerPASS, String hostID, String sslCERT){
  String ptr = "<html>";
  ptr +="<h1 style=\"text-align: left;\">Know It's Off Device Setup</h1>\n";
  ptr +="<p><strong>Device ID: " + String(devID) + "</strong></p>\n";
  ptr +=String("<p>")+"(enter this number during device configuration)</p>\n";
  ptr +="<p><strong><span style=\"text-decoration: underline;\">Router Information</span></strong></p>\n";
  ptr +="<p>SSID: " + routerSSID + "</p>\n";
  ptr +="<form action=\"/get\"><input name=\"routerSSID\" type=\"text\" /><input type=\"submit\" value=\"Update SSID\" /></form>\n";
  ptr +="<p>Password: " + routerPASS + "</p>\n";
  ptr +="<form action=\"/get\"><input name=\"routerPASS\" type=\"text\" /><input type=\"submit\" value=\"Update Password\" /></form>\n";
  ptr +="<p><strong><span style=\"text-decoration: underline;\">Database Information</span></strong></p>\n";
  ptr +="<p>Hostname: " + hostID + "</p>\n";
  ptr +="<form action=\"/get\"><input name=\"hostID\" type=\"text\" /><input type=\"submit\" value=\"Update Hostname\" /></form>\n";
  ptr +="<p>SSL Certificate: " +sslCERT + "</p>\n";
  ptr +="<form action=\"/get\"><input name=\"sslCERT\" type=\"text\" /><input type=\"submit\" value=\"Update Certificate\" /></form>\n";
  ptr +="<form action=\"/reset\"><input type=\"submit\" value=\"Reset\" /></form>\n";
  ptr +="</html>\n";
  return ptr;
}

/* Creates an HTTPS post request for updating a specified database with device battery, state, and id */
String generatePOST(String host, int deviceID, int deviceState, int deviceBattery){
  String body = "{\r\n";
  body += "\"device_id\": " + String(deviceID) + "," + "\r\n";
  body += "\"device_state\": " + String(deviceState) + "," + "\r\n";
  body += "\"device_battery\": " + String(deviceBattery) + "\r\n";
  body += "}";
  String post = "POST /post HTTP/1.1\r\n";
  post += "Host: " + host + "\r\n";
  post += "Content-Type: application/json\r\n";
  post += "Content-Length: " + String(body.length()) + "\r\n\r\n";
  post += body + "\r\n";
  post += "Connection: close\r\n\r\n";
  return post;
}
