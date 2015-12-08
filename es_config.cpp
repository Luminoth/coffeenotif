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
#include <SD.h>
#include <WiFi101.h>
#include "es_config.h"

namespace energonsoftware
{
    // TODO: move these into the config
    const String COFFEE_STARTED_NOTIFS[] = {
        "Coffee Started!",
        "Fresh Pot Incoming!"
    };
    const size_t COFFEE_STARTED_NOTIF_COUNT = sizeof(COFFEE_STARTED_NOTIFS) / sizeof(COFFEE_STARTED_NOTIFS[0]);
    
    const String COFFEE_FINISHED_NOTIFS[] = {
        "Coffee Finished!",
        "Fresh Pot Ready!"
    };
    const size_t COFFEE_FINISHED_NOTIF_COUNT = sizeof(COFFEE_FINISHED_NOTIFS) / sizeof(COFFEE_FINISHED_NOTIFS[0]);
    
    const char Config::CONFIG_FILE[] = "config.cfg";
    
    Config::Config()
        : _wifi_encryption_type(ENC_TYPE_NONE), _wifi_ssid(), _wifi_wep_key(), _wifi_wep_key_index(0), _wifi_wpa_password(),
            _use_dhcp(true), _static_ip_address(),
            _local_ntp_port(2123), _ntp_host("pool.ntp.org"), _ntp_update_rate_ms(3600000),
            _slack_api_token(), _slack_username(), _slack_channel(),
            _coffee_brew_ms(60000)
    {
    }
    
    bool Config::read_from_sd()
    {
        Serial.print("Reading config from '");
        Serial.print(CONFIG_FILE);
        Serial.println("'...");
        
        File configFile = SD.open(CONFIG_FILE);
        if(!configFile) {
            Serial.print("Unable to open config file '");
            Serial.print(CONFIG_FILE);
            Serial.print("'!");
            return false;
        }
        
        String config;
        while(configFile.available()) {
            config += configFile.readString();
        }
        
        configFile.close();
        
        return parse_config(config);
    }
    
    bool Config::parse_config(const String& config)
    {
        for(int i=0; i<config.length(); ++i) {
            String current_line;
            while(i<config.length() && i != '\r' && i != '\n') {
                current_line += config[i++];
            }

            current_line.trim();
            if(!parse_config_line(current_line)) {
                return false;
            }
        }
        return true;
    }
    
    bool Config::parse_config_line(const String& config_line)
    {
        if(config_line.length() <= 0 || config_line[0] == '#') {
            return true;
        }
        
        // TODO
        
        return true;
    }
    
    const String& Config::get_random_coffee_notification(bool finished) const
    {
        return finished ? COFFEE_FINISHED_NOTIFS[random(COFFEE_FINISHED_NOTIF_COUNT)] : COFFEE_STARTED_NOTIFS[random(COFFEE_STARTED_NOTIF_COUNT)];
    }
}
