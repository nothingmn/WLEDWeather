#include <Arduino.h>

/* WIFI AP DEFAULTS
First connection, you will have to join the AP_NAME Access Point and configure it to your own home network.
Once you join the AP, browse to this URL:

http://192.168.4.1

*/
const char *AP_NAME = "WLEDWeather";
const char *AP_PASSWORD = "88888888";

/* OPEN WEATHER
Get your API key here:
https://openweathermap.org/api
*/
String openWeatherMapApiKey = "xxxxxxxxxxxxxxxxxxxxxxxxxx";
String city = "Burnaby";
String countryCode = "Canada";

/* WLED DEVICE AND WHICH EFFECTS TO CALL  {WLED_HOST}/win&PL={EFFECT}

Full list of conditions here:
https://openweathermap.org/weather-conditions

Make sure to setup the corresponding Effects on your WLED device.

WLED_SIMPLE means we will simplify the response from the weather report to the main 7 categories
WLED_ADVANCED means we will just send the weather ID value over
*/
const int WLED_SIMPLE = 0;
const int WLED_ADVANCED = 1;

String WLED_HOST = "http://10.0.0.98";
int WLED_MODE = WLED_SIMPLE;

/*EFFECTS

ONLY considering changing if you are using WLED_MODE = WLED_SIMPLE*/
int thunderstormEffect = 6;
int drizzleEffect = 3;
int rainEffect = 4;
int snowEffect = 5;
int atmosphereEffect = 9;
int clearEffect = 7;
int cloudsEffect = 8;

/* DELAY BETWEEN UPDATES
  1000 (1 second) * 60 (1 minute) * 5 (5 minutes)
  Careful not to hit limits on the weather API, once every 5 minutes seems good enough
  https://openweathermap.org/price
  Current limits for the free tier:
  60 calls/minute
  1,000,000 calls/month
*/
int updateDelay = 1000 * 60 * 5;
