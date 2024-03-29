#pragma once

#include <Arduino.h>

// Version
#define VERSION                             "V1.0.0"

#define DEFAULT_HOSTNAME                    "CSRWemosDriver"

#define NTP_UPDATE_INTERVAL_MSEC     (1 * 3600 * 1000)      // Update time from NTP server every 1 hour

// LED
#define LED_PIN             LED_BUILTIN
// Timezone
#define UTC_OFFSET      +1

// change for different ntp (time servers)
#define NTP_SERVERS "0.fr.pool.ntp.org", "time.nist.gov", "pool.ntp.org"

#define SCR_PIN 4
#define ZERO_CROSS_PIN 5
#define CURRENT_SENSING_ANA A0