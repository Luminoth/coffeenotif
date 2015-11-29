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
#include "es_network.h"
#include "es_slack.h"

namespace energonsoftware
{
    String Slack::build_message(const String& api_token, const String& channel, const String& username, const String& message)
    {
        return String("{ ")
            + "\"token\": \"" + api_token + "\", "
            + "\"channel\": \"" + channel + "\", "
            + "\"text\": \"" + message + "\", "
            + "\"username\": \"" + username + "\""
            + " }";
    }

    Slack::Slack()
        : _api_token()
    {
    }

    bool Slack::connect(INetwork& network)
    {
        Serial.println("Connecting to Slack API...");
        return network.connect_server_ssl("slack.com", 80);
    }

    void Slack::disconnect(INetwork& network)
    {
        Serial.println("Disconnecting from Slack API...");
        network.disconnect_server();
    }

    void Slack::send_message(INetwork& network, const String& channel, const String& message)
    {
        String api_message = build_message(_api_token, channel, _username, message);
        Serial.println("Sending message to Slack API: " + api_message);

        network.println("GET /api/chat.postMessage HTTP/1.1");
        network.println("Host: slack.com");
        network.println("User-Agent: ArduinoWiFi/1.1");
        network.println("Connection: close");
        network.println();
        network.println(api_message);

// http://blog.devpost.com/post/93328687487/hacking-the-office-arduinos-extensions-and
// https://api.slack.com/
// https://api.slack.com/getting-started
// https://api.slack.com/bot-users
// https://api.slack.com/incoming-webhooks
// https://api.slack.com/rtm
// https://slack.com/api/chat.postMessage?channel=%23general&text=test&username=derp_bot&pretty=1 

// wat?

        Serial.println("Waiting for response...");
        uint32_t start_ms = millis();
        while(network.available() < 1) {
            uint32_t current_ms = millis();
            if(current_ms >= start_ms + TimeoutMs) {
                Serial.println("Slack API response timeout!");
                return;
            }
        }

        network.println("Slack API response: " + network.read_string());
    }
}
