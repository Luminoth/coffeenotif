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
// https://www.arduino.cc/en/Tutorial/Wifi101ConnectWithWPA

#include <SPI.h>
#include <WiFi101.h>

const wl_enc_type ENCRYPTION_TYPE = ENC_TYPE_NONE;

const char WIFI_SSID[] = "<YOUR-SSID>";
const char WIFI_KEY[] = "<YOUR-WEP-KEY>";
const int WIFI_KEY_INDEX = 0;
const char WIFI_PASS[] = "<YOUR-WPA-PASSWORD>";

void safe_exit()
{
    // TODO: set a failure LED

    // TODO: can we call exit() instead of hard-locking the controller?
    while(true);
}

void init_serial()
{
    Serial.begin(9600);

    // wait for serial port to connect
    // (needed for native USB port only?)
    while(true) {
        if(Serial) {
            break;
        }
    }
}

void print_mac_address()
{
    static byte mac[6];
    WiFi.macAddress(mac);

    Serial.print("MAC: ");
    Serial.print(mac[5], HEX);
    Serial.print(":");
    Serial.print(mac[4], HEX);
    Serial.print(":");
    Serial.print(mac[3], HEX);
    Serial.print(":");
    Serial.print(mac[2], HEX);
    Serial.print(":");
    Serial.print(mac[1], HEX);
    Serial.print(":");
    Serial.println(mac[0], HEX);
}

void print_wifi_info()
{
    IPAddress ipAddres = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ipAddres);
}

void connect_wifi()
{
    int status = WiFi.status();
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
            break;
        }

        if(WL_CONNECTED != status) {
            Serial.print("Connection failed: ");
            Serial.println(status);
            delay(10000);
        }
    }

    Serial.println("Connection successful!");
    // TODO: set connected LED
    print_wifi_info();
}

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

    // verify we have the WiFi shield
    if(WL_NO_SHIELD == WiFi.status()) {
        Serial.println("WiFi shield not present!");
        safe_exit();
    }

    print_mac_address();
}

void loop()
{
    connect_wifi();

    if(poll_button()) {
        notify_slack_channel();
    }
}
