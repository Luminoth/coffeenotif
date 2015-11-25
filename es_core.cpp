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

#include <arduino.h>
#include "es_core.h"

void safe_exit()
{
    // TODO: set a failure LED

    // TODO: can we call exit() instead of hard-locking the controller?
    while(true);
}

void init_serial()
{
    Serial.begin(9600);

    // wait for serial port to connect
    // (needed for native USB port only?)
    while(true) {
        if(Serial) {
            break;
        }
    }
}
