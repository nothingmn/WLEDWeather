#include <Arduino.h>
#include <Arduino_JSON.h>
#include <ESP8266HTTPClient.h>
#include <NTPClient.h>
#include <WiFiClient.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <WiFiUdp.h>

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
String openWeatherMapApiKey = "ab30134860307ce8c12fcf86eb710d18";
// Replace with your country code and city
String city = "Burnaby";
String countryCode = "Canada";


String httpGETRequest(const char *serverName)
{
  HTTPClient http;

  // Your IP address with path or Domain name with URL path
  http.begin(serverName);

  // Send HTTP POST request
  int httpResponseCode = http.GET();

  String payload = "{}";

  if (httpResponseCode > 0)
  {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else
  {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}


void setup()
{
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP

  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  delay(3000);
  Serial.println("\n Starting");
  // WiFi.mode(WiFi_STA); // it is a good practice to make sure your code sets wifi mode how you want it.

  //WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wm;

  //reset settings - wipe credentials for testing
  //wm.resetSettings();

  // Automatically connect using saved credentials,
  // if connection fails, it starts an access point with the specified name ( "AutoConnectAP"),
  // if empty will auto generate SSID, if password is blank it will be anonymous AP (wm.autoConnect())
  // then goes into a blocking loop awaiting configuration and will return success result

  bool res;
  // res = wm.autoConnect(); // auto generated AP name from chipid
  // res = wm.autoConnect("AutoConnectAP"); // anonymous ap
  res = wm.autoConnect("Cloudy", "88888888"); // password protected ap

  if (!res)
  {
    Serial.println("Failed to connect");
    // ESP.restart();
    return;
  }
  else
  {
    //if you get here you have connected to the WiFi
    Serial.println("connected...yeey :)");
  }
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT -8 = -28800
  // GMT 0 = 0
  timeClient.setTimeOffset(-28800);
}

String jsonBuffer;

void loop()
{
  // put your main code here, to run repeatedly:
  timeClient.update();

  Serial.println(timeClient.getFormattedTime());

  String serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&APPID=" + openWeatherMapApiKey;

  jsonBuffer = httpGETRequest(serverPath.c_str());
  Serial.println(jsonBuffer);
  JSONVar myObject = JSON.parse(jsonBuffer);

  // JSON.typeof(jsonVar) can be used to get the type of the var
  if (JSON.typeof(myObject) == "undefined")
  {
    Serial.println("Parsing input failed!");
    return;
  }

  Serial.print("JSON object = ");
  Serial.println(myObject);
  Serial.print("Temperature: ");
  Serial.println(myObject["main"]["temp"]);
  Serial.print("Pressure: ");
  Serial.println(myObject["main"]["pressure"]);
  Serial.print("Humidity: ");
  Serial.println(myObject["main"]["humidity"]);
  Serial.print("Wind Speed: ");
  Serial.println(myObject["wind"]["speed"]);

  delay(10000);
  //1000 (1 second) * 60 (1 minute) * 60 (1 hour)
  //delay(1000 * 60 * 60);
}
