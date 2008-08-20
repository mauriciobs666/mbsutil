/*
	MBS-Util - General purpose C++ library
	Copyright (C) 2007 - Mauricio Bieze Stefani

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

#ifndef MBSTRACE_H
#define MBSTRACE_H

#include <stdio.h>
#include <stdarg.h>

#define TRACE_LEVEL_INFO 3
#define TRACE_LEVEL_WARN 2
#define TRACE_LEVEL_ERROR 1

#define TRACE_INFO(fmt,...)		MBSTrace::getInstance().print(TRACE_LEVEL_INFO,__FILE__,__LINE__,fmt,##__VA_ARGS__)
#define TRACE_WARN(fmt,...)		MBSTrace::getInstance().print(TRACE_LEVEL_WARN,__FILE__,__LINE__,fmt,##__VA_ARGS__)
#define TRACE_ERROR(fmt,...)	MBSTrace::getInstance().print(TRACE_LEVEL_ERROR,__FILE__,__LINE__,fmt,##__VA_ARGS__)

class MBSTrace
{
	public:
		virtual ~MBSTrace();
		static MBSTrace& getInstance()
		{
		    static MBSTrace *instance=NULL;
		    if(!instance)
                instance=new MBSTrace();
            return *instance;
		}
		void print(int level, char *mesg) { print(level,NULL,0,mesg); }
		void print(int level, char *filename, int line, char *mesg, ...);
	protected:
        FILE *file;
        MBSTrace();
};

#endif // MBSTRACE_H
