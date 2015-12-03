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
        const String& get_wifi_ssid() const { return _wifi_ssid; }
        void set_wifi_ssid(const String& wifi_ssid) { _wifi_ssid = wifi_ssid; }

    public:
        // NOTE: this assumes the SD card is available
        bool read_from_sd();
        
    private:
        bool parse_config(const String& config);
    
    private:
        String _wifi_ssid;
        String _wifi_key;
        int _wifi_key_index;
        String _wifi_password;
    };
}

#endif
