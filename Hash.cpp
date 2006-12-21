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

#include "Hash.h"
#include "Parser.h"
#include <string>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>

using namespace std;

namespace {
	static bool inicializado;
	void iniciaRandom()
	{
		if(!inicializado)
		{
			srand(time(NULL));
			inicializado=true;
		}
	}
};

void Hash128::random()
{
	iniciaRandom();
    for(int x=0;x<16;x++)
		h.b[x]=rand();
}

string Hash128::toString() const
{
	char tmp[33];
	dump(tmp);
	return string(tmp);
}

int Hash128::fromString(const std::string& s)
{
	if(32!=s.size())
		return -1;
	for(int x=0;x<16;x++)
		h.b[x]=asc2hex(s[x*2],s[x*2+1]);
	return 0;
}

char* Hash128::dump(char *dest) const
{
    char *tmp=dest;
    for(int x=0;x<16;x++)
    {
        sprintf(tmp,"%02X",(unsigned char)h.b[x]);
        tmp+=2;
    }
    *tmp=0;
    return dest;
}

int Hash128::read(unsigned char *&pkt)
{
    memcpy(h.b,pkt,16);
    pkt+=16;
    return 0;
}

int Hash128::write(unsigned char *&pkt) const
{
    memcpy(pkt,h.b,16);
    pkt+=16;
    return 0;
}

ostream& operator<<(ostream& os, const Hash128& h)
{
	return os << h.toString();
}

istream& operator>>(istream& is, Hash128& h)
{
//TODO: operador >> Hash128
	return is;
}

//------------------------------------------------------------------------------
//      MD4 - implementado a partir da RFC 1320
//------------------------------------------------------------------------------

namespace
{
	const unsigned long I0=0x67452301;	/* Initial values for MD buffer */
	const unsigned long I1=0xefcdab89;
	const unsigned long I2=0x98badcfe;
	const unsigned long I3=0x10325476;

		/* C2 and C3 are from Knuth, The Art of Programming, Volume 2
		** (Seminumerical Algorithms), Second Edition (1981), Addison-Wesley.
		** Table 2, page 660.
		*/
	const unsigned long C2=013240474631;/* round 2 constant = sqrt(2) in octal */
	const unsigned long C3=015666365641;/* round 3 constant = sqrt(3) in octal */

	const unsigned long fs1=3;			/* round 1 shift amounts */
	const unsigned long fs2=7;
	const unsigned long fs3=11;
	const unsigned long fs4=19;
	const unsigned long gs1=3;			/* round 2 shift amounts */
	const unsigned long gs2=5;
	const unsigned long gs3=9;
	const unsigned long gs4=13;
	const unsigned long hs1=3;			/* round 3 shift amounts */
	const unsigned long hs2=9;
	const unsigned long hs3=11;
	const unsigned long hs4=15;
};

MD4::MD4()
{
    reset();
}

int MD4::arquivo(const char *arq)
{
	ifstream ifs(arq, ios::binary | ios::in);
	if(ifs.good())
		return calc(ifs);
	return -1;
}

int MD4::calc(const unsigned char *pnt, unsigned long tam)
{
	unsigned long x;
	reset();
	for(x=0;(x+64)<=tam;x+=64)
		update(pnt+x,64*8);
	if(x<tam)
		update(pnt+x,(tam-x)*8);
	update(pnt+x,0);
	return 0;
}

int MD4::calc(istream& is, unsigned long tam)
{
	//TODO: soh calcula de arquivos inteiros por enquanto...
	char X[64];
	int b;
	reset();
	while(is.good()&(!is.eof()))
	{
		is.read(X,64);
		b=is.gcount()*8;
		if(b)
			update((unsigned char*)X,b);
	}
	update((unsigned char*)X,0);
	return 0;
}

void MD4::reset()
{
    h.l[0]=I0;
    h.l[1]=I1;
    h.l[2]=I2;
    h.l[3]=I3;
	memset(count,0,sizeof(count));
    done=false;
}

int MD4::update(const unsigned char *X, unsigned long qtdbits)
{
    unsigned long i, tmp, bit, byte, mask;
    unsigned char XX[64];
    unsigned char *p;

    if ((qtdbits==0)&&done)
        return 0;
    if(done)
        return -1;

    tmp=qtdbits;
    p=count;

    while(tmp)
    {
        tmp+=*p;
        *p++=tmp;
        tmp=tmp>>8;
    }

    if(qtdbits==512)
        bloco((unsigned long*)X);
    else if(qtdbits > 512)
        return -2;
    else
    {
        byte=qtdbits >> 3;
        bit=qtdbits & 7;
        for(i=0;i<=byte;i++)
            XX[i]=X[i];
        for(i=byte+1;i<64;i++)
            XX[i]=0;
        mask=1<<(7-bit);
        XX[byte]=(XX[byte]|mask)&~(mask-1);
        if (byte<=55)
        {
            for(i=0;i<8;i++)
                XX[56+i]=count[i];
            bloco((unsigned long*)XX);
        }
        else
        {
            bloco((unsigned long*)XX);
            for(i=0;i<56;i++)
                XX[i]=0;
            for (i=0;i<8;i++)
                XX[56+i]=count[i];
            bloco((unsigned long*)XX);
        }
        done=true;
    }
    return 0;
}

void MD4::bloco(const unsigned long *X)
{
	register unsigned long tmp, A, B, C, D;

	A = h.l[0];
	B = h.l[1];
	C = h.l[2];
	D = h.l[3];

	#define f(X,Y,Z)             ((X&Y) | ((~X)&Z))
	#define g(X,Y,Z)             ((X&Y) | (X&Z) | (Y&Z))
	#define h(X,Y,Z)             (X^Y^Z)
	#define rot(X,S)             (tmp=X,(tmp<<S) | (tmp>>(32-S)))
	#define ff(A,B,C,D,i,s)      A = rot((A + f(B,C,D) + X[i]),s)
	#define gg(A,B,C,D,i,s)      A = rot((A + g(B,C,D) + X[i] + C2),s)
	#define hh(A,B,C,D,i,s)      A = rot((A + h(B,C,D) + X[i] + C3),s)

	ff(A , B , C , D ,  0 , fs1); /* Round 1 */
	ff(D , A , B , C ,  1 , fs2);
	ff(C , D , A , B ,  2 , fs3);
	ff(B , C , D , A ,  3 , fs4);
	ff(A , B , C , D ,  4 , fs1);
	ff(D , A , B , C ,  5 , fs2);
	ff(C , D , A , B ,  6 , fs3);
	ff(B , C , D , A ,  7 , fs4);
	ff(A , B , C , D ,  8 , fs1);
	ff(D , A , B , C ,  9 , fs2);
	ff(C , D , A , B , 10 , fs3);
	ff(B , C , D , A , 11 , fs4);
	ff(A , B , C , D , 12 , fs1);
	ff(D , A , B , C , 13 , fs2);
	ff(C , D , A , B , 14 , fs3);
	ff(B , C , D , A , 15 , fs4);

	gg(A , B , C , D ,  0 , gs1); /* Round 2 */
	gg(D , A , B , C ,  4 , gs2);
	gg(C , D , A , B ,  8 , gs3);
	gg(B , C , D , A , 12 , gs4);
	gg(A , B , C , D ,  1 , gs1);
	gg(D , A , B , C ,  5 , gs2);
	gg(C , D , A , B ,  9 , gs3);
	gg(B , C , D , A , 13 , gs4);
	gg(A , B , C , D ,  2 , gs1);
	gg(D , A , B , C ,  6 , gs2);
	gg(C , D , A , B , 10 , gs3);
	gg(B , C , D , A , 14 , gs4);
	gg(A , B , C , D ,  3 , gs1);
	gg(D , A , B , C ,  7 , gs2);
	gg(C , D , A , B , 11 , gs3);
	gg(B , C , D , A , 15 , gs4);

	hh(A , B , C , D ,  0 , hs1); /* Round 3 */
	hh(D , A , B , C ,  8 , hs2);
	hh(C , D , A , B ,  4 , hs3);
	hh(B , C , D , A , 12 , hs4);
	hh(A , B , C , D ,  2 , hs1);
	hh(D , A , B , C , 10 , hs2);
	hh(C , D , A , B ,  6 , hs3);
	hh(B , C , D , A , 14 , hs4);
	hh(A , B , C , D ,  1 , hs1);
	hh(D , A , B , C ,  9 , hs2);
	hh(C , D , A , B ,  5 , hs3);
	hh(B , C , D , A , 13 , hs4);
	hh(A , B , C , D ,  3 , hs1);
	hh(D , A , B , C , 11 , hs2);
	hh(C , D , A , B ,  7 , hs3);
	hh(B , C , D , A , 15 , hs4);

	h.l[0] += A;
	h.l[1] += B;
	h.l[2] += C;
	h.l[3] += D;
}
