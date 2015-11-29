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
        while(true) {
            if(Serial) {
                break;
            }
        }
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
        }

        return true;
    }

    // http://www.codeguru.com/cpp/cpp/algorithms/strings/article.php/c12759/URI-Encoding-and-Decoding.htm
    // Only alphanum is safe.
    const char SAFE[256] =
    {
        /*      0 1 2 3  4 5 6 7  8 9 A B  C D E F */
        /* 0 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
        /* 1 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
        /* 2 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
        /* 3 */ 1,1,1,1, 1,1,1,1, 1,1,0,0, 0,0,0,0,
    
        /* 4 */ 0,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
        /* 5 */ 1,1,1,1, 1,1,1,1, 1,1,1,0, 0,0,0,0,
        /* 6 */ 0,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
        /* 7 */ 1,1,1,1, 1,1,1,1, 1,1,1,0, 0,0,0,0,
    
        /* 8 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
        /* 9 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
        /* A */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
        /* B */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    
        /* C */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
        /* D */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
        /* E */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
        /* F */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0
    };

    String uri_encode(const String& uri)
    {
        static const char DEC2HEX[16 + 1] = "0123456789ABCDEF";

        String result;
        for(int i=0; i<uri.length(); ++i) {
            const char current = uri[i];
            if(SAFE[current]) {
                result += current;
            } else {
                // escape this char
                result += '%';
                result += DEC2HEX[current >> 4];
                result += DEC2HEX[current & 0x0F];
            }
        }
        return result;
    }
}
