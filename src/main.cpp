#include <WiFi.h>
#include <WebServer.h>

#include "FS.h"
#include <LittleFS.h>

#include "BBS_Server.h"

#include "Index.h"

void handle_NotFound();

/* Put your SSID & Password */
const char *ssid = "ThemSSID";  // Enter SSID here
const char *password = "ThemPassword";  //Enter Password here

Index *siteindex;

void setup()
{
    Serial.begin(115200);

    if (!LittleFS.begin(false))
    {
        Serial.println("Mount failed, giving up!");
        return;
    }

    File root = LittleFS.open("/");
    if (!root)
    {
        Serial.println("- failed to open directory");
        return;
    }
    if (!root.isDirectory())
    {
        Serial.println(" - not a directory");
        return;
    }
    File file = root.openNextFile();
    while (file)
    {
        if (file.isDirectory())
        {
            Serial.print("  DIR : ");
            Serial.println(file.name());
        }
        else
        {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("\tSIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }

    siteindex = new Index();

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected!");
    Serial.print("Got IP: ");
    Serial.println(WiFi.localIP());


    server.on("/", HTTP_GET, []()
              {
                  if (server.hasArg("thread"))
                  {
                      siteindex->ReturnPage(server.arg("thread").toInt());
                  }
                  else
                  {
                      siteindex->ReturnPage();
                  }
              }
    );

    server.on("/new", HTTP_POST, []()
              {

                  if (server.hasArg("parent") && server.hasArg("user") && server.hasArg("topic") && server.hasArg("body"))
                  {
                      siteindex->Post(server.arg("parent").toInt(),
                                      std::string(server.arg("user").c_str()),
                                      std::string(server.arg("topic").c_str()),
                                      std::string(server.arg("body").c_str()));
                  }
                  else
                  {
                      server.send(400, "text/html", "Input malformed!");
                  }
              }
    );

    server.onNotFound(handle_NotFound);

    server.begin();
    Serial.println("HTTP server started");
}

void loop()
{
    server.handleClient();
}

void handle_NotFound()
{
    server.send(404, "text/plain", "Not found");
}
