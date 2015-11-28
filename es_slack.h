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

#ifndef _ES_SLACK_H__
#define _ES_SLACK_H__

namespace energonsoftware
{
    class Slack
    {
    public:
        Slack();
        virtual ~Slack() throw() { }

    public:
        void set_api_token(const String& api_token) { _api_token = api_token; }
        const String& get_api_token() const { return _api_token; }

    private:
        String _api_token;
    };
}

#endif
