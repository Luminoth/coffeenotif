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

#include <SPI.h>
#include <WiFi101.h>
#include "es_core.h"
#include "es_wifi.h"

//// WIFI SETTINGS (WPA2 Enterprise not supported)
const wl_enc_type ENCRYPTION_TYPE = ENC_TYPE_NONE;
const char WIFI_SSID[] = "<YOUR-SSID>";
const char WIFI_KEY[] = "<YOUR-WEP-KEY>";
const int WIFI_KEY_INDEX = 0;
const char WIFI_PASS[] = "<YOUR-WPA-PASSWORD>";

const bool USE_DHCP = true;
// TODO: add non-DHCP settings

const int RECONNECT_DELAY_MS = 10 * 1000;
//// END WIFI SETTINGS

bool g_connected = false;   // used to monitor disconnects

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
    init_serial();

    if(!init_wifi()) {
        safe_exit();
        return;
    }

    print_wifi_info();
}

void loop()
{
    connect_wifi();

    if(poll_button()) {
        notify_slack_channel();
    }
}
