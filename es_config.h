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

#if !defined __ES_CONFIG_H__
#define __ES_CONFIG_H__

namespace energonsoftware
{
    class Config
    {
    private:
        static const char CONFIG_FILE[];
        
    public:
        Config();
        virtual ~Config() throw() { }
        
    public:
        unsigned int get_wifi_encryption_type() const { return _wifi_encryption_type; }
        void set_wifi_encryption_type(unsigned int wifi_encryption_type) { _wifi_encryption_type = wifi_encryption_type; }
    
        const String& get_wifi_ssid() const { return _wifi_ssid; }
        void set_wifi_ssid(const String& wifi_ssid) { _wifi_ssid = wifi_ssid; }
        
        const String& get_wifi_wep_key() const { return _wifi_wep_key; }
        void set_wifi_wep_key(const String& wifi_wep_key) { _wifi_wep_key = wifi_wep_key; }
        
        unsigned int get_wifi_wep_key_index() const { return _wifi_wep_key_index; }
        void set_wifi_wep_key_index(unsigned int wifi_wep_key_index) { _wifi_wep_key_index = wifi_wep_key_index; } 
        
        const String& get_wifi_wpa_password() const { return _wifi_wpa_password; }
        void set_wifi_wpa_password(const String& wifi_wpa_password) { _wifi_wpa_password = wifi_wpa_password; }
        
        bool get_use_dhcp() const { return _use_dhcp; }
        void set_use_dhcp(bool use_dhcp) { _use_dhcp = use_dhcp; }
        
        const String& get_static_ip_address() const { return _static_ip_address; }
        void set_static_ip_address(const String& static_ip_address) { _static_ip_address = static_ip_address; }
        
        uint16_t get_local_ntp_port() const { return _local_ntp_port; }
        void set_local_ntp_port(uint16_t local_ntp_port) { _local_ntp_port = local_ntp_port; }
        
        const String& get_ntp_host() const { return _ntp_host; }
        void set_ntp_host(const String& ntp_host) { _ntp_host = ntp_host; }
        
        unsigned int get_ntp_update_rate_ms() const { return _ntp_update_rate_ms; }
        void set_ntp_update_rate_minutes(unsigned int ntp_update_rate_minutes) { _ntp_update_rate_ms = ntp_update_rate_minutes * 60000; }
        
        const String& get_slack_api_token() const { return _slack_api_token; }
        void set_slack_api_token(const String& slack_api_token) { _slack_api_token = slack_api_token; }
        
        const String& get_slack_username() const { return _slack_username; }
        void set_slack_username(const String& slack_username) { _slack_username = slack_username; }
        
        const String& get_slack_channel() const { return _slack_channel; }
        void set_slack_channel(const String& slack_channel) { _slack_channel = slack_channel; }
        
        unsigned long get_coffee_brew_ms() const { return _coffee_brew_ms; }
        void set_coffee_brew_minutes(unsigned int coffee_brew_minutes) { _coffee_brew_ms = coffee_brew_minutes * 60000; }
        
        const String& get_random_coffee_notification(bool finished) const;

    public:
        // NOTE: this assumes the SD card is available
        bool read_from_sd();
        
    private:
        bool parse_config(const String& config);
        bool parse_config_line(const String& config_line);
    
    private:    
        unsigned int _wifi_encryption_type;
        String _wifi_ssid;
        String _wifi_wep_key;
        unsigned int _wifi_wep_key_index;
        String _wifi_wpa_password;
        
        bool _use_dhcp;
        String _static_ip_address;
        
        uint16_t _local_ntp_port;
        String _ntp_host;
        unsigned long _ntp_update_rate_ms;

        String _slack_api_token;
        String _slack_username;
        String _slack_channel;

        unsigned long _coffee_brew_ms;
    };
}

#endif
