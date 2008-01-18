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

#ifdef _WIN32

#ifndef GLFONTE_H
#define GLFONTE_H

#include <gl\gl.h>
#include <windows.h>

//!Carrega um arquivo de fonte

class GLFonte
{
public:
	int altura;	//!<altura da fonte carregada (pra espacamento)
	GLFonte()
		{ base=glGenLists(96); }
	virtual ~GLFonte()
		{ glDeleteLists(base, 96); }

	/** Carrega do arquivo
		\param hDC Handle pro contexto do dispositivo (Windows)
		\param nome Nome do arquivo de fonte a ser carregado
		\param size Tamanho fixo da fonte
	*/
	int load(HDC hDC, const char* nome, int size);

	//!similar ao printf da libc
	void printf(const char *fmt, ...);
private:
	GLuint	base;
};

#endif // GLFONTE_H
#endif
