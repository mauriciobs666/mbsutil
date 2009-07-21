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
								FILE_FLAG_OVERLAPPED,// we want overlapped operation
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

    return 0;
}

int SerialPort::write()
{
	//WriteFile (portHandle,data,dwSize,&dwBytesWritten ,&ov);
}

int SerialPort::read()
{
	//abRet = ::ReadFile(m_hCommPort,szTmp ,sizeof(szTmp ), &dwBytesRead,&ovRead) ;
}

int SerialPort::closePort()
{
	if (INVALID_HANDLE_VALUE != portHandle)
	{
		CloseHandle(portHandle);
		portHandle = INVALID_HANDLE_VALUE;
	}
}
