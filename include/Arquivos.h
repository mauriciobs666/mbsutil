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

#ifndef MBSUTIL_ARQUIVOS_H
#define MBSUTIL_ARQUIVOS_H

#include <string>
#include <list>

class Path
{
public:
	std::string path;

	Path() : path("")
		{}
	Path(const std::string &p) : path(p)
		{}

	std::string relativo();
	std::string relativo(const std::string &nome);

	static std::list<std::string> find(const std::string &nome, int restricoes=0);
};

#endif
