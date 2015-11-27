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

#if !defined __ES_WIFI_H__
#define __ES_WIFI_H__

namespace energonsoftware
{
    class WiFi
    {
    public:
        static void print_firmware_version();
        static void print_mac_address();
        static void print_shield_info();

        static void print_ip_address();
        static void print_signal_strength();
        static void print_connection_info();

    public:
        WiFi();
        virtual ~WiFi() throw() { }

    public:
        void set_use_dhcp(bool use_dhcp) { _use_dhcp = use_dhcp; }
        bool get_use_dhcp() const { return _use_dhcp; }

        void set_ssid(const String& ssid) { _ssid = ssid; }
        const String& get_ssid() const { return _ssid; }

        void set_encryption_type(int encryption_type) { _encryption_type = encryption_type; }
        int get_encryption_type() const { return _encryption_type; }

        // TODO: access to the WEP values

        void set_wpa_password(const String& wpa_password) { _wpa_password = wpa_password; }
        const String& get_wpa_password() const { return _wpa_password; }

    public:
        bool init();
        void connect(int connectingLedPin=-1, int connectedLedPin=-1);

    private:
        bool _use_dhcp;

// TODO: add non-dhcp settings

        String _ssid;

        int _encryption_type;
        String _wep_key;
        int _wep_key_index;
        String _wpa_password;

        bool _connected;
        unsigned int _reconnect_delay_ms;
    };
}

#endif
