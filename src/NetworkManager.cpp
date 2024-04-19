#include <ArduinoOTA.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>

#include "NetworkManager.h"
#include "settings.h"

/********************************************************/
/******************** Public Method *********************/
/********************************************************/

NetworkManager::NetworkManager() : _tickNTPUpdate(0), _otaStartFunc(nullptr)
{}

NetworkManager::~NetworkManager() 
{}

void NetworkManager::setup()
{
    /* Initialize Wifi */
    _wifiManager.setDebugOutput(true);

    Log.println("Try to connect to WiFi...");
    _wifiManager.setConfigPortalTimeout(300); // Set Timeout for portal configuration to xx seconds
    if (not _wifiManager.autoConnect(DEFAULT_HOSTNAME))
    {
        Log.println("failed to connect and hit timeout");
        delay(3000);
        //reset and try again, or maybe put it to deep sleep
        ESP.reset();
        delay(5000);
    }

    MDNS.begin(DEFAULT_HOSTNAME);
    _telnetSerialStream.begin();
    Log.addPrintStream(std::make_shared<TelnetSerialStream>(_telnetSerialStream));
    
    Log.println(String("Connected to ") + WiFi.SSID());
    Log.println(String("IP address: ") + WiFi.localIP().toString());

    /* Initialize OTA Server */
    Log.println("Arduino OTA activated");

    // Port defaults to 8266
    ArduinoOTA.setPort(8266);

    // Hostname defaults to esp8266-[ChipID]
    ArduinoOTA.setHostname(DEFAULT_HOSTNAME);

    ArduinoOTA.onStart([&]() {
        Log.println("Arduino OTA: Start updating");
        if (_otaStartFunc == nullptr)
            _otaStartFunc();
    });

    ArduinoOTA.onEnd([]() {
        Log.println("Arduino OTA: End");
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Log.print("Arduino OTA Progress: ");
        Log.print(String(progress / (total / 100)));
        Log.println(" %");
    });

    ArduinoOTA.onError([](ota_error_t error) {
        Log.print("Arduino OTA Error : ");
        Log.print(String(error));
        if (error == OTA_AUTH_ERROR)
            Log.println("Arduino OTA: Auth Failed");
        else if (error == OTA_BEGIN_ERROR)
            Log.println("Arduino OTA: Begin Failed");
        else if (error == OTA_CONNECT_ERROR)
            Log.println("Arduino OTA: Connect Failed");
        else if (error == OTA_RECEIVE_ERROR)
            Log.println("Arduino OTA: Receive Failed");
        else if (error == OTA_END_ERROR)
            Log.println("Arduino OTA: End Failed");
    });

    ArduinoOTA.begin();

    /* Initialize MDNS */
    //MDNS.addService("http", "tcp", 80);
}

void NetworkManager::handle()
{
    ArduinoOTA.handle();
    MDNS.update();

    unsigned long currentMillis = millis();
    if ((currentMillis - _tickNTPUpdate) >= (unsigned long)(NTP_UPDATE_INTERVAL_MSEC))
    {
        updateNTP();
        _tickNTPUpdate = currentMillis;
    }
}

WiFiClient& NetworkManager::client()
{
    return _wifiClient;
}

void NetworkManager::updateNTP()
{
  configTime(UTC_OFFSET * 3600, 0, NTP_SERVERS);
  delay(500);
  while (!time(nullptr))
  {
    Log.print("#");
    delay(1000);
  }
  Log.println("Update NTP");
}

void NetworkManager::onOTAStart(THandlerFunction fn)
{
    _otaStartFunc = fn;
}


#if !defined(NO_GLOBAL_INSTANCES) 
NetworkManager Network;
#endif
