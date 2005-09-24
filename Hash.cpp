#include "Hash.h"
#include <string.h>
#include <stdlib.h>

void Hash128::random()
{
//	randomize();
    for(int x=0;x<16;x++)
		h.b[x]=rand();
}

int Hash128::compara(Hash128 *base)
{
//  1 : local > base
// -1 : local < base
//  0 : local == base
    unsigned char *bas=base->h.b;
    unsigned char ba;
    for(int x=0;x<16;x++)
    {
        ba=*bas++;
        if(h.b[x]<ba)
            return -1;
        else if(h.b[x]>ba)
            return 1;
    }
    return 0;
}

char* Hash128::dump(char *dest)
// dest -> DEVE apontar espaco com no MINIMO 33 bytes
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

int Hash128::abrirPacote(unsigned char *&pkt)
{
    memcpy(h.b,pkt,16);
    pkt+=16;
    return 0;
}

int Hash128::montarPacote(unsigned char *&pkt)
{
    memcpy(pkt,h.b,16);
    pkt+=16;
    return 0;
}

int Hash128::abrir(FILE *arq)
{
    if(fread(h.b,sizeof(char),16,arq)!=16)
        return -1;
    return 0;
}

int Hash128::salvar(FILE *arq)
{
    if(fwrite(h.b,sizeof(char),16,arq)!=16)
        return -1;
    return 0;
}

//------------------------------------------------------------------------------
//      MD4 - implementado a partir da RFC 1320
//------------------------------------------------------------------------------

#define I0  0x67452301       /* Initial values for MD buffer */
#define I1  0xefcdab89
#define I2  0x98badcfe
#define I3  0x10325476
#define C2  013240474631     /* round 2 constant = sqrt(2) in octal */
#define C3  015666365641     /* round 3 constant = sqrt(3) in octal */
 /* C2 and C3 are from Knuth, The Art of Programming, Volume 2
 ** (Seminumerical Algorithms), Second Edition (1981), Addison-Wesley.
 ** Table 2, page 660.
 */

 #define fs1  3               /* round 1 shift amounts */
 #define fs2  7
 #define fs3 11
 #define fs4 19
 #define gs1  3               /* round 2 shift amounts */
 #define gs2  5
 #define gs3  9
 #define gs4 13
 #define hs1  3               /* round 3 shift amounts */
 #define hs2  9
 #define hs3 11
 #define hs4 15

 #define f(X,Y,Z)             ((X&Y) | ((~X)&Z))
 #define g(X,Y,Z)             ((X&Y) | (X&Z) | (Y&Z))
 #define h(X,Y,Z)             (X^Y^Z)
 #define rot(X,S)             (tmp=X,(tmp<<S) | (tmp>>(32-S)))
 #define ff(A,B,C,D,i,s)      A = rot((A + f(B,C,D) + X[i]),s)
 #define gg(A,B,C,D,i,s)      A = rot((A + g(B,C,D) + X[i] + C2),s)
 #define hh(A,B,C,D,i,s)      A = rot((A + h(B,C,D) + X[i] + C3),s)

MD4::MD4()
{
    reset();
}

int MD4::arquivo(char *arq)
{
    unsigned char X[64];
    int b;
    FILE *f = fopen(arq,"rb");
    if (f==NULL)
        return -1;
    reset();
    while((b=fread(X,1,64,f))!=0)
        update(X,b*8);
    update(X,0);
    fclose(f);
    return 0;
}

void MD4::reset()
{
    h.h.l[0]=I0;
    h.h.l[1]=I1;
    h.h.l[2]=I2;
    h.h.l[3]=I3;
    for(int i=0;i<8;i++)
        count[i]=0;
    done=0;
}

int MD4::update(unsigned char *X,unsigned int qtd)
{
    unsigned int i, tmp, bit, byte, mask;
    unsigned char XX[64];
    unsigned char *p;

    if ((qtd==0)&&done)
        return 0;
    if(done)
        return -1;

    tmp=qtd;
    p=count;
    while(tmp)
    {
        tmp+=*p;
        *p++=tmp;
        tmp=tmp>>8;
    }
    if(qtd==512)
        bloco((unsigned int*)X);
    else if (qtd > 512)
        return -2;
    else
    {
        byte=qtd >> 3;
        bit=qtd & 7;
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
            bloco((unsigned int*)XX);
        }
        else
        {
            bloco((unsigned int *)XX);
            for(i=0;i<56;i++)
                XX[i]=0;
            for (i=0;i<8;i++)
                XX[56+i]=count[i];
            bloco((unsigned int*)XX);
        }
        done=1;
    }
    return 0;
}

void MD4::bloco(unsigned int *X)
{
    register unsigned int tmp, A, B, C, D;

    A = h.h.l[0];
    B = h.h.l[1];
    C = h.h.l[2];
    D = h.h.l[3];

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

    h.h.l[0] += A;
    h.h.l[1] += B;
    h.h.l[2] += C;
    h.h.l[3] += D;
}
