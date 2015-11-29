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
#include "es_core.h"
#include "es_slack.h"

namespace energonsoftware
{
// https://api.slack.com/
// https://api.slack.com/getting-started
// https://api.slack.com/bot-users
// https://api.slack.com/rtm

    const String Slack::SlackApiHost = String("slack.com");

    String Slack::build_start_rtm_uri(const String& api_token)
    {
        return String("/api/rtm.start")
            + "?token=" + energonsoftware::uri_encode(api_token);
    }

    String Slack::build_post_message_uri(const String& api_token, const String& channel, const String& username, const String& message)
    {
        return String("/api/chat.postMessage")
            + "?token=" + energonsoftware::uri_encode(api_token)
            + "&channel=" + energonsoftware::uri_encode(channel)
            + "&text=" + energonsoftware::uri_encode(message)
            + "&username=" + energonsoftware::uri_encode(username)
            + "&parse=full";
    }

    void Slack::send_packet(Client& client, const String& uri)
    {
        Serial.println("Sending message to Slack API: " + uri);
        client.println("GET " + uri + " HTTP/1.1");
        client.println("Host: " + SlackApiHost);
        client.println("User-Agent: ArduinoWiFi/1.1");
        client.println("Connection: close");
        client.println();
    }

    void Slack::recv_response(Client& client)
    {
        if(!energonsoftware::poll_timeout(client, TimeoutMs)) {
            return;
        }

        Serial.println("Slack API response: " + client.readString());
    }

    Slack::Slack()
        : _api_token()
    {
    }

    bool Slack::connect(Client& client)
    {
        Serial.println("Connecting to Slack API (" + SlackApiHost + ":" + SlackApiPort + ")...");
        return client.connect(SlackApiHost.c_str(), SlackApiPort) > 0;
    }

    void Slack::disconnect(Client& client)
    {
        Serial.println("Disconnecting from Slack API...");
        client.stop();
    }

    void Slack::start(Client& client)
    {
        send_packet(client, build_start_rtm_uri(_api_token));
        recv_response(client);
    }

    void Slack::send_message(Client& client, const String& channel, const String& message)
    {
        send_packet(client, build_post_message_uri(_api_token, channel, _username, message));
        recv_response(client);
    }
}
