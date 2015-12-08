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

// https://api.slack.com/
// https://api.slack.com/getting-started
// https://api.slack.com/bot-users
// https://api.slack.com/rtm

// No slash commands: https://twitter.com/Luminoth82/status/671069517938827264
// possible giphy solution: https://github.com/Giphy/GiphyAPI

namespace energonsoftware
{
    const char Slack::SlackApiHost[] = "slack.com";

    Slack::Slack()
        : _api_token(), _username(), _last_response()
    {
    }

    bool Slack::connect(Client& client)
    {
        Serial.print("Connecting to Slack API (");
        Serial.print(SlackApiHost);
        Serial.print(":");
        Serial.print(SlackApiPort);
        Serial.println(")...");

        return client.connect(SlackApiHost, SlackApiPort) > 0;
    }

    void Slack::disconnect(Client& client)
    {
        Serial.println("Disconnecting from Slack API...");
        client.stop();
    }

    void Slack::start(Client& client)
    {
        connect(client);
        send_packet(client, build_start_rtm_uri().c_str());
        recv_response(client);
        disconnect(client);
    }

    void Slack::send_message(Client& client, const char* const channel, const char* const message)
    {
        connect(client);
        send_packet(client, build_post_message_uri(channel, message).c_str());
        recv_response(client);
        disconnect(client);
    }
    
    void Slack::send_packet(Client& client, const char* const uri)
    {
        Serial.print("Sending message to Slack API: ");
        Serial.println(uri);

        client.print("GET ");
        client.print(uri);
        client.println(" HTTP/1.1");

        client.print("Host: ");
        client.println(SlackApiHost);

        client.println("User-Agent: ArduinoWiFi/1.1");
        client.println("Connection: close");

        client.println();
    }

    void Slack::recv_response(Client& client)
    {
        if(!energonsoftware::poll_timeout(client, TimeoutMs)) {
            return;
        }

        _last_response = client.readString();
        Serial.println("Slack API response: " + _last_response);
        // TODO: should we try to parse anything out of this?
    }

    String Slack::build_start_rtm_uri() const
    {
        return String("/api/rtm.start")
            + "?token=" + energonsoftware::uri_encode(_api_token.c_str());
    }

    String Slack::build_post_message_uri(const char* const channel, const char* const message) const
    {
        return String("/api/chat.postMessage")
            + "?token=" + energonsoftware::uri_encode(_api_token.c_str())
            + "&channel=" + energonsoftware::uri_encode(channel)
            + "&text=" + energonsoftware::uri_encode(message)
            + "&username=" + energonsoftware::uri_encode(_username.c_str())
            + "&parse=full";
    }
}
