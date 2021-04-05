#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h> 
#include <LittleFS.h>

/* Variables */
String router_ssid = "test1";  // router ssid for normal operation
String router_password = "test2";  // router password for normal operation
String host = "test3";  // variable that holds the address of the database handler

AsyncWebServer server(80);  // webserver object that listens on port 80



void setup() {

  // Initialize serial connection for debug
  Serial.begin(115200);
  Serial.println("");
  Serial.println("");

  // Mount LittleFS filesystem
  LittleFS.begin();
  
  const char* local_ssid = "KIOSetup";  // ssid of device in AP mode
  const char* local_password = "letsgoducks";  // password of device in AP mode
  IPAddress local_ip(192,168,1,1);  
  IPAddress gateway(192,168,1,1);  // create ip address objects for access point configuration
  IPAddress subnet(255,255,255,0);
  WiFi.softAPConfig(local_ip, gateway, subnet);  // load access point configuration
  WiFi.softAP(local_ssid, local_password, 1, false, 4);  // launches local accesspoint *note password MUST be 8+ characters long or this function will fail*
  Serial.println("Launched AP");
  
  delay(1000);

  // Handle root request, send index.html that contains links to all other required files to display the landing page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("Request for root");
    request->send(LittleFS, "index.html", String(), false, processor);
  });

  // Handle request for bootstrap css file
  server.on("/bootstrap.min.css", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("Request for bootstrap.min.css");
    request->send(LittleFS, "bootstrap.min.css", "text/css");
  });

  // Handle request for boostrap javascript file 
  server.on("/bootstrap.bundle.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("Request for bootstrap.bundle.min.js");
    request->send(LittleFS, "bootstrap.bundle.min.js", "text/javascript");
  });
  
  // Handle request for certification file upload
  server.on("/upload", HTTP_POST, [](AsyncWebServerRequest *request){
    request->send(200);
  }, onUpload);
  
  // Handle request for certification file download
  server.on("/trust_root.cer", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "trust_root.cer");
  });

  // Handle variable update requests for host, router_ssid, router_password
  server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request){
    int params = request->params();
    for(int i=0;i<params;i++){
      AsyncWebParameter* p = request->getParam(i);
      if(p->name() == "routerSSID"){
        router_ssid=p->value();
      }
      else if(p->name() == "routerPASS"){
        router_password=p->value();
      }
      else if(p->name() == "hostID"){
        host=p->value();
      }
    }
    request->redirect("/");
  });

  // Handle request for calibrate funciton
  server.on("/calibrate", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200);
  });

  // Handle request for reset
  server.on("/reset", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200);
  });

  // Handle invalid request
  server.onNotFound([](AsyncWebServerRequest *request){
    request->send(404);
  });

  // Start server
  server.begin();
  Serial.println("Launched Webserver");
}

// Nothing needs to be run in the loop for an Asyncronous Server
void loop() {
}

// Writes data to a specified file
void writeFile(String filepath, String message){
  Serial.println("Writing File: " + filepath);
  File file = LittleFS.open(filepath, "w");
  if(file.print(message.c_str())){
    #ifdef DEBUG_EN
    Serial.println(filepath + " written with message: " + message);
    #endif
  }
  else{  
    #ifdef DEBUG_EN
    Serial.println("Write Failed");
    #endif
  }
  file.close();
}

// Handle upload request for trust_root.cer
void onUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
  Serial.println("UploadStart: trust_root.cer");
  File file = LittleFS.open("trust_root.cer", "w");
  for(size_t i=0; i<len; i++){
    file.write(data[i]);
  }
  if(final){
    Serial.printf("UploadEnd: %s, %u B\n", filename.c_str(), index+len);
  }
}

// Handles filling in displayed variables on the index.html webpage
String processor(const String&var){
  Serial.println("in processor");
  Serial.println(var);
  if(var=="ROUTERSSID"){
    return router_ssid;
  }
  else if(var=="HOSTNAME"){
    return host;
  }
  else if(var=="ROUTERPASSWORD"){
    return router_password;
  }
}
