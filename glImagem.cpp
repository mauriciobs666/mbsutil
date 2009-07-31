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

#include "glImagem.h"
#include <cstring>

using namespace std;

#undef DEBUG

glImagem::glImagem()
{
	glId=0;
	pixels=NULL;
}

glImagem::~glImagem()
{
	if(pixels)
		delete[] pixels;
}

int glImagem::desenha() const
{
	if(pixels)
	{
		glDrawPixels(w,h,format,GL_UNSIGNED_BYTE,pixels);
		return 0;
	}
	return -1;
}

//	Carrega textura na placa de video
int glImagem::bind()
{
	if(NULL==pixels)
		return -1;

	if(glId==0)
	{
		glGenTextures(1,&glId);
		if(glId==0)
		{
			#ifdef DEBUG
				printf("Erro ao alocar nome pra textura\n");
			#endif
			return -1;
		}
		glBindTexture(GL_TEXTURE_2D, glId);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
   		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   		glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, pixels);
   		#ifdef DEBUG
			printf("Textura carregada\n");
		#endif
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, glId);
		#ifdef NEVER
			printf("Textura ativada\n");
		#endif
	}
	return 0;
}

int glImagem::resize()
{
	if(NULL==pixels)
		return -1;

	int novoX=4;
	int novoY=4;
	while(novoX<w) novoX*=2;	//procura menor potencia de 2
	while(novoY<h) novoY*=2;

	int novoPitch=novoX;
	novoPitch*=(format==GL_RGB)?3:4;

	ratio.x=(float)w/(float)novoX;
	ratio.y=(float)h/(float)novoY;

	GLubyte *p=new GLubyte[novoY*novoPitch];	//cria nova imagem

	GLubyte *dst=p;			//temporario pra copia
	GLubyte *org=pixels;	//temporario pra copia
	for(int y=0;y<h;y++)	//copia linha por linha
	{
		memcpy(dst,org,pitch);
		org+=pitch;
		dst+=novoPitch;
	}

	//atualiza vars ...
	delete[] pixels;
	pixels=p;
	w=novoX;
	h=novoY;
	pitch=novoPitch;

	#ifdef DEBUG
		printf("Resizada pra %d*%d,%d\n",w,h,pitch);
	#endif
	return 0;
}

int glImagem::setaPixel(unsigned short x, unsigned short y, glCor *c)
{
	GLubyte *ptmp;
	ptmp=pixels+(pitch*y)+(x*4);
	*ptmp=c->r;
	ptmp++;
	*ptmp=c->g;
	ptmp++;
	*ptmp=c->b;
	ptmp++;
	if(format==GL_RGBA)
		*ptmp=c->a;
	return 0;
}
#endif
