#ifndef MBSUTIL_HASH_H
#define MBSUTIL_HASH_H

#include <stdio.h>
#include <string>
#include <ostream>

class Hash128
{
public:
	union hash
	{
		unsigned char b[16];
		unsigned long l[4];
	} h;

	int cmp(const Hash128& base) const
	/*
		 1 : local > base
		-1 : local < base
		 0 : local == base
	*/
	{
		return memcmp(h.b,base.h.b,16); 
	}

	bool operator==(const Hash128& base) const { return (cmp(base)==0); }
	bool operator<(const Hash128& base) const { return (cmp(base)<0); }

	void random();
	std::string toString() const;
	char* dump(char *dest) const;
	// dest -> DEVE apontar espaco com no MINIMO 33 bytes

	int read(unsigned char *&pkt);
	int write(unsigned char *&pkt) const;
	int read(FILE *arq);
	int write(FILE *arq) const;
};

std::ostream& operator<<(std::ostream& os, const Hash128& h);

class MD4 : public Hash128
{
public:
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
