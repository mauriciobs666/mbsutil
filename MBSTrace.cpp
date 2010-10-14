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

    for(int x=0;x<NUMBER_TRACE_LEVELS;x++)
    {
		TraceLevelOpt[x].printFileLine=true;
		TraceLevelOpt[x].printFilePathLine=false;
		TraceLevelOpt[x].printLevelPreffix[0]=0;
    }
    strcpy(TraceLevelOpt[TRACE_LEVEL_INFO].printLevelPreffix,"[NFO]");
    strcpy(TraceLevelOpt[TRACE_LEVEL_WARN].printLevelPreffix,"[WRN]");
    strcpy(TraceLevelOpt[TRACE_LEVEL_ERROR].printLevelPreffix,"[ERR]");
}

MBSTrace::~MBSTrace()
{
	if(basename)
		delete basename;
	if(extension)
		delete extension;
	if(header)
		delete header;
	closeFile();
}

void MBSTrace::print(int level, const char *filename, int line, const char *mesg, ...)
{
	if((level<0)||(level>=NUMBER_TRACE_LEVELS))
		return;

	structTraceLevelOpt *levelOpt = &TraceLevelOpt[level];

	if(strlen(levelOpt->printLevelPreffix) > 0)
	{
		printf("%s ", levelOpt->printLevelPreffix);
		if(file)
			fprintf(file,"%s ", levelOpt->printLevelPreffix);
	}

	if(mesg)
	{
		va_list l;
		va_start( l, mesg );
		vprintf( mesg, l );
		if(file)
			vfprintf(file, mesg, l );
		va_end(l);
	}

	if(levelOpt->printFileLine)
	{
		if(filename)
		{
			if(!levelOpt->printFilePathLine)
			{
				//remove the full path and print only the file name
				const char *slash=filename;
				while(*slash!=0)
				{
					//find the last slash and ignore everything before it
					if((*slash=='\\')||(*slash=='/'))
						filename=slash+1;   //first char after the slash
					slash++;
				}
			}
			printf(" (%s:%d)",filename,line);
			if(file)
				fprintf(file," (%s:%d)",filename,line);
		}
	}

    printf("\n");
    if(file)
		fprintf(file,"\n");
}

int MBSTrace::setFileName(const char *basename, const char *extension)
{
	if(basename==NULL)
		return -1;

	int sb=strlen(basename)+1;
	this->basename=new char[sb];
	strncpy(this->basename,basename,sb);

	//extension is optional since its meant to be used with timestamp or sequence number
	if(extension!=NULL)
	{
		int se=strlen(extension)+1;
		this->extension=new char[se];
		strncpy(this->extension, extension, se);
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

int MBSTrace::setFileHeader(const char *header)
{
	if(this->header!=NULL)
		delete this->header;
	if(header==NULL)
		this->header=NULL;
	else
	{
		int sh=strlen(header);
		this->header=new char[sh+1];
		strncpy(this->header,header,sh);
	}
	return 0;
}

int MBSTrace::openFile()
{
	char filename[512];

	if(basename==NULL)
		return -1;

	if(extension==NULL)
		snprintf(filename, 512, "%s", basename);
	else
		snprintf(filename, 512, "%s%s", basename, extension);

	if(openForAppend)
		file=fopen(filename,"a+");
	else
		file=fopen(filename,"w+");

	if(file==NULL)
		return -2;

	return 0;
}

int MBSTrace::closeFile()
{
	if(file)
        fclose(file);
	return 0;
}
