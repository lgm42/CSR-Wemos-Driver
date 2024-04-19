#pragma once

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <Ethernet.h>
#include <TLog.h> 
#include <TelnetSerialStream.h>

class NetworkManager 
{
  public:
    typedef std::function<void(void)> THandlerFunction;

	  NetworkManager();
  	virtual ~NetworkManager();

    void setup();
    void handle();

    WiFiClient& client();
    
  void onOTAStart(THandlerFunction fn);

  private:
    void updateNTP();
    
    unsigned long _tickNTPUpdate;
    WiFiManager _wifiManager;
    WiFiClient _wifiClient;
    TelnetSerialStream _telnetSerialStream;
    THandlerFunction _otaStartFunc;

};

#if !defined(NO_GLOBAL_INSTANCES)
extern NetworkManager Network;
#endif
