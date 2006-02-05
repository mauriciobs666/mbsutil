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
