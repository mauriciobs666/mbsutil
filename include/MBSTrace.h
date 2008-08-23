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

#define NUMBER_TRACE_LEVELS 3
#define TRACE_LEVEL_INFO 2
#define TRACE_LEVEL_WARN 1
#define TRACE_LEVEL_ERROR 0

#define TRACE_INFO(fmt,...)		MBSTrace::getInstance().print(TRACE_LEVEL_INFO,__FILE__,__LINE__,fmt,##__VA_ARGS__)
#define TRACE_WARN(fmt,...)		MBSTrace::getInstance().print(TRACE_LEVEL_WARN,__FILE__,__LINE__,fmt,##__VA_ARGS__)
#define TRACE_ERROR(fmt,...)	MBSTrace::getInstance().print(TRACE_LEVEL_ERROR,__FILE__,__LINE__,fmt,##__VA_ARGS__)

class MBSTrace
{
	public:
		struct structTraceLevelOpt
		{
			bool printFileLine;
			bool printFilePathLine;
			char printLevelPreffix[10];
		} TraceLevelOpt[NUMBER_TRACE_LEVELS];

		virtual ~MBSTrace();

		// singleton
		static MBSTrace& getInstance()
		{
		    static MBSTrace *instance=NULL;
		    if(!instance)
                instance=new MBSTrace();
            return *instance;
		}

		// print functions
		void print(int level, char *mesg) { print(level,NULL,0,mesg); }
		void print(int level, char *filename, int line, char *mesg, ...);

		int setFileName(char *basename, char *extension=NULL);

		int setFileOptions(	bool openForAppend=false,	//overwrite/append existing file
							int timeLimitMinutes=-1,	//start new file after x minutes
							int sizeLimitMB=-1,			//start new file if grow bigger than x megs
							bool timestampSuffix=false,	//sprintf(logfile,"%sYYYYMMDDHHMMSS.%s", basename, extension)
							bool sequenceSuffix=false );//sprintf(logfile,"%sNNNNNNNNNNNNNN.%s", basename, extension)

		int setFileHeader(char *header);	// printed at the beggining of the log file

		int openFile();		// manually start a logfile
		int closeFile();	// manually stop the logfile

	protected:
        FILE *file;
        char *basename;
        char *extension;
		bool openForAppend;
		int timeLimitMinutes;
		int sizeLimitMB;
		bool timestampSuffix;
		bool sequenceSuffix;
		char *header;

        MBSTrace();
};

#endif // MBSTRACE_H
