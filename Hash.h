#ifndef hashH
#define hashH

#include <stdio.h>

class Hash
{
	public:
		virtual bool operator==(const Hash& hash);
};

class Hash128 //: public Hash
{
	public:
		bool operator==(const Hash& hash);
		
		union hash
		{
			unsigned char b[16];
			unsigned long l[4];
		}h;
		void random();
        int compara(Hash128 *base);
        char* dump(char *dest);
        int abrirPacote(unsigned char *&pkt);
        int montarPacote(unsigned char *&pkt);
        int abrir(FILE *arq);
        int salvar(FILE *arq);
};

class MD4 : public Hash128
{
    public:
//        Hash128 h;
        MD4();
        int arquivo(char *arq);
    private:
        unsigned char count[8];
        unsigned int done;
        void bloco(unsigned int *X);
        void reset();
        int update(unsigned char *X,unsigned int qtd);
};

#endif
