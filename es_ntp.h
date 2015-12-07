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

#if !defined __ES_NTP_H__
#define __ES_NTP_H__

#if defined ARDUINO_SAMD_ZERO
#include <RTCZero.h>
#endif

#include "es_udp.h"

namespace energonsoftware
{
    class Ntp
    {
    private:
        static const int NtpPacketSize = 48;
        static const uint16_t NtpPort = 123;
        static const unsigned long TimeoutMs = 60000;
        static const unsigned long SeventyYearsSeconds = 2208988800UL;

    private:
        static bool send_packet(UdpWrapper& udp, const char* const host, byte* const buffer);
        static bool recv_packet(UDP& udp, byte* const buffer);

        static unsigned long parse_epoch(const byte* const buffer);

    public:
        Ntp() { }
        virtual ~Ntp() throw() { }

    public:
#if defined ARDUINO_SAMD_ZERO
        bool set_rtc(RTCZero& rtc, UDP& udp, uint16_t local_port, const char* const host);
#endif
    };
}

#endif
