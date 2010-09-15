/*
	MBS-Util - General purpose C++ library
	Copyright (C) 2007-2010 - Mauricio Bieze Stefani

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

#ifndef AVERAGE_HPP_INCLUDED
#define AVERAGE_HPP_INCLUDED

#ifndef NUM_READINGS
#define NUM_READINGS 10
#endif

class Average
{
public:
    Average()
    {
        for(int i=0; i < NUM_READINGS; i++)
            readings[i] = 0;
        total = 0;
        index = 0;
    }
    long read(long value)
    {
        total -= readings[index];       // subtract the last reading:
        readings[index] = value;
        total += readings[index];

		index++;
        if (index >= NUM_READINGS)
            index = 0;

        return getAverage();
    }
    long getAverage()
    {
        return total / NUM_READINGS;
    }
private:
    long readings[NUM_READINGS]; // the readings from the analog input
    int index;                  // the index of the current reading
    long total;                  // the running total
};

#endif // AVERAGE_HPP_INCLUDED
