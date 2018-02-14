#include <FS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include "EEPROM.h"


// the current address in the EEPROM (i.e. which byte
// we're going to write to next)
int addr = 0;
#define EEPROM_SIZE 64
char EEPROM_BUFFER[EEPROM_SIZE];

char first_eeprom_value;

const char *ssid = "TheFBall";
const char *password = "password";

AsyncWebServer server(80);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  
  Serial.println("initialize EEPROM...");
  if (!EEPROM.begin(EEPROM_SIZE))
  {
    Serial.println("failed to initialise EEPROM"); delay(1000000);
  }

  // Easily clear out EEPROM
  clear_eeprom();

  Serial.println(" bytes read from Flash . Values are:");
  for (int i = 0; i < EEPROM_SIZE; i++)
  {
    Serial.print(byte(EEPROM.read(i))); Serial.print(" ");
  }
  
  WiFi.softAP(ssid, password, 8, false);
  
 
  Serial.println(WiFi.softAPIP());

  server.on("/hello", HTTP_GET, [](AsyncWebServerRequest *request){
//    request->send(200, "text/plain", "Hello World");
    request->send(200, "text/html", 
    "<form action=\"update\" method=\"post\" name\"ssid_info\">"
      "<label for=\"ssid\">SSID: </label><input id=\"ssid\" name=\"ssid\" placeholder=\"Enter Your SSID\" /> <br />"
      "<label for=\"pass\">Password: </label><input id=\"pass\" name=\"pass\" placeholder=\"WiFi Password\" />"
      "<input type=\"reset\" value=\"Reset\"/>"
      "<input type=\"submit\" value=\"Save\"/>"
    "</form>");
  });

  server.on("/update", HTTP_POST, [](AsyncWebServerRequest *request){
    int params = request->params();
    char creds[62] ="";
    for(int i=0;i<params;i++){
      AsyncWebParameter* p = request->getParam(i);
      if(p->isFile()){ //p->isPost() is also true
        Serial.printf("FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
      } else if(p->isPost()){
        // Create a chart array based on the length of user input
        char arr[p->value().length() + 1];

        // concat our 'global' char[] -> creds with our newly created char array -> p->name().c_str()
        strcat(creds, strcpy(arr, p->value().c_str()));
        
        if(p->name() == "ssid"){
          String delimiterStr = ":";
          char delimiter[delimiterStr.length() + 1];
          strcat(creds, strcpy(delimiter, delimiterStr.c_str()));
        }
        
        
      } else {
        Serial.printf("GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
      }
    }

    Serial.printf("EEPROM => %s", creds);
    save_string_to_eeprom(creds);
    EEPROM.commit();

    request->send(200, "text/plain", "SSID updated successful");
  });

  server.begin();
}

void save_string_to_eeprom(char val[]){
  
  for(int i = 0; i < (strlen(val) - 1); i++){
    
      EEPROM.write(i, val[i]);
    
  }

}

void read_string_from_eeprom(){
  
  for(int i = 0; i < (EEPROM_SIZE - 1); i++){
  
    EEPROM_BUFFER[i] = EEPROM.read(i);
  
  }

}

void clear_eeprom(){
  for (int i = 0 ; i < EEPROM_SIZE; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
}

void loop() {
  delay(3000);
  
  Serial.println(" bytes read from Flash . Values are:");
  for (int i = 0; i < EEPROM_SIZE; i++)
  {
    Serial.print(byte(EEPROM.read(i))); Serial.print(" ");
  }
  read_string_from_eeprom();
  Serial.println("Buffer");
  String str(EEPROM_BUFFER);
  Serial.println(str );

}
