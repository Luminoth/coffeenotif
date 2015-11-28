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

#include <RTCZero.h>
#include "es_network.h"

namespace energonsoftware
{
    class Ntp
    {
    private:
        static const int NtpPacketSize = 48;
        static const uint16_t NtpPort = 123;
        static const uint32_t TimeoutMs = 60 * 1000;

        // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
        static const unsigned long SeventyYears = 2208988800UL;

    private:
        static bool send_packet(UdpWrapper& udp, const String& host, byte* const buffer);
        static bool recv_packet(UdpWrapper& udp, byte* const buffer);

        static unsigned long parse_epoch(const byte* const buffer);

    public:
        Ntp() { }
        virtual ~Ntp() throw() { }

    public:
        bool set_rtc(RTCZero& rtc, INetwork& network, uint16_t local_port, const String& host);
    };
}

#endif
