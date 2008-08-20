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

MBSTrace::MBSTrace()
{
    file=NULL;
}

MBSTrace::~MBSTrace()
{
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

/*
int     tracePrint( char *format, ... )
{
   va_list parameter;
   va_start( parameter, format );

   if( TrInternal.fd != (FILE *) 0)
   {
      if( format != (char *) 0)
      {
         ret = vfprintf( TrInternal.fd, format, parameter );
         fflush( TrInternal.fd );
      }
   }
   va_end(parameter);
   return( ret );
}
*/
