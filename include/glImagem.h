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

#ifndef GLIMAGEM_H
#define GLIMAGEM_H
#include <gl\gl.h>
#include "geometria.h"

class glCor
{
public:
	unsigned char r,g,b,a;
	glCor(unsigned char red=0, unsigned char green=0, unsigned char blue=0, unsigned char alpha=255)
		: r(red), g(green), b(blue), a(alpha) {}
	unsigned char *ubv()
		{ return &r; }
};

//! Textura
class glImagem
{
	public:
		GLsizei w;		//!<width: largura em pixels
		GLsizei h;		//!<height: altura em pixels
		GLenum format; 	//!<formato dos pixels (GL_RGB ou GL_RGBA)
		GLubyte *pixels;//!<sem comentarios ;-)

		GLuint glId;	//!<bind de textura
		int pitch;		//!<tamanho de cada linha em bytes

		glImagem();
		virtual ~glImagem();

		int desenha() const;
		int bind();		//!<cria um ID pra textura e carrega na mem de video

		int resize();	//!<redimensiona imagens pra tamanhos potencia de 2
		Vetor2D ratio;	//!<proporcao pra ser usada com glTexCoord2f
};

#endif // GLIMAGEM_H
