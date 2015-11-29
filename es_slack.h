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

#if !defined __ES_SLACK_H__
#define __ES_SLACK_H__

#include <Client.h>

namespace energonsoftware
{
    class Slack
    {
    private:
        static const String SlackApiHost;
        static const uint16_t SlackApiPort = 443;
        static const uint32_t TimeoutMs = 60 * 1000;

    private:
        static String build_start_rtm_uri(const String& api_token);
        static String build_post_message_uri(const String& api_token, const String& channel, const String& username, const String& message);

        static void send_packet(Client& client, const String& uri);
        static void recv_response(Client& client);

    public:
        Slack();
        virtual ~Slack() throw() { }

    public:
        void set_api_token(const String& api_token) { _api_token = api_token; }
        const String& get_api_token() const { return _api_token; }

        void set_username(const String& username) { _username = username; }
        const String& get_username() const { return _username; }

    public:
        bool connect(Client& client);
        void disconnect(Client& client);

        void start(Client& client);
        void send_message(Client& client, const String& channel, const String& message);

    private:
        String _api_token;
        String _username;
    };
}

#endif
