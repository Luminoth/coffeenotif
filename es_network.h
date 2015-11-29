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

#if !defined __ES_NETWORK_H__
#define __ES_NETWORK_H__

namespace energonsoftware
{
    class INetwork
    {
    public:
        INetwork() { }
        virtual ~INetwork() throw() { }

    public:
        virtual bool connect_server(const String& host, uint16_t port) = 0;
        virtual bool connect_server_ssl(const String& host, uint16_t port) = 0;

        virtual void disconnect_server() = 0;

        virtual void println(const String& message="") = 0;

        virtual int available() = 0;

        virtual String read_string() = 0;

    public:
        virtual bool begin_udp(uint16_t local_port) = 0;
        virtual void end_udp() = 0;

        virtual bool send_udp_packet(const String& host, uint16_t port, const byte* const buffer, size_t buffer_len) = 0;

        virtual int udp_available() = 0;
        virtual bool parse_udp_packet() = 0;
        virtual bool read_udp_packet(byte* const buffer, size_t buffer_len) = 0;
    };

    class UdpWrapper
    {
    public:
        UdpWrapper(INetwork& network, uint16_t local_port)
            : _network(network), _valid(false)
        {
            _valid = _network.begin_udp(local_port);
        }

        virtual ~UdpWrapper() throw()
        {
            _network.end_udp();
        }

    public:
        bool is_valid() const { return _valid; }

        INetwork& network() const { return _network; }

    private:
        INetwork& _network;
        bool _valid;

    private:
        UdpWrapper() = delete;
        UdpWrapper(const UdpWrapper&) = delete;
    };
}

#endif
