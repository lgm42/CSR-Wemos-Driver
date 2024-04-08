#include <Arduino.h>

#include <HADevice.h>
#include <HAMqtt.h>
#include <Client.h>
#include <device-types/HANumber.h>
#include <device-types/HASensorNumber.h>

class CustomHaDevice
{
    public:
        CustomHaDevice(String uniqueId, Client & client);
        virtual ~CustomHaDevice();

        void setup();
        void loop();

        void networkFrequency(float value);
    private:
        String _uniqueId;
        HADevice _device;
        HAMqtt _mqtt;
        float _networkFrequency;
        String _networkFrequencyName;
        HASensorNumber _networkFrequencyEntity;
        HASensorNumber _uptimeSensorEntity;
        unsigned long _last_report;
};