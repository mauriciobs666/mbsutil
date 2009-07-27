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

#include <windows.h>

class SerialPort
{
	public:
		SerialPort();
		virtual ~SerialPort();
		int init(char *port, int baud, char byteSize, char parity, char stopBits);
		int write(char *data, int size);
		int read(char *data, int maxSize);
		int closePort();
	protected:
		HANDLE portHandle;
	private:
};

#endif // SERIALPORT_H
