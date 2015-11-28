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
#include <RTCZero.h>
#include "es_ntp.h"

// https://www.arduino.cc/en/Tutorial/SimpleRTC

namespace energonsoftware
{
    bool Ntp::send_packet(UdpWrapper& udp, const String& host)
    {
        static byte buffer[NtpPacketSize];
        memset(buffer, 0, NtpPacketSize);
        buffer[0] = 0b11100011;     // LI, Version, Mode
        buffer[1] = 0;              // Stratum, or type of clock
        buffer[2] = 6;              // Polling Interval
        buffer[3] = 0xEC;           // Peer Clock Precision
        // 8 bytes of zero for Root Delay & Root Dispersion
        buffer[12]  = 49;
        buffer[13]  = 0x4E;
        buffer[14]  = 49;
        buffer[15]  = 52;

        if(!udp.network().send_udp_packet(host, NtpPort, buffer, NtpPacketSize)) {
            return false;
        }

        return true;
    }

    bool Ntp::set_rtc(INetwork& network, uint16_t local_port, const String& host)
    {
        RTCZero rtc;
        rtc.begin();

        UdpWrapper udp(network, local_port);
        if(!udp.is_valid()) {
            return false;
        }

        if(!send_packet(udp, host)) {
            return false;
        }

        return true;
    }
}
