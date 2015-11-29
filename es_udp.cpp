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
#include "es_udp.h"

namespace energonsoftware
{
    UdpWrapper::UdpWrapper(UDP& udp, uint16_t local_port)
        : _udp(udp), _valid(false)
    {
        _valid = _udp.begin(local_port);
    }

    UdpWrapper::~UdpWrapper() throw()
    {
        _udp.stop();
    }

    bool UdpWrapper::send_packet(const String& host, uint16_t remote_port, const byte* const buffer, size_t buffer_len)
    {
        Serial.println("Sending UDP packet to " + host + ":" + remote_port + " (" + buffer_len + ")");

        if(_udp.beginPacket(host.c_str(), remote_port) < 1) {
            Serial.println("Failed to connect to UDP host!");
            return false;
        }

        _udp.write(buffer, buffer_len);

        if(_udp.endPacket() < 1) {
            Serial.println("Failed to send UDP packet!");
            return false;
        }

        return true;
    }
}
