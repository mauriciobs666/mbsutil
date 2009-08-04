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

#ifndef MBSUTIL_BUFFER_H
#define MBSUTIL_BUFFER_H

#include <stdio.h>

class MBSBuffer
{
public:
	unsigned char *dados;
	unsigned char *pntE,*pntL;

	MBSBuffer(size_t tam=0);
	virtual ~MBSBuffer() { if(dados) delete[] dados; }

	void reset();	//pntL=pntE=dados
	int mudaTamanho(size_t tam);
	inline size_t pegaTamanho() const { return tamanho; }
	inline size_t disponiveis() const { return (pntE>pntL)?(pntE-pntL):0; }
	inline size_t livres() const { return tamanho-(pntE-dados); }
	inline size_t ocupados() const { return pntE-dados; }
	unsigned long append(MBSBuffer& b, size_t qtd=0);
		//anexa qtd bytes de b->pntL em pntE
		//se qtd==0, anexa disponiveis() bytes de b->pntL em pntE

	int readByte(unsigned char *uc);
	int writeByte(unsigned char uc);
	int readShort(unsigned short *us);
	int writeShort(unsigned short us);
	int readLong(unsigned long *ul);
	int writeLong(unsigned long ul);
	int readString(char *dest, size_t max);
	int writeString(const char *orig, size_t max=0);
protected:
	size_t tamanho;
};

#endif
