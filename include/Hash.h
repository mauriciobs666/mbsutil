#ifndef MBSUTIL_HASH_H
#define MBSUTIL_HASH_H

#include <string>
#include <iostream>

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
		{ return memcmp(h.b,base.h.b,16); }

	bool operator==(const Hash128& base) const
		{ return (cmp(base)==0); }
	bool operator<(const Hash128& base) const
		{ return (cmp(base)<0); }

	void random();
	std::string toString() const;
	int fromString(const std::string& s);
	char* dump(char *dest) const;	// dest DEVE apontar MINIMO 32+1 bytes

	int read(unsigned char *&pkt);
	int write(unsigned char *&pkt) const;
    std::istream& read(std::istream& is)
		{ return is.read((char*)h.b,sizeof(h)); }
    std::ostream& write(std::ostream& os) const
		{ return os.write((char*)h.b,sizeof(h)); }
};
std::ostream& operator<<(std::ostream& os, const Hash128& h);
std::istream& operator>>(std::istream& is, Hash128& h);

class MD4 : public Hash128
{
public:
	MD4();
	int arquivo(const char *arq);
private:
	unsigned char count[8];
	unsigned int done;
	void bloco(unsigned int *X);
	void reset();
	int update(unsigned char *X,unsigned int qtd);
};

#endif
