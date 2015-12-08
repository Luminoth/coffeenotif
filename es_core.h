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

#if !defined __ES_CORE_H__
#define __ES_CORE_H__

#include <Stream.h>

namespace energonsoftware
{
    void safe_exit(int led_pin=-1);

    void init_serial(unsigned long baud_rate);

    bool init_sd(uint32_t select_pin);

    bool poll_timeout(Stream& stream, uint32_t timeout_ms);

    String uri_encode(const char* const uri);
    
    String json_escape(const char* const value);
}

#endif
