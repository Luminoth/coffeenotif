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
        : INetwork(),
            _use_dhcp(true), _ip_address(), _dns_server(), _gateway(), _subnet(),
            _ssid(), _encryption_type(ENC_TYPE_NONE),
            _wep_key(), _wep_key_index(0),
            _wpa_password(),
            _connected(false), _reconnect_delay_ms(10 * 1000),
            _client(), _udp()
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

    void WiFi::connect(int connecting_led_pin, int connected_led_pin)
    {
        uint8_t status = ::WiFi.status();
        if(WL_CONNECTED == status) {
            return;
        }

        if(_connected) {
            Serial.println("WiFi disconnected!");
            _connected = false;
        }

        while(WL_CONNECTED != status) {
            if(connecting_led_pin > 0) {
                // TODO: set connecting LED
            }

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
                safe_exit();
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

        if(connected_led_pin > 0) {
            // TODO: set connected LED
        }

        print_connection_info();
    }

    void WiFi::disconnect()
    {
        Serial.println("Disconnecting WiFi...");
        ::WiFi.disconnect();
        _connected = false;
    }

    bool WiFi::connect_server(const String& host, uint16_t port)
    {
        Serial.println("Connecting to host: " + host + ":" + port + "...");
        return _client.connect(host.c_str(), port) > 0;
    }

    bool WiFi::connect_server(const IPAddress& address, uint16_t port)
    {
        Serial.print("Connecting to address: ");
        Serial.print(address);
        Serial.print(":");
        Serial.print(port);
        Serial.println("...");
        return _client.connect(address, port) > 0;
    }

    void WiFi::disconnect_server()
    {
        Serial.println("Disconnecting from server...");
        _client.stop();
    }

    bool WiFi::begin_udp(uint16_t local_port)
    {
        Serial.print("UDP listen: ");
        Serial.println(local_port);
        return _udp.begin(local_port) > 0;
    }

    void WiFi::end_udp()
    {
        Serial.println("Stopping UDP...");
        _udp.stop();
    }

    bool WiFi::send_udp_packet(const String& host, uint16_t remote_port, const byte* const buffer, size_t buffer_len)
    {
        Serial.println("Sending UDP packet to " + host + ":" + remote_port + " (" + buffer_len + ")");

        if(_udp.beginPacket(host.c_str(), remote_port) < 1) {
            Serial.println("Failed to connect to UDP host!");
            return false;
        }

        _udp.write(buffer, buffer_len);

        if(_udp.endPacket() < 1) {
            Serial.println("Failed to send UDP packet!");
            return false;
        }

        return true;
    }

    bool WiFi::send_udp_packet(const IPAddress& address, uint16_t remote_port, const byte* const buffer, size_t buffer_len)
    {
        Serial.print("Sending UDP packet to ");
        Serial.print(address);
        Serial.print(":");
        Serial.print(remote_port);
        Serial.print(" (");
        Serial.print(buffer_len);
        Serial.println((")");

        if(_udp.beginPacket(address, remote_port) < 1) {
            Serial.println("Failed to connect to UDP address!");
            return false;
        }

        _udp.write(buffer, buffer_len);

        if(_udp.endPacket() < 1) {
            Serial.println("Failed to send UDP packet!");
            return false;
        }

        return true;
    }

    int WiFi::udp_available()
    {
        return _udp.available();
    }

    bool WiFi::parse_udp_packet()
    {
        return _udp.parsePacket() > 0;
    }

    bool WiFi::read_udp_packet(byte* const buffer, size_t buffer_len)
    {
        return _udp.read(buffer, buffer_len) > 0;
    }
}
