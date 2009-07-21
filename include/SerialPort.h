#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <windows.h>

class SerialPort
{
	public:
		SerialPort();
		virtual ~SerialPort();
		int init(char *port, int baud, char byteSize, char parity, char stopBits);
		int write();
		int read();
		int closePort();
	protected:
		HANDLE portHandle;
	private:
};

#endif // SERIALPORT_H
