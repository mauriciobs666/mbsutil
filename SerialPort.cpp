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
		cout << "Failed to open COM port Reason: " << GetLastError() << endl;
		return -1;
	}

	DCB dcb = {0};
	dcb.DCBlength = sizeof(DCB);

	if ( ! GetCommState(portHandle, &dcb) )
	{
		cout << "Failed to Get Comm State Reason: " << GetLastError() << endl;
		closePort();
		return -2;
	}

	dcb.BaudRate    = baud;
	dcb.ByteSize    = byteSize;
	dcb.Parity      = parity;
	dcb.StopBits    = stopBits;

	if ( ! SetCommState(portHandle,&dcb) )
	{
		cout << "Failed to Set Comm State Reason: " << GetLastError() << endl;
		closePort();
		return -3;
	}

	COMMTIMEOUTS comTimeOut;

	// Specify time-out between charactor for receiving.
	comTimeOut.ReadIntervalTimeout = 3;

	// Specify value that is multiplied by the requested number of bytes to be read.
	comTimeOut.ReadTotalTimeoutMultiplier = 3;

	// Specify value is added to the product of the ReadTotalTimeoutMultiplier member
	comTimeOut.ReadTotalTimeoutConstant = 2;

	// Specify value that is multiplied by the requested number of bytes to be sent.
	comTimeOut.WriteTotalTimeoutMultiplier = 3;

	// Specify value is added to the product of the WriteTotalTimeoutMultiplier member
	comTimeOut.WriteTotalTimeoutConstant = 2;

	// set the time-out parameter into device control.
	SetCommTimeouts(portHandle, &comTimeOut);

    return 0;
}

int SerialPort::write(char *data, int size)
{
	DWORD written;
	if( INVALID_HANDLE_VALUE == portHandle )
		return -1;
	if( WriteFile (portHandle, data, size, &written, NULL) )
		return 0;
	return -2;
}

int SerialPort::read(char *data, int maxSize)
{
	DWORD read;
	ReadFile( portHandle, data, maxSize, &read, NULL );
	return read;
}

int SerialPort::closePort()
{
	if (INVALID_HANDLE_VALUE != portHandle)
	{
		CloseHandle(portHandle);
		portHandle = INVALID_HANDLE_VALUE;
	}
}
