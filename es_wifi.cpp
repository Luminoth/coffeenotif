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

#include <Arduino.h>
#include <WiFi101.h>
#include "es_core.h"
#include "es_wifi.h"

// https://www.arduino.cc/en/Reference/WiFi101
// https://www.arduino.cc/en/Guide/ArduinoWiFiShield101

namespace energonsoftware
{
    void WiFi::print_firmware_version()
    {
        Serial.print("WiFi firmware version: ");
        Serial.println(::WiFi.firmwareVersion());
    }

    void WiFi::print_mac_address()
    {
        static byte mac[6];
        ::WiFi.macAddress(mac);

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

    void WiFi::print_shield_info()
    {
        print_firmware_version();
        print_mac_address();
    }

    void WiFi::print_ssid()
    {
        Serial.print("SSID: ");
        Serial.println(::WiFi.SSID());
    }

    void WiFi::print_ip_address()
    {
        const IPAddress local_ip(::WiFi.localIP());
        const IPAddress subnet(::WiFi.subnetMask());

        Serial.print("IP Address: ");
        Serial.print(local_ip);
        Serial.print(" / ");
        Serial.println(subnet);

        const IPAddress gateway(::WiFi.gatewayIP());

        Serial.print("Gateway: ");
        Serial.println(gateway);
    }

    void WiFi::print_signal_strength()
    {
        Serial.print("Signal strength (RSSI): ");
        Serial.print(::WiFi.RSSI());
        Serial.println(" dBm");
    }

    void WiFi::print_connection_info()
    {
        print_ssid();
        print_signal_strength();
        print_ip_address();
    }

    WiFi::WiFi()
        : _use_dhcp(true), _ip_address(), _dns_server(), _gateway(), _subnet(),
            _ssid(), _encryption_type(ENC_TYPE_NONE),
            _wep_key(), _wep_key_index(0),
            _wpa_password(),
            _connected(false), _reconnect_delay_ms(10 * 1000)
    {
    }

    bool WiFi::init()
    {
        // verify we have the WiFi shield
        if(WL_NO_SHIELD == ::WiFi.status()) {
            Serial.println("WiFi shield not present!");
            return false;
        }

        Serial.println("Initializing WiFi...");

        if(!_use_dhcp) {
// TODO: sort this out to enable these other options
            //::WiFi.config(_ip_address, _dns_server, _gateway, _subnet);
            ::WiFi.config(_ip_address);
        } else {
            Serial.println("Using DHCP...");
        }

        return true;
    }

    bool WiFi::connect(int error_led_pin)
    {
        uint8_t status = ::WiFi.status();
        if(WL_CONNECTED == status) {
            return false;
        }

        if(_connected) {
            Serial.println("WiFi disconnected!");
            _connected = false;
        }

        while(WL_CONNECTED != status) {
            Serial.println("Attempting to connect to SSID: " + _ssid);

            switch(_encryption_type)
            {
            case ENC_TYPE_NONE:
                status = ::WiFi.begin(_ssid);
                break;
            case ENC_TYPE_WEP:
                status = ::WiFi.begin(_ssid, _wep_key_index, _wep_key);
                break;
            case ENC_TYPE_TKIP: // WPA
            case ENC_TYPE_CCMP: // WPA2
                status = ::WiFi.begin(_ssid, _wpa_password);
                break;
            default:
                Serial.print("Unsupported encryption type: ");
                Serial.println(_encryption_type);
                safe_exit(error_led_pin);
                break;
            }

            if(WL_CONNECTED != status) {
                Serial.print("Connection failed: ");
                Serial.println(status);
                delay(_reconnect_delay_ms);
            }
        }

        Serial.println("Connection successful!");
        _connected = true;
        print_connection_info();

        return true;
    }

    void WiFi::disconnect()
    {
        Serial.println("Disconnecting WiFi...");
        ::WiFi.disconnect();
        _connected = false;
    }

    String WiFi::get_local_ip_address_str() const
    {
        static char ipstr[24];

        IPAddress ip_address = ::WiFi.localIP();
        sprintf(ipstr, "%d.%d.%d.%d", ip_address[0], ip_address[1], ip_address[2], ip_address[3]);
        return ipstr;
    }
}
