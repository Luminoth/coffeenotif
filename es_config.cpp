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
#include "es_config.h"

namespace energonsoftware
{
    const char Config::CONFIG_FILE[] = "config.cfg";
    
    bool Config::read_from_sd()
    {
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
// TODO
        return true;
    }
}
