#include <DNSServer.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include "ESPAsyncWebServer.h"

DNSServer dnsServer;
AsyncWebServer server(80);

int h = 12;
int m = 10;

int color1[] = {100, 100, 100};
int color2[] = {50, 200, 70};

class CaptiveRequestHandler : public AsyncWebHandler
{
public:
  CaptiveRequestHandler() {}
  virtual ~CaptiveRequestHandler() {}

  bool canHandle(AsyncWebServerRequest *request)
  {
    //request->addInterestingHeader("ANY");
    return true;
  }

  void handleRequest(AsyncWebServerRequest *request)
  {
    AsyncResponseStream *response = request->beginResponseStream("text/html");

    if (request->hasParam("clockTime"))
    {
      AsyncWebParameter *clockTimeParam = request->getParam("clockTime");
      clockTimeParam->value().c_str()
    }
    if (request->hasParam("backgroundColor"))
    {
      AsyncWebParameter *backgroundColor = request->getParam("backgroundColor");
    }
    if (request->hasParam("wordColor"))
    {
      AsyncWebParameter *wordColor = request->getParam("wordColor");
    }

    response->print("<!DOCTYPE html><html lang='de'><head>");
    response->print("<meta name='viewport' content='width=device-width'>");
    response->print("<style>");
    response->print("li{");
    response->print("list-style-type: none;");
    response->print("border-color: white;");
    response->print("border-style: solid;");
    response->print("text-align: center;");
    response->print("   }");
    response->print("</style>");
    response->print("</head>");
    response->print("<body>");
    response->print("<form action=\"/timeentered\">");
    response->print("<ul>");
    response->print("<li>");
    response->print("<h1>WordClock Configurator</h1>");
    response->print("<label for=\"timeform\">Uhrzeit einstellen:  </label>");
    response->printf("<input type=\"time\" id=\"timeform\" name=\"clockTime\" value=\"%02i:%02i\" autofocus required>", h, m);
    response->print("</li>");
    response->print("<li>");
    response->print("<label for=\"background\">Hintergrund Farbe:  </label>");
    response->printf("<input type= \"color\" id = \"backgroundColor\" name=\"backgroundColor\" value =\"#%02X%02X%02X\" required>", color1);
    response->print("</li>");
    response->print("<li>");
    response->print("<label for=\"wordColor\">Woerter Farbe:  </label>");
    response->printf("<input type= \"color\" id = \"wordColor\" name=\"wordColor\" value=\"#%02X%02X%02X\" required>", color2);
    response->print("</li>");
    response->print(" <li>");
    response->print("<input type=\"submit\" value=\"Ok\">");
    response->print("</li>");
    response->print("</ul>");
    response->print("</form>");
    response->print("</body></html>");
    request->send(response);
  }
};

void setup()
{
  //your other setup stuff...
  Serial.begin(115200);
  WiFi.softAP("esp-captive");
  dnsServer.start(53, "*", WiFi.softAPIP());
  server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER); //only when requested from AP
  //more handlers...
  server.begin();
}

void loop()
{
  Serial.println(ESP.getFreeHeap());
  dnsServer.processNextRequest();
}
