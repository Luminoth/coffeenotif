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

// https://www.arduino.cc/en/Guide/ArduinoZero
// http://arduino.stackexchange.com/questions/13178/classes-and-objects-how-many-and-which-file-types-i-actually-need-to-use-them

// **** https://www.arduino.cc/en/Tutorial.Wifi101GoogleCalendar (great example!)
// https://www.arduino.cc/en/Tutorial/WiFiWebClient

#include <RTCZero.h>
#include <SPI.h>
#include <WiFi101.h>
#include <WiFiUdp.h>
#include "es_core.h"
#include "es_ntp.h"
#include "es_slack.h"
#include "es_wifi.h"

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
const String SLACK_API_TOKEN = "<YOUR-SLACK-API-TOKEN>";
const String SLACK_USERNAME = "<YOUR-SLACK-BOT-USERNAME>";
const String SLACK_CHANNEL = "#general";
//// END SLACK SETTINGS

//// NTP SETTINGS
const uint16_t LOCAL_NTP_PORT = 2123;
const String NTP_HOST("pool.ntp.org");
//// END NTP SETTINGS

//// PIN SETTINGS

// NOTE: all Zero pins are PWM except 2 and 7
// NOTE: WiFi101 shield uses digital pins 5, 6 (the onboard button), 7 and should not be used
// the docs say pins 11, 12, and 13 are used for SPI but they're safe to re-use, I guess?
// pin 10 is slave select (can this be re-used?)

const uint32_t ERROR_LED_PIN = 3;       // red
const uint32_t SLACK_LED_PIN = 4;       // blue

const uint32_t INPUT_BUTTON_PIN = 9;
//// END PIN SETTINGS

energonsoftware::WiFi g_wifi;
energonsoftware::Slack g_slack;
WiFiSSLClient g_slack_client;
WiFiUDP g_ntp_client;

RTCZero rtc;
const int NTP_UPDATE_RATE_MS = 1 * 60 * 60 * 1000;  // 1 hour updates
unsigned long g_last_ntp_update_ms = 0;

int g_last_button_state = LOW;

unsigned long g_last_coffee_start_ms = 0;

void update_rtc()
{
    unsigned long current_ms = millis();
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

void notify_slack_channel()
{
    analogWrite(SLACK_LED_PIN, 255);

    g_slack.connect(g_slack_client);
    g_slack.send_message(g_slack_client, SLACK_CHANNEL, "/giphy coffee brewing");
    g_slack.disconnect(g_slack_client);

    analogWrite(SLACK_LED_PIN, 0);
}

void setup()
{
    energonsoftware::init_serial(115200);

    Serial.println("Initializing I/O...");
    pinMode(ERROR_LED_PIN, OUTPUT);
    pinMode(SLACK_LED_PIN, OUTPUT);
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
    if(g_wifi.connect(ERROR_LED_PIN)) {
        start_slack();
    }

    update_rtc();

    if(poll_button_released()) {
        g_last_coffee_start_ms = millis();
        notify_slack_channel();
    }
}
