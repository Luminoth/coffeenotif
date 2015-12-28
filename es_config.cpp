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

    const String Config::WIFI_ENCRYPTION_TYPE("WIFI_ENCRYPTION_TYPE");
    const String Config::WIFI_SSID("WIFI_SSID");
    const String Config::WIFI_WEP_KEY("WIFI_WEP_KEY");
    const String Config::WIFI_WEP_KEY_INDEX("WIFI_WEP_KEY_INDEX");
    const String Config::WIFI_WPA_PASSWORD("WIFI_WPA_PASSWORD");
        
    const String Config::USE_DHCP("USE_DHCP");
    const String Config::IP_ADDRESS("IP_ADDRESS");
        
    const String Config::LOCAL_NTP_PORT("LOCAL_NTP_PORT");
    const String Config::NTP_HOST("NTP_HOST");
    const String Config::NTP_UPDATE_RATE_MINUTES("NTP_UPDATE_RATE_MINUTES");

    const String Config::SLACK_API_TOKEN("SLACK_API_TOKEN");
    const String Config::SLACK_USERNAME("SLACK_USERNAME");
    const String Config::SLACK_CHANNEL("SLACK_CHANNEL");

    const String Config::COFFEE_BREW_MS("COFFEE_BREW_MS");
    
    Config::Config()
        : _hash_array(), _config_map(_hash_array, CONFIG_SIZE)
    {
    }

    void Config::init()
    {
        _config_map[0](WIFI_ENCRYPTION_TYPE, String(ENC_TYPE_NONE));
        _config_map[1](WIFI_SSID, "");
        _config_map[2](WIFI_WEP_KEY, "");
        _config_map[3](WIFI_WEP_KEY_INDEX, "0");
        _config_map[4](WIFI_WPA_PASSWORD, "");

        _config_map[5](USE_DHCP, "true");
        _config_map[6](IP_ADDRESS, "");

        _config_map[7](LOCAL_NTP_PORT, "2123");
        _config_map[8](NTP_HOST, "pool.ntp.org");
        _config_map[9](NTP_UPDATE_RATE_MINUTES, "60");

        _config_map[10](SLACK_API_TOKEN, "");
        _config_map[11](SLACK_USERNAME, "");
        _config_map[12](SLACK_CHANNEL, "");

        _config_map[13](COFFEE_BREW_MS, "60000");
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
            Serial.println("'!");
            return false;
        }
        
        String config;
        while(configFile.available()) {
            config += configFile.readString();
        }
        
        configFile.close();
        
        return parse_config(config);
    }

    void Config::dump()
    {
        Serial.println("Config:");
        for(size_t i=0; i<CONFIG_SIZE; ++i) {
            auto v = _config_map[i];
            Serial.print(v.getHash());
            Serial.print(" = ");
            Serial.println(v.getValue());
        }
    }
    
    bool Config::parse_config(const String& config)
    {
        for(size_t i=0; i<config.length(); ++i) {
            char ch = config[i];
            if(isspace(ch)) {
                continue;
            }

            String current_line;
            while('\r' != ch && '\n' != ch) {
                current_line += ch;
                if(++i >= config.length()) {
                    break;
                }
                ch = config[i];
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

        int idx = config_line.indexOf('=');
        if(idx < 0) {
            return false;
        }

        if(idx >= config_line.length() - 1) {
            return true;
        }

        String key = config_line.substring(0, idx);
        String value = config_line.substring(idx+1);

        byte keyIndex = _config_map.getIndexOf(key);
        /*Serial.print("Setting config value (");
        Serial.print(keyIndex);
        Serial.print("): ");
        Serial.print(key);
        Serial.print(" = ");
        Serial.println(value);*/
        _config_map[keyIndex](key, value);

        return true;
    }
    
    const String& Config::get_random_coffee_notification(bool finished) const
    {
        return finished ? COFFEE_FINISHED_NOTIFS[random(COFFEE_FINISHED_NOTIF_COUNT)] : COFFEE_STARTED_NOTIFS[random(COFFEE_STARTED_NOTIF_COUNT)];
    }
}
