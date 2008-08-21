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

#include "MBSTrace.h"
#include <string.h>

MBSTrace::MBSTrace()
{
	basename=extension=header=NULL;
	openForAppend=timestampSuffix=sequenceSuffix=false;
	timeLimitMinutes=sizeLimitMB=-1;
    file=NULL;
}

MBSTrace::~MBSTrace()
{
	if(basename)
		delete basename;
	if(extension)
		delete extension;
	if(header)
		delete header;
	if(file)
        fclose(file);
}

void MBSTrace::print(int level, char *filename, int line, char *mesg, ...)
{
    if(level==TRACE_LEVEL_INFO)
        printf("[INF] ");
    else if(level==TRACE_LEVEL_WARN)
        printf("[WRN] ");
    else if(level==TRACE_LEVEL_ERROR)
        printf("[ERR] ");

    if(filename)
    {
        //I dont want the full path, only the file name
        char *slash=filename;
        while(*slash!=0)
        {
            //find the last slash and ignore everything before it
            if((*slash=='\\')||(*slash=='/'))
                filename=slash+1;   //first char after the slash
            slash++;
        }
        printf("%s:%d ",filename,line);
    }

	if(mesg)
	{
		va_list l;
		va_start( l, mesg );
		vprintf( mesg, l );
		va_end(l);
	}

    printf("\n");
}

int MBSTrace::setFileName(char *basename, char *extension)
{
	if(basename==NULL)
		return -1;

	int sb=strlen(basename);
	this->basename=new char[sb+1];
	strncpy(this->basename,basename,sb);

	//extension is optional since its meant to be used with timestamp or sequence number
	if(extension!=NULL)
	{
		int se=strlen(extension);
		this->extension=new char[se+1];
		strncpy(this->extension,extension,sb);
	}

	return 0;
}

int MBSTrace::setFileOptions(bool openForAppend, int timeLimitMinutes, int sizeLimitMB, bool timestampSuffix, bool sequenceSuffix)
{
	this->openForAppend=openForAppend;
	this->timeLimitMinutes=timeLimitMinutes;
	this->sizeLimitMB=sizeLimitMB;
	this->timestampSuffix=timestampSuffix;
	this->sequenceSuffix=sequenceSuffix;
	return 0;
}

int MBSTrace::setFileHeader(char *header)
{
	return 0;
}

int MBSTrace::openFile()
{
	return 0;
}

int MBSTrace::closeFile()
{
	return 0;
}
