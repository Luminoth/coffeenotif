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

#include <ctime>
#include <Arduino.h>
#include "es_ntp.h"

// https://www.arduino.cc/en/Tutorial/SimpleRTC

namespace energonsoftware
{
    bool Ntp::send_packet(UdpWrapper& udp, const String& host, byte* const buffer)
    {
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

    bool Ntp::recv_packet(UdpWrapper& udp, byte* const buffer)
    {
        uint32_t start_ms = millis();
        while(udp.network().udp_available() < 1) {
            uint32_t current_ms = millis();
            if(current_ms >= start_ms + TimeoutMs) {
                break;
            }
        }

        if(!udp.network().parse_udp_packet()) {
            Serial.println("NTP timeout!");
            return false;
        }

        if(!udp.network().read_udp_packet(buffer, NtpPacketSize)) {
            return false;
        }

        return true;
    }

    unsigned long Ntp::parse_epoch(const byte* const buffer)
    {
        // the timestamp starts at byte 40 of the received packet and is four bytes,
        // or two words, long. First, esxtract the two words:
        const unsigned long high_word = word(buffer[40], buffer[41]);
        const unsigned long low_word = word(buffer[42], buffer[43]);

        // combine the four bytes (two words) into a long integer
        // this is NTP time (seconds since Jan 1 1900):
        const unsigned long ntp_time = high_word << 16 | low_word;

        // now convert NTP time into everyday time.
        // Subtract seventy years:
        return ntp_time - SeventyYearsSeconds;
    }

    bool Ntp::set_rtc(RTCZero& rtc, INetwork& network, uint16_t local_port, const String& host)
    {
        Serial.println("Setting RTC from NTP server " + host + "...");

        UdpWrapper udp(network, local_port);
        if(!udp.is_valid()) {
            Serial.println("UDP listen error!");
            return false;
        }

        static byte buffer[NtpPacketSize];
        memset(buffer, 0, NtpPacketSize);

        if(!send_packet(udp, host, buffer)) {
            Serial.println("Error sending NTP packet!");
            return false;
        }

        if(!recv_packet(udp, buffer)) {
            Serial.println("Error receiving NTP packet!");
            return false;
        }

        const unsigned long epoch = parse_epoch(buffer);

        time_t rawtime = static_cast<time_t>(epoch);
        struct tm* timeinfo = localtime(&rawtime);

        Serial.print("NTP local time and date: ");
        Serial.println(asctime(timeinfo));

        rtc.setSeconds(timeinfo->tm_sec);
        rtc.setMinutes(timeinfo->tm_min);
        rtc.setHours(timeinfo->tm_hour);    // TODO: this probably needs a timezone offset

        rtc.setDay(timeinfo->tm_mday);
        rtc.setMonth(timeinfo->tm_mon + 1);
        rtc.setYear(timeinfo->tm_year - 100);

        return true;
    }
}
