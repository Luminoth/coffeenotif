/*
    Copyright (C) 2015 Energon Software

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

// https://www.arduino.cc/en/Guide/ArduinoZero
// http://arduino.stackexchange.com/questions/13178/classes-and-objects-how-many-and-which-file-types-i-actually-need-to-use-them

// **** https://www.arduino.cc/en/Tutorial.Wifi101GoogleCalendar (great example!)
// https://www.arduino.cc/en/Tutorial/WiFiWebClient

#include <SPI.h>
#include <WiFi101.h>
#include "es_core.h"
#include "es_wifi.h"

//// WIFI SETTINGS (WPA2 Enterprise not supported)
const wl_enc_type ENCRYPTION_TYPE = ENC_TYPE_CCMP;
const char WIFI_SSID[] = "unicron";
const char WIFI_KEY[] = "<YOUR-WEP-KEY>";
const int WIFI_KEY_INDEX = 0;
const char WIFI_PASSWORD[] = "cibertr0n";

const bool USE_DHCP = true;
const IPAddress IP_ADDRESS(127, 0, 0, 1);
//// END WIFI SETTINGS

energonsoftware::WiFi g_wifi;

// NOTE: WiFi101 shield uses digital pins 5, 6, 7 and should not be used
// the docs say pins 11, 12, and 13 are used for SPI but they're safe to re-use, I guess?
// and pin 10 is slave select

bool poll_button()
{
    // TODO
    return false;
}

void notify_slack_channel()
{
    // TODO
}

void setup()
{
    energonsoftware::init_serial(115200);

    g_wifi.set_encryption_type(ENCRYPTION_TYPE);
    g_wifi.set_ssid(WIFI_SSID);
    // TODO: WEP values
    g_wifi.set_wpa_password(WIFI_PASSWORD);

    g_wifi.set_use_dhcp(USE_DHCP);
    g_wifi.set_ip_address(IP_ADDRESS);

    if(!g_wifi.init()) {
        energonsoftware::safe_exit();
        return;
    }

    g_wifi.print_shield_info();
}

void loop()
{
    g_wifi.connect();

    if(poll_button()) {
        notify_slack_channel();
    }
}