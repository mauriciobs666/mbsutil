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

class Buffer
{
public:
	unsigned char *dados;
	unsigned char *pntE,*pntL;

	Buffer(unsigned long tam=0);
	virtual ~Buffer() { if(dados) delete[] dados; }

	void reset();	//pntL=pntE=dados
	int mudaTamanho(unsigned long tam);
	inline unsigned long pegaTamanho() const { return tamanho; }
	inline unsigned long disponiveis() const { return (pntE>pntL)?(pntE-pntL):0; }
	inline unsigned long livres() const { return tamanho-(pntE-dados); }
	inline unsigned long ocupados() const { return pntE-dados; }
	unsigned long append(Buffer& b, unsigned long qtd=0);
		//anexa qtd bytes de b->pntL em pntE
		//se qtd==0, anexa disponiveis() bytes de b->pntL em pntE

	int readByte(unsigned char *uc);
	int writeByte(unsigned char uc);
	int readShort(unsigned short *us);
	int writeShort(unsigned short us);
	int readLong(unsigned long *ul);
	int writeLong(unsigned long ul);
	int readString(char *dest, unsigned long max);
	int writeString(const char *orig);
protected:
	unsigned long tamanho;
};

#endif
