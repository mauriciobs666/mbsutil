/*
	MBS-Util - General purpose C++ library
	Copyright (C) 2009 - Mauricio Bieze Stefani

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef SERIALPORT_H
#define SERIALPORT_H

#ifdef _WIN32
    #include <windows.h>
#endif

#include <stdio.h>

class SerialPort
{
	public:
		SerialPort();
		virtual ~SerialPort();
		int init(const char *port, int baud=9600, char byteSize=8, char parity=0, char stopBits=0);
		int Write(const char *data, size_t size);
		int Read(char *data, size_t maxSize);
		int closePort();
	protected:
        #ifdef _WIN32
            HANDLE portHandle;
        #else
            int fd;
        #endif
	private:
};

#endif // SERIALPORT_H
