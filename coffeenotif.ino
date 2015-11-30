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

#include <RTCZero.h>
#include <SPI.h>
#include <WiFi101.h>
#include <WiFiUdp.h>
#include "es_core.h"
#include "es_ntp.h"
#include "es_slack.h"
#include "es_wifi.h"

const char VERSION[] = "1.0";

//// PIN SETTINGS
// https://www.arduino.cc/en/Guide/ArduinoZero

// NOTE: all Zero pins are PWM except 2 and 7
// NOTE: WiFi101 shield uses digital pins 5, 6 (the onboard button), 7 and should not be used
// the docs say pins 11, 12, and 13 are used for SPI but they're safe to re-use, I guess?
// pin 10 is slave select (can this be re-used?)

const uint32_t ERROR_LED_PIN = 3;       // red
const uint32_t SLACK_LED_PIN = 4;       // blue
const uint32_t BREWING_LED_PIN = 8;     // green

const uint32_t INPUT_BUTTON_PIN = 2;
//// END PIN SETTINGS

//// WIFI SETTINGS (WPA2 Enterprise not supported)
const wl_enc_type ENCRYPTION_TYPE = ENC_TYPE_NONE;
const char WIFI_SSID[] = "<YOUR-SSID>";
const char WIFI_KEY[] = "<YOUR-WEP-KEY>";
const int WIFI_KEY_INDEX = 0;
const char WIFI_PASSWORD[] = "<YOUR-WPA-PASSWORD>";

const bool USE_DHCP = true;
const IPAddress IP_ADDRESS(127, 0, 0, 1);
//// END WIFI SETTINGS

//// SLACK SETTINGS
const char SLACK_API_TOKEN[] = "<YOUR-SLACK-API-TOKEN>";
const char SLACK_USERNAME[] = "coffee_bot";
const char SLACK_CHANNEL[] = "#coffee";
//// END SLACK SETTINGS

//// COFFEE SETTINGS
const uint32_t COFFEE_BREW_MS = 6 * 60 * 1000;

const char* COFFEE_STARTED_NOTIFS[] = {
    "Coffee Started!",
    "Fresh Pot Incoming!"
};
const size_t COFFEE_STARTED_NOTIF_COUNT = sizeof(COFFEE_STARTED_NOTIFS) / sizeof(COFFEE_STARTED_NOTIFS[0]);

const char* COFFEE_FINISHED_NOTIFS[] = {
    "Coffee Finished!",
    "Fresh Pot Ready!"
};
const size_t COFFEE_FINISHED_NOTIF_COUNT = sizeof(COFFEE_FINISHED_NOTIFS) / sizeof(COFFEE_FINISHED_NOTIFS[0]);
//// END COFFEE SETTINGS

//// NTP SETTINGS
const uint16_t LOCAL_NTP_PORT = 2123;
const char NTP_HOST[] = "pool.ntp.org";
//// END NTP SETTINGS

// TODO: this stuff should be encapsulated in a State structure

energonsoftware::WiFi g_wifi;
energonsoftware::Slack g_slack;
WiFiServer g_http_server(80);
WiFiSSLClient g_slack_client;
WiFiUDP g_ntp_client;

RTCZero rtc;
const int NTP_UPDATE_RATE_MS = 1 * 60 * 60 * 1000;  // 1 hour updates
unsigned long g_last_ntp_update_ms = 0;

int g_last_button_state = LOW;

uint32_t g_last_coffee_start_ms = 0;

void update_rtc()
{
    uint32_t current_ms = millis();
    if(g_last_ntp_update_ms > 0 && g_last_ntp_update_ms + NTP_UPDATE_RATE_MS >= current_ms) {
        return;
    }

    static energonsoftware::Ntp ntp;
    ntp.set_rtc(rtc, g_ntp_client, LOCAL_NTP_PORT, NTP_HOST);
    g_last_ntp_update_ms = current_ms;
}

bool poll_button_released()
{
    bool button_released = false;

    int current_button_state = digitalRead(INPUT_BUTTON_PIN);
    if(LOW == current_button_state && HIGH == g_last_button_state) {
        button_released = true;
    }

    g_last_button_state = current_button_state;
    return button_released;
}

void start_slack()
{
    Serial.println("Starting Slack RTM session...");

    analogWrite(SLACK_LED_PIN, 255);

    g_slack.connect(g_slack_client);
    g_slack.start(g_slack_client);
    g_slack.disconnect(g_slack_client);

    analogWrite(SLACK_LED_PIN, 0);
}

const char* random_coffee_notification(bool finished)
{
    return finished ? COFFEE_FINISHED_NOTIFS[random(COFFEE_FINISHED_NOTIF_COUNT)] : COFFEE_STARTED_NOTIFS[random(COFFEE_STARTED_NOTIF_COUNT)];
}

void notify_slack_channel(bool finished)
{
    analogWrite(SLACK_LED_PIN, 255);

    g_slack.connect(g_slack_client);
    g_slack.send_message(g_slack_client, SLACK_CHANNEL, random_coffee_notification(finished));
    g_slack.disconnect(g_slack_client);

    analogWrite(SLACK_LED_PIN, 0);
}

void start_coffee_brewing()
{
    uint32_t current_ms = millis();
    if(0 != g_last_coffee_start_ms && current_ms < g_last_coffee_start_ms + COFFEE_BREW_MS) {
        return;
    }

    g_last_coffee_start_ms = current_ms;
    analogWrite(BREWING_LED_PIN, 255);
    notify_slack_channel(false);
}

void update_coffee_brewing()
{
    uint32_t current_ms = millis();
    if(0 == g_last_coffee_start_ms || current_ms < g_last_coffee_start_ms + COFFEE_BREW_MS) {
        return;
    }

    g_last_coffee_start_ms = 0;
    analogWrite(BREWING_LED_PIN, 0);
    notify_slack_channel(true);
}

void http_listen()
{
    auto client = g_http_server.available();
    if(!client) {
        return;
    }

    // TODO

    client.stop();    
}

void setup()
{
    energonsoftware::init_serial(115200);

    // if analog input pin 0 is unconnected, random analog
    // noise will cause the call to randomSeed() to generate
    // different seed numbers each time the sketch runs.
    // randomSeed() will then shuffle the random function.
    randomSeed(analogRead(0));

    Serial.print("Coffee notifier v");
    Serial.print(VERSION);
    Serial.println(" coming online...");

    Serial.println("Initializing I/O...");
    pinMode(ERROR_LED_PIN, OUTPUT);
    pinMode(SLACK_LED_PIN, OUTPUT);
    pinMode(BREWING_LED_PIN, OUTPUT);
    pinMode(INPUT_BUTTON_PIN, INPUT);

    g_wifi.set_encryption_type(ENCRYPTION_TYPE);
    g_wifi.set_ssid(WIFI_SSID);
    // TODO: WEP values
    g_wifi.set_wpa_password(WIFI_PASSWORD);

    g_wifi.set_use_dhcp(USE_DHCP);
    g_wifi.set_ip_address(IP_ADDRESS);

    if(!g_wifi.init()) {
        energonsoftware::safe_exit(ERROR_LED_PIN);
        return;
    }

    g_wifi.print_shield_info();

    Serial.println("Initializing Slack...");
    g_slack.set_api_token(SLACK_API_TOKEN);
    g_slack.set_username(SLACK_USERNAME);
}

void loop()
{
    bool connected = g_wifi.connect(ERROR_LED_PIN);

    update_rtc();

    if(connected) {
        g_http_server.begin();
        start_slack();
    }

    if(poll_button_released()) {
        start_coffee_brewing();
    }
    update_coffee_brewing();

    http_listen();
}
