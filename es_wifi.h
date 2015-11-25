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

// NOTE: WiFi101 shield uses digital pins 5, 6, 7 and should not be used
// the docs say pins 11, 12, and 13 are used for SPI but they're safe to re-use, I guess?
// and pin 10 is slave select

void print_wifi_firmware_version();
void print_wifi_mac_address();
void print_wifi_info();

void print_wifi_ip_address();
void print_wifi_signal_strength();
void print_wifi_connection_info();

bool init_wifi();
void connect_wifi();

#endif
