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
#include <SD.h>
#include "es_core.h"

namespace energonsoftware
{
    void safe_exit(int led_pin)
    {
        if(led_pin > 0) {
            analogWrite(led_pin, 255);
        }

        // TODO: can we call exit() instead of hard-locking the controller?
        while(true);
    }

    void init_serial(unsigned long baud_rate)
    {
        Serial.begin(baud_rate);

        // wait for serial port to connect
        // (needed for native USB port only?)
        while(!Serial);
    }

    bool init_sd(uint32_t select_pin)
    {
        return SD.begin(select_pin);
    }

    bool poll_timeout(Stream& stream, uint32_t timeout_ms)
    {
        Serial.print("Polling stream for ");
        Serial.print(timeout_ms);
        Serial.println("ms...");

        uint32_t start_ms = millis();
        while(stream.available() < 1) {
            if(millis() >= start_ms + timeout_ms) {
                Serial.println("Stream timeout!");
                return false;
            }
            delay(1);
        }

        return true;
    }

    // http://www.codeguru.com/cpp/cpp/algorithms/strings/article.php/c12759/URI-Encoding-and-Decoding.htm
    String uri_encode(const char* const uri)
    {
        static const char DEC2HEX[16 + 1] = "0123456789ABCDEF";

        String result;

        const char* current = uri;
        while(NULL != *current) {
            char ch = *current;
            if(isAlphaNumeric(ch)) {
                result += ch;
            } else {
                // escape this char
                result += '%';
                result += DEC2HEX[ch >> 4];
                result += DEC2HEX[ch & 0x0F];
            }
            ++current;
        }

        return result;
    }
}
