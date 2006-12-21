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
	~Buffer() { if(dados!=0) delete[] dados; }

	void reset();	//pntL=pntE=dados
	unsigned long pegaTamanho() const { return tamanho; }
	int mudaTamanho(unsigned long tam);
	unsigned long disponiveis() const { return (pntE>pntL)?(pntE-pntL):0; }
	unsigned long livres() const { return tamanho-ocupados(); }
	unsigned long ocupados() const { return pntE-dados; }
	unsigned long append(Buffer& b, unsigned long qtd=0);
		//anexa qtd bytes de b->pntL em pntE
		//se qtd==0, anexa disponiveis() bytes de b->pntL em pntE

	unsigned char readByte();
	unsigned long writeByte(unsigned char uc);
	unsigned short readShort();
	unsigned long writeShort(unsigned short us);
	unsigned long readLong();
	unsigned long writeLong(unsigned long ul);
protected:
	unsigned long tamanho;
};
#endif
