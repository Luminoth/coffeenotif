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

#include <HashMap.h>

namespace energonsoftware
{
    class Config
    {
    private:
        static const size_t CONFIG_SIZE = 16;

        static const char CONFIG_FILE[];

        static const String WIFI_ENCRYPTION_TYPE;
        static const String WIFI_SSID;
        static const String WIFI_WEP_KEY;
        static const String WIFI_WEP_KEY_INDEX;
        static const String WIFI_WPA_PASSWORD;

        static const String USE_DHCP;
        static const String IP_ADDRESS;

        static const String LOCAL_NTP_PORT;
        static const String NTP_HOST;
        static const String NTP_UPDATE_RATE_MINUTES;

        static const String SLACK_API_TOKEN;
        static const String SLACK_USERNAME;
        static const String SLACK_CHANNEL;

        static const String COFFEE_BREW_MS;
        
    public:
        Config();
        virtual ~Config() throw() { }
        
    public:
        unsigned int get_wifi_encryption_type() { return atoi(_config_map.getValueOf(WIFI_ENCRYPTION_TYPE).c_str()); }
        void set_wifi_encryption_type(unsigned int wifi_encryption_type) { _config_map[_config_map.getIndexOf(WIFI_ENCRYPTION_TYPE)](WIFI_ENCRYPTION_TYPE, String(wifi_encryption_type)); }
    
        String get_wifi_ssid() { return _config_map.getValueOf(WIFI_SSID); }
        void set_wifi_ssid(const String& wifi_ssid) { _config_map[_config_map.getIndexOf(WIFI_SSID)](WIFI_SSID, wifi_ssid); }
        
        String get_wifi_wep_key() { return _config_map.getValueOf(WIFI_WEP_KEY); }
        void set_wifi_wep_key(const String& wifi_wep_key) { _config_map[_config_map.getIndexOf(WIFI_WEP_KEY)](WIFI_WEP_KEY, String(wifi_wep_key)); }
        
        unsigned int get_wifi_wep_key_index() { return atoi(_config_map.getValueOf(WIFI_WEP_KEY_INDEX).c_str()); }
        void set_wifi_wep_key_index(unsigned int wifi_wep_key_index) { _config_map[_config_map.getIndexOf(WIFI_WEP_KEY_INDEX)](WIFI_WEP_KEY_INDEX, String(wifi_wep_key_index)); } 
        
        String get_wifi_wpa_password() { return _config_map.getValueOf(WIFI_WPA_PASSWORD); }
        void set_wifi_wpa_password(const String& wifi_wpa_password) { _config_map[_config_map.getIndexOf(WIFI_WPA_PASSWORD)](WIFI_WPA_PASSWORD, wifi_wpa_password); }
        
        bool get_use_dhcp() { return _config_map.getValueOf(USE_DHCP) == "true"; }
        void set_use_dhcp(bool use_dhcp) { _config_map[_config_map.getIndexOf(USE_DHCP)](USE_DHCP, String(use_dhcp)); }
        
        String get_static_ip_address() { return _config_map.getValueOf(IP_ADDRESS); }
        void set_static_ip_address(const String& static_ip_address) { _config_map[_config_map.getIndexOf(IP_ADDRESS)](IP_ADDRESS, static_ip_address); }
        
        uint16_t get_local_ntp_port() { return atoi(_config_map.getValueOf(LOCAL_NTP_PORT).c_str()); }
        void set_local_ntp_port(uint16_t local_ntp_port) { _config_map[_config_map.getIndexOf(LOCAL_NTP_PORT)](LOCAL_NTP_PORT, String(local_ntp_port)); }
        
        String get_ntp_host() { return _config_map.getValueOf(NTP_HOST); }
        void set_ntp_host(const String& ntp_host) { _config_map[_config_map.getIndexOf(NTP_HOST)](NTP_HOST, ntp_host); }
        
        uint32_t get_ntp_update_rate_ms() { return atoi(_config_map.getValueOf(NTP_UPDATE_RATE_MINUTES).c_str()) * 60000; }
        void set_ntp_update_rate_minutes(uint32_t ntp_update_rate_minutes) { _config_map[_config_map.getIndexOf(NTP_UPDATE_RATE_MINUTES)](NTP_UPDATE_RATE_MINUTES, String(ntp_update_rate_minutes)); }
        
        String get_slack_api_token() { return _config_map.getValueOf(SLACK_API_TOKEN); }
        void set_slack_api_token(const String& slack_api_token) { _config_map[_config_map.getIndexOf(SLACK_API_TOKEN)](SLACK_API_TOKEN, slack_api_token); }
        
        String get_slack_username() { return _config_map.getValueOf(SLACK_USERNAME); }
        void set_slack_username(const String& slack_username) { _config_map[_config_map.getIndexOf(SLACK_USERNAME)](SLACK_USERNAME, slack_username); }
        
        String get_slack_channel() { return _config_map.getValueOf(SLACK_CHANNEL); }
        void set_slack_channel(const String& slack_channel) { _config_map[_config_map.getIndexOf(SLACK_CHANNEL)](SLACK_CHANNEL, slack_channel); }
        
        uint32_t get_coffee_brew_ms() { return atoi(_config_map.getValueOf(COFFEE_BREW_MS).c_str()); }
        void set_coffee_brew_minutes(uint32_t coffee_brew_minutes) { _config_map[_config_map.getIndexOf(COFFEE_BREW_MS)](COFFEE_BREW_MS, String(coffee_brew_minutes * 60000)); }
        
        const String& get_random_coffee_notification(bool finished) const;

    public:
        void init();

        // NOTE: this assumes the SD card is available
        bool read_from_sd();

        void dump();
        
    private:
        bool parse_config(const String& config);
        bool parse_config_line(const String& config_line);
    
    private:
        HashType<String, String> _hash_array[CONFIG_SIZE];
        HashMap<String, String> _config_map;
    };
}

#endif
