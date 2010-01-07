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

#ifndef _WIN32
#include <stdio.h>    /* Standard input/output definitions */
#include <stdlib.h>
#include <stdint.h>   /* Standard types */
#include <string.h>   /* String function definitions */
#include <unistd.h>   /* UNIX standard function definitions */
#include <fcntl.h>    /* File control definitions */
#include <errno.h>    /* Error number definitions */
#include <termios.h>  /* POSIX terminal control definitions */
#include <sys/ioctl.h>
#endif

using namespace std;


SerialPort::SerialPort()
{
    #ifdef _WIN32
        portHandle = INVALID_HANDLE_VALUE;
    #else
        fd = -1;
    #endif
}


SerialPort::~SerialPort()
{
	closePort();
}

int SerialPort::init(const char *port, int baud, char byteSize, char parity, char stopBits)
{
	char lastError[1024];

	closePort();

    #ifdef _WIN32
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

    #else

        struct termios toptions;

        fd = open(port, O_RDWR | O_NOCTTY | O_NDELAY);
        if (fd == -1)  {
            perror("init_serialport: Unable to open port ");
            return -1;
        }

        if (tcgetattr(fd, &toptions) < 0) {
            perror("init_serialport: Couldn't get term attributes");
            return -1;
        }
        speed_t brate = baud; // let you override switch below if needed
        switch(baud) {
        case 4800:   brate=B4800;   break;
        case 9600:   brate=B9600;   break;
    #ifdef B14400
        case 14400:  brate=B14400;  break;
    #endif
        case 19200:  brate=B19200;  break;
    #ifdef B28800
        case 28800:  brate=B28800;  break;
    #endif
        case 38400:  brate=B38400;  break;
        case 57600:  brate=B57600;  break;
        case 115200: brate=B115200; break;
        }
        cfsetispeed(&toptions, brate);
        cfsetospeed(&toptions, brate);

        // 8N1
        toptions.c_cflag &= ~PARENB;
        toptions.c_cflag &= ~CSTOPB;
        toptions.c_cflag &= ~CSIZE;
        toptions.c_cflag |= CS8;
        // no flow control
        toptions.c_cflag &= ~CRTSCTS;

        toptions.c_cflag |= CREAD | CLOCAL;  // turn on READ & ignore ctrl lines
        toptions.c_iflag &= ~(IXON | IXOFF | IXANY); // turn off s/w flow ctrl

        toptions.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // make raw
        toptions.c_oflag &= ~OPOST; // make raw

        // see: http://unixwiz.net/techtips/termios-vmin-vtime.html
        toptions.c_cc[VMIN]  = 0;
        toptions.c_cc[VTIME] = 20;

        if( tcsetattr(fd, TCSANOW, &toptions) < 0) {
            perror("init_serialport: Couldn't set term attributes");
            return -1;
        }
    #endif
    return 0;
}

int SerialPort::Write(const char *data, size_t size)
{
    #ifdef _WIN32
        DWORD written;
        if( INVALID_HANDLE_VALUE == portHandle )
            return -1;
        if( WriteFile (portHandle, data, size, &written, NULL) )
            return 0;
    #else
        if( -1 == fd )
            return -1;
        if( write(fd, data, size) == size)
            return 0;
    #endif
	return -2;
}

int SerialPort::Read(char *data, size_t maxSize)
{
    #ifdef _WIN32
        DWORD read;
        if( ReadFile( portHandle, data, maxSize, &read, NULL ) )
            return read;
    #else
        int n = read(fd, data, maxSize);  // read a char at a time
        if( n >= 0 )
            return n;
    #endif
	return -1;
}

int SerialPort::closePort()
{
    #ifdef _WIN32
        if (INVALID_HANDLE_VALUE != portHandle)
        {
            CloseHandle(portHandle);
            portHandle = INVALID_HANDLE_VALUE;
        }
	#else
        if ( -1 != fd )
        {
            close(fd);
            fd=-1;
        }
	#endif
	return 0;
}

