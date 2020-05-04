#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>


const byte DNS_PORT = 53;
IPAddress apIP(172, 217, 28, 1);
DNSServer dnsServer;
ESP8266WebServer webServer(80);

String PROGMEM responseHTML =
                      "<!DOCTYPE html><html lang='de'><head>"
                      "<meta name='viewport' content='width=device-width'>"
                      "<title>Uhrzeit einstellen</title></head><body>"
                      "<form action=\"/timeentered\">"
                      "<label for=\"appt\">Uhrzeit einstellen:  </label>"
                      "<input type=\"time\" id=\"timeform\" name=\"clockTime\" autofocus required>"
                      "<input type=\"submit\" value=\"Ok\">"
                      "</form>"
                      "</body></html>";

void handleTimeForm(){
  String s = webServer.arg("clockTime");
  webServer.send(200, "text/html", responseHTML);
  String h = splitString(s, ';', 0);
  String m = splitString(s, ';', 1);

  int hNumber = h.toInt();
  int mNumber = m.toInt();
   
}

void setup() {
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP("WordClock Setup");
  Serial.begin(115200);

  // if DNSServer is started with "*" for domain name, it will reply with
  // provided IP to all DNS request
  dnsServer.start(DNS_PORT, "*", apIP);

  // replay to all requests with same HTML
  webServer.onNotFound([]() {
    webServer.send(200, "text/html", responseHTML);
  });
  webServer.on("/timeentered", handleTimeForm);
    
  webServer.begin();
  Serial.println("started");
}

void loop() {
  dnsServer.processNextRequest();
  webServer.handleClient();
  
}

String splitString(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}
