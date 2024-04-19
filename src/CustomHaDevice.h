#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Ethernet.h>

#include <HADevice.h>
#include <HAMqtt.h>
#include <Client.h>
#include <device-types/HANumber.h>
#include <device-types/HASensorNumber.h>

class CustomHaDevice
{
    public:
        CustomHaDevice();
        virtual ~CustomHaDevice();

        void setup();
        void loop();

        void networkFrequency(float value);
        float setPoint() const;

    private:
        WiFiClient _wifiClient;
        String _uniqueId;
        HADevice *_device;
        HAMqtt *_mqtt;
        float _networkFrequency;
        HASensorNumber _networkFrequencyEntity;
        HASensorNumber _uptimeSensorEntity;
        HANumber _powerSetPoint;
        HANumber _testNumber;

        unsigned long _last_report;
        float _setPointValue;

        static void onSetPointCommand(HANumeric number, HANumber* sender);

};

#if !defined(NO_GLOBAL_INSTANCES)
extern CustomHaDevice HaDevice;
#endif