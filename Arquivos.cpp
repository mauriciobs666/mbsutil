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

#include "Arquivos.h"

#ifdef _WIN32
#include <windows.h>

using namespace std;

list<string> Path::find(const string &nome, int restricoes)
{
	WIN32_FIND_DATA wfd;
	list<string> temp;

	HANDLE hnd=FindFirstFile(nome.c_str(), &wfd);
	if(INVALID_HANDLE_VALUE!=hnd)
	{

		do
		{
			string str=string(wfd.cFileName);
			if((str!=".")&(str!=".."))
				temp.push_back(str);
		} while(FindNextFile(hnd,&wfd));
		FindClose(hnd);
	}
	return temp;
}

string Path::relativo()
{
    return relativo(path);
}

string Path::relativo(const string &nome)
{
	return "TO-DO...";
/*
    char *tmp=nome;
    char *barra=NULL;

    while(*tmp!=0)
    {
        if(*tmp=='\\')
            barra=tmp;
        ++tmp;
    }
    if(barra==NULL)
        return nome;
    return ++barra;
*/
}
#endif
