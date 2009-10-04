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

#include "SerialPort.h"
#include <iostream>

using namespace std;

#ifdef _WIN32
SerialPort::SerialPort()
{
	portHandle = INVALID_HANDLE_VALUE;
}

SerialPort::~SerialPort()
{
	closePort();
}

int SerialPort::init(char *port, int baud, char byteSize, char parity, char stopBits)
{
	char lastError[1024];

	closePort();

	portHandle = CreateFile(	port,
								GENERIC_READ|GENERIC_WRITE,//access ( read and write)
								0,    //(share) 0:cannot share the COM port
								0,    //security  (None)
								OPEN_EXISTING,// creation : open_existing
								0, //FILE_FLAG_OVERLAPPED,// we want overlapped operation
								0// no templates file for COM port...
							);

	if ( INVALID_HANDLE_VALUE == portHandle )
	{
		// ERROR_ACCESS_DENIED = already opened by other application
		// ERROR_FILE_NOT_FOUND = port that doesn't exist

		FormatMessage( 	FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
						NULL,
						GetLastError(),
						MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
						lastError,
						1024,
						NULL
					 );

		cout << "Failed to open " << port << ": " << lastError << endl;
		return -1;
	}

	DCB dcb = {0};
	dcb.DCBlength = sizeof(DCB);

	if ( ! GetCommState(portHandle, &dcb) )
	{
		FormatMessage( 	FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
						NULL,
						GetLastError(),
						MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
						lastError,
						1024,
						NULL
					 );

		cout << "Failed to Get Comm State Reason: " << lastError << endl;
		closePort();
		return -2;
	}

	dcb.BaudRate    = baud;
	dcb.ByteSize    = byteSize;
	dcb.Parity      = parity;
	dcb.StopBits    = stopBits;

	if ( ! SetCommState(portHandle,&dcb) )
	{
		FormatMessage( 	FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
						NULL,
						GetLastError(),
						MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
						lastError,
						1024,
						NULL
					 );

		cout << "Failed to Set Comm State Reason: " << lastError << endl;
		closePort();
		return -3;
	}

	// info bellow is from http://www.codeproject.com/KB/system/chaiyasit_t.aspx
	COMMTIMEOUTS comTimeOut;

	/*	ReadIntervalTimeout
		Specifies the maximum time, in milliseconds, allowed to elapse between the arrival of two characters
		on the communications line. During a ReadFile operation, the time period begins when the first character
		is received. If the interval between the arrival of any two characters exceeds this amount, the ReadFile
		operation is completed and any buffered data is returned. A value of zero indicates that interval
		time-outs are not used.
		A value of MAXDWORD, combined with zero values for both the ReadTotalTimeoutConstant and
		ReadTotalTimeoutMultiplier members, specifies that the read operation is to return immediately with the
		characters that have already been received, even if no characters have been received.
	*/
	comTimeOut.ReadIntervalTimeout = 3;

	/*	ReadTotalTimeoutMultiplier
		Specifies the multiplier, in milliseconds, used to calculate the total time-out period for read operations.
		For each read operation, this value is multiplied by the requested number of bytes to be read.
	*/
	comTimeOut.ReadTotalTimeoutMultiplier = 3;

	/*	ReadTotalTimeoutConstant
		Specifies the constant, in milliseconds, used to calculate the total time-out period for read operations.
		For each read operation, this value is added to the product of the ReadTotalTimeoutMultiplier member and
		the requested number of bytes.
		A value of zero for both the ReadTotalTimeoutMultiplier and ReadTotalTimeoutConstant members indicates
		that total time-outs are not used for read operations.
	*/
	comTimeOut.ReadTotalTimeoutConstant = 2;

	/*	WriteTotalTimeoutMultiplier
		Specifies the multiplier, in milliseconds, used to calculate the total time-out period for write operations.
		For each write operation, this value is multiplied by the number of bytes to be written.
	*/
	comTimeOut.WriteTotalTimeoutMultiplier = 3;

	/*	WriteTotalTimeoutConstant
		Specifies the constant, in milliseconds, used to calculate the total time-out period for write operations.
		For each write operation, this value is added to the product of the WriteTotalTimeoutMultiplier member and
		the number of bytes to be written.
		A value of zero for both the WriteTotalTimeoutMultiplier and WriteTotalTimeoutConstant members indicates
		that total time-outs are not used for write operations.
		Note: After the user has set the time-out of communication without any error, the serial port has opened
		already.
	*/
	comTimeOut.WriteTotalTimeoutConstant = 2;

	SetCommTimeouts(portHandle, &comTimeOut);

    return 0;
}

int SerialPort::write(const char *data, size_t size)
{
	DWORD written;
	if( INVALID_HANDLE_VALUE == portHandle )
		return -1;
	if( WriteFile (portHandle, data, size, &written, NULL) )
		return 0;
	return -2;
}

int SerialPort::read(char *data, size_t maxSize)
{
	DWORD read;
	if( ReadFile( portHandle, data, maxSize, &read, NULL ) )
		return read;
	return -1;
}

int SerialPort::closePort()
{
	if (INVALID_HANDLE_VALUE != portHandle)
	{
		CloseHandle(portHandle);
		portHandle = INVALID_HANDLE_VALUE;
	}
	return 0;
}
#endif
