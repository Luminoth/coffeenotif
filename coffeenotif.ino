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

#if defined ARDUINO_SAMD_ZERO
#include <RTCZero.h>
#endif

#include <SD.h>
#include <SPI.h>
#include <WiFi101.h>
#include <WiFiUdp.h>
#include "es_config.h"
#include "es_core.h"
#include "es_ntp.h"
#include "es_slack.h"
#include "es_wifi.h"

const char VERSION[] = "1.0";

//// PIN SETTINGS
// https://www.arduino.cc/en/Guide/ArduinoZero

// NOTE: all Zero pins are PWM except 2 and 7
// WiFi101 shield uses digital pins 5, 6 (the onboard button), 7 and should not be used
// pin 10 is slave select on the Zero
// pins 11, 12, and 13 are used for SPI

const uint32_t ERROR_LED_PIN = 3;       // red (error)
const uint32_t SLACK_LED_PIN = 4;       // blue (communicating with slack)
const uint32_t BREWING_LED_PIN = 8;     // green (brewing coffee)

const uint32_t INPUT_BUTTON_PIN = 2;

const uint32_t CHIP_SELECT_PIN = 10;
//// END PIN SETTINGS

const uint32_t SLEEP_TIME_MS = 1000;

energonsoftware::Config g_config;
#if defined ARDUINO_SAMD_ZERO
RTCZero g_rtc;
#endif
energonsoftware::WiFi g_wifi;
energonsoftware::Slack g_slack;
WiFiServer g_http_server(80);
WiFiSSLClient g_slack_client;

volatile bool g_button_pushed = false;

uint32_t g_coffee_brew_count = 0;
unsigned long g_last_coffee_start_ms = 0;

void update_rtc()
{
    static WiFiUDP ntp_client;
    static unsigned long last_update_ms = 0;
    
    unsigned long current_ms = millis();
    if(last_update_ms > 0 && last_update_ms + g_config.get_ntp_update_rate_ms() >= current_ms) {
        return;
    }

    static energonsoftware::Ntp ntp;
#if defined ARDUINO_SAMD_ZERO
    ntp.set_rtc(g_rtc, ntp_client, g_config.get_local_ntp_port(), g_config.get_ntp_host().c_str());
#endif
    last_update_ms = current_ms;
}

void on_button_released()
{
    g_button_pushed = true;
}

void start_slack()
{
    Serial.println("Starting Slack RTM session...");
    
    const String message("Listening at http://" + g_wifi.get_local_ip_address_str() + "/");

    analogWrite(SLACK_LED_PIN, 255);

    g_slack.start(g_slack_client);
    g_slack.send_message(g_slack_client, g_config.get_slack_channel().c_str(), message.c_str());

    analogWrite(SLACK_LED_PIN, 0);
}

void notify_slack_channel(bool finished)
{
    analogWrite(SLACK_LED_PIN, 255);

    g_slack.send_message(g_slack_client, g_config.get_slack_channel().c_str(), g_config.get_random_coffee_notification(finished).c_str());

    analogWrite(SLACK_LED_PIN, 0);
}

void start_coffee_brewing()
{
    unsigned long current_ms = millis();
    if(0 != g_last_coffee_start_ms && current_ms < g_last_coffee_start_ms + g_config.get_coffee_brew_ms()) {
        Serial.println("Coffee brew in progress, denying new start request!");
        return;
    }
    
    Serial.println("Coffee is brewing!");
    ++g_coffee_brew_count;

    g_last_coffee_start_ms = current_ms;
    analogWrite(BREWING_LED_PIN, 255);
    notify_slack_channel(false);
}

void update_coffee_brewing()
{
    unsigned long current_ms = millis();
    if(0 == g_last_coffee_start_ms || current_ms < g_last_coffee_start_ms + g_config.get_coffee_brew_ms()) {
        return;
    }
    
    Serial.println("Coffee is done!");

    g_last_coffee_start_ms = 0;
    analogWrite(BREWING_LED_PIN, 0);
    notify_slack_channel(true);
}

void print_json_value(Stream& stream, const String& key, const String& value)
{
    stream.print("\"" + key + "\": \"" + energonsoftware::json_escape(value.c_str()) + "\"");
}

void print_json_value(Stream& stream, const String& key, unsigned long value)
{
    stream.print("\"" + key + "\": " + value);
}

void http_listen()
{
    const int timeout_ms = 5000;
    
    auto client = g_http_server.available();
    if(!client) {
        return;
    }
    
    Serial.println("New HTTP connection, reading request...");

    unsigned long start_ms = millis();
    while(client.available() && start_ms + timeout_ms < millis()) {
        client.readString();
    }
    
    if(start_ms + timeout_ms < millis()) {
        Serial.println("HTTP read timeout!");
        return;
    }
    
    Serial.println("Sending HTTP response...");
    
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: application/json");
    client.println("Connection: close");
    client.println();
    client.print("{");
    print_json_value(client, "coffee_brew_count", g_coffee_brew_count); client.print(",");
    print_json_value(client, "last_coffee_start_ms", g_last_coffee_start_ms); client.print(",");
    print_json_value(client, "last_slack_response", g_slack.get_last_response());
    client.println("}");
    client.flush();

    client.stop();    
}

void setup()
{
    energonsoftware::init_serial(9600);

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
    pinMode(CHIP_SELECT_PIN, OUTPUT);
    
#if defined ARDUINO_SAMD_ZERO
    Serial.println("Initializing RTC...");
    g_rtc.begin();
#endif

    bool have_sd_card = energonsoftware::init_sd(CHIP_SELECT_PIN);
    Serial.print("Have SD card? ");
    Serial.println(have_sd_card ? "Yes" : "No");

    if(have_sd_card) {
        g_config.read_from_sd();
    }

    g_wifi.set_encryption_type(g_config.get_wifi_encryption_type());
    g_wifi.set_ssid(g_config.get_wifi_ssid());
    // TODO: WEP values
    g_wifi.set_wpa_password(g_config.get_wifi_wpa_password());

    g_wifi.set_use_dhcp(g_config.get_use_dhcp());
    if(!g_config.get_use_dhcp()) {
        //g_wifi.set_ip_address(g_config.get_static_ip_address());
    }

    if(!g_wifi.init()) {
        energonsoftware::safe_exit(ERROR_LED_PIN);
        return;
    }

    g_wifi.print_shield_info();

    Serial.println("Initializing Slack...");
    g_slack.set_api_token(g_config.get_slack_api_token());
    g_slack.set_username(g_config.get_slack_username());
    
    Serial.println("Attaching interrupts...");
#if defined ARDUINO_SAMD_ZERO
    int button_interrupt = INPUT_BUTTON_PIN;
#else
    int button_interrupt = digitalPinToInterrupt(INPUT_BUTTON_PIN);
#endif
    attachInterrupt(button_interrupt, on_button_released, FALLING);
    
    Serial.println("Ready!");
}

void loop()
{
    bool connected = g_wifi.connect(ERROR_LED_PIN);

    update_rtc();

    if(connected) {
        g_http_server.begin();
        start_slack();
    }

    if(g_button_pushed) {
        g_button_pushed = false;
        start_coffee_brewing();
    }
    update_coffee_brewing();

    http_listen();
    
    delay(SLEEP_TIME_MS);
}
