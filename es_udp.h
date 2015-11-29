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

#if !defined __ES_UDP_H__
#define __ES_UDP_H__

#include <UDP.h>

namespace energonsoftware
{
    class UdpWrapper
    {
    public:
        UdpWrapper(UDP& udp, uint16_t local_port);
        virtual ~UdpWrapper() throw();

    public:
        bool is_valid() const { return _valid; }

        UDP& udp() const { return _udp; }

    public:
        bool send_packet(const String& host, uint16_t remote_port, const byte* const buffer, size_t buffer_len);

    private:
        UDP& _udp;
        bool _valid;

    private:
        UdpWrapper() = delete;
        UdpWrapper(const UdpWrapper&) = delete;
    };
}

#endif
