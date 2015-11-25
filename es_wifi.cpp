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

#include <WiFi101.h>
#include "es_wifi.h"

// https://www.arduino.cc/en/Guide/ArduinoWiFiShield101
// https://www.arduino.cc/en/Tutorial/Wifi101ConnectWithWPA

// TODO: make a "settings" class or something for passing in the WiFi settings
// rather than having a bunch of weird globals

void print_wifi_firmware_version()
{
    Serial.print("WiFi firmware version: ");
    Serial.println(WiFi.firmwareVersion());
}

void print_wifi_mac_address()
{
    static byte mac[6];
    WiFi.macAddress(mac);

    Serial.print("MAC Address: ");
    Serial.print(mac[0], HEX);
    Serial.print(":");
    Serial.print(mac[1], HEX);
    Serial.print(":");
    Serial.print(mac[2], HEX);
    Serial.print(":");
    Serial.print(mac[3], HEX);
    Serial.print(":");
    Serial.print(mac[4], HEX);
    Serial.print(":");
    Serial.println(mac[5], HEX);
}

void print_wifi_info()
{
    print_wifi_mac_address();
    print_wifi_firmware_version();
}

void print_wifi_ip_address()
{
    IPAddress ipAddres = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ipAddres);
}

void print_wifi_signal_strength()
{
    Serial.print("Signal strength (RSSI): ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
}

void print_wifi_connection_info()
{
    print_wifi_ip_address();
    print_wifi_signal_strength();
}

bool init_wifi()
{
    // verify we have the WiFi shield
    if(WL_NO_SHIELD == WiFi.status()) {
        Serial.println("WiFi shield not present!");
        return false;
    }

    Serial.println("Initializing WiFi...");

    if(!USE_DHCP) {
        // TODO: use WiFi.config() to set the non-DHCP settings
    } else {
        Serial.println("Using DHCP...");
    }

    return true;
}

void connect_wifi()
{
    int status = WiFi.status();
    if(WL_CONNECTED == status) {
        return;
    }

    if(g_connected) {
        Serial.println("WiFi disconnected!");
        g_connected = false;
    }

    while(WL_CONNECTED != status) {
        // TODO: set connecting LED

        Serial.print("Attempting to connect to SSID: ");
        Serial.println(WIFI_SSID);

        switch(ENCRYPTION_TYPE)
        {
        case ENC_TYPE_NONE:
            status = WiFi.begin(WIFI_SSID);
            break;
        case ENC_TYPE_WEP:
            status = WiFi.begin(WIFI_SSID, WIFI_KEY_INDEX, WIFI_KEY);
            break;
        case ENC_TYPE_TKIP: // WPA
        case ENC_TYPE_CCMP: // WPA2
            status = WiFi.begin(WIFI_SSID, WIFI_PASS);
            break;
        default:
            Serial.print("Unsupported encryption type: ");
            Serial.println(ENCRYPTION_TYPE);
            safe_exit();
            break;
        }

        if(WL_CONNECTED != status) {
            Serial.print("Connection failed: ");
            Serial.println(status);
            delay(RECONNECT_DELAY_MS);
        }
    }

    Serial.println("Connection successful!");
    g_connected = true;
    // TODO: set connected LED
    print_wifi_connection_info();
}
