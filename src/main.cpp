#include <Arduino.h>
#include <Arduino_JSON.h>
#include <ESP8266HTTPClient.h>
#include <NTPClient.h>
#include <WiFiClient.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include "./config.h"

//SEE CONFIG.H for configuration options

void blink(int times, int wait) {
  for (int i = 0; i < times; i++)
  {
    digitalWrite(LED_BUILTIN, LOW);
    delay(wait);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(wait);
  }
}
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
    blink(5, 250);
  }
  // Free resources
  http.end();

  return payload;
}

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);

  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP

  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  delay(3000);
  Serial.println("\n Starting");
  blink(2, 250);
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
  res = wm.autoConnect(AP_NAME, AP_PASSWORD); // password protected ap

  if (!res)
  {
    Serial.println("Failed to connect to Wifi");
    //delay 10 seconds
    blink(4, 1000);
    // should we just restart?
    // ESP.restart();
    return;
  }
  else
  {
    //if you get here you have connected to the WiFi
    Serial.println("Connected to Wifi");
    blink(3, 500);

  }
}

String jsonBuffer;
String wledBuffer;

void loop()
{
  Serial.print("OpenWeather API Key: ");
  Serial.println(openWeatherMapApiKey);

  String serverPath = "http://api.openweathermap.org/data/2.5/weather?units=metric&q=" + city + "," + countryCode + "&APPID=" + openWeatherMapApiKey;

  jsonBuffer = httpGETRequest(serverPath.c_str());
  Serial.println(serverPath);
  Serial.println(jsonBuffer);
  JSONVar rawWeather = JSON.parse(jsonBuffer);

  // JSON.typeof(jsonVar) can be used to get the type of the var
  if (JSON.typeof(rawWeather) == "undefined")
  {
    Serial.println("Parsing input failed!");
    delay(updateDelay);
    return;
  }
  Serial.println("-----weather-----");
  int count = rawWeather["weather"].length();
  int weatherId = 800;
  String weatherMain = "Clear";

  if (count > 0)
  {
    weatherId = rawWeather["weather"][0]["id"];
    weatherMain = rawWeather["weather"][0]["main"];
    Serial.print("weatherId:");
    Serial.println(weatherId);
    Serial.print("weatherMain:");
    Serial.println(weatherMain);
  }
  Serial.println("----------------------");
  Serial.println("-----WLED-----");

  int effect = weatherId; //800

  //only mutate if we are in WLED_SIMPLE mode
  if (WLED_MODE == WLED_SIMPLE)
  {
    Serial.println("WLED_SIMPLE is in use, mapping effect");

    if (weatherId >= 200 && weatherId < 300)
      effect = thunderstormEffect;
    if (weatherId >= 300 && weatherId < 400)
      effect = drizzleEffect;
    if (weatherId >= 500 && weatherId < 600)
      effect = rainEffect;
    if (weatherId >= 600 && weatherId < 700)
      effect = snowEffect;
    if (weatherId >= 700 && weatherId < 800)
      effect = atmosphereEffect;
    if (weatherId == 800)
      effect = clearEffect;
    if (weatherId >= 801 && weatherId < 900)
      effect = cloudsEffect;
  }

  Serial.print("effect:");
  Serial.println(effect);
  String wledPath = WLED_HOST + "/win&PL="; //the lack of a ? is not a type-o.
  String wledUri = wledPath + effect;

  Serial.print("wledUri:");
  Serial.println(wledUri);

  HTTPClient http;
  http.begin(wledUri.c_str());
  int httpResponseCode = http.GET();
  http.end();

  Serial.print("WLED response:");
  Serial.println(httpResponseCode);
  Serial.println("----------------------");

  delay(updateDelay);
}
