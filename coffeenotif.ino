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

//// COFFEE SETTINGS
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

bool g_has_sd_card = false;

energonsoftware::Config g_config;
energonsoftware::WiFi g_wifi;
energonsoftware::Slack g_slack;
WiFiServer g_http_server(80);
WiFiSSLClient g_slack_client;
WiFiUDP g_ntp_client;

RTCZero rtc;
unsigned long g_last_ntp_update_ms = 0;

int g_last_button_state = LOW;

unsigned long g_last_coffee_start_ms = 0;

void update_rtc()
{
    uint32_t current_ms = millis();
    if(g_last_ntp_update_ms > 0 && g_last_ntp_update_ms + g_config.get_ntp_update_rate_ms() >= current_ms) {
        return;
    }

    static energonsoftware::Ntp ntp;
    ntp.set_rtc(rtc, g_ntp_client, g_config.get_local_ntp_port(), g_config.get_ntp_host().c_str());
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

    g_slack.start(g_slack_client);
    g_slack.send_message(g_slack_client, g_config.get_slack_channel().c_str(), g_wifi.get_local_ip_address_str().c_str());

    analogWrite(SLACK_LED_PIN, 0);
}

const char* random_coffee_notification(bool finished)
{
    return finished ? COFFEE_FINISHED_NOTIFS[random(COFFEE_FINISHED_NOTIF_COUNT)] : COFFEE_STARTED_NOTIFS[random(COFFEE_STARTED_NOTIF_COUNT)];
}

void notify_slack_channel(bool finished)
{
    analogWrite(SLACK_LED_PIN, 255);

    g_slack.send_message(g_slack_client, g_config.get_slack_channel().c_str(), random_coffee_notification(finished));

    analogWrite(SLACK_LED_PIN, 0);
}

void start_coffee_brewing()
{
    uint32_t current_ms = millis();
    if(0 != g_last_coffee_start_ms && current_ms < g_last_coffee_start_ms + g_config.get_coffee_brew_ms()) {
        return;
    }

    g_last_coffee_start_ms = current_ms;
    analogWrite(BREWING_LED_PIN, 255);
    notify_slack_channel(false);
}

void update_coffee_brewing()
{
    uint32_t current_ms = millis();
    if(0 == g_last_coffee_start_ms || current_ms < g_last_coffee_start_ms + g_config.get_coffee_brew_ms()) {
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

    while(client.available()) {
        client.readString();
    }
    
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: application/json");
    client.println("Connection: close");
    client.println();
    client.print("{");
// TODO: write diagnostic stuff here
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

    g_has_sd_card = energonsoftware::init_sd(CHIP_SELECT_PIN);
    Serial.print("Have SD card? ");
    Serial.println(g_has_sd_card ? "Yes" : "No");

    if(g_has_sd_card) {
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
