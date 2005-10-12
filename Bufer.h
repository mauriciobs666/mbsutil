#ifndef MBSUTIL_BUFFER_H
#define MBSUTIL_BUFFER_H

class Buffer
{
public:
	unsigned char *dados;
	unsigned char *pntE,*pntL;
	unsigned long ocupados;

	Buffer(unsigned long tam=0);
	unsigned long pegaTamanho() const 
		{ return tamanho; }
	int mudaTamanho(unsigned long tam);
	~Buffer() 
		{ if(dados) delete[] dados; }

	void resetPnt();							
		//pntL=dados,pntE=(dados+ocupados)
	unsigned long faltaTratar() const;			
		//pntE-pntL
	unsigned long disponiveis() 
		{ return (pntE>pntL)?(pntE-pntL):0; }
	unsigned long livres() const 
		{ return tamanho-ocupados; }
	unsigned long append(Buffer *b, unsigned long qtd=0);
		//anexa qtd bytes de b->pntL em pntE
		//se qtd==0, anexa faltaTratar() bytes de b->pntL em pntE
protected:
	unsigned long tamanho;
};
#endif
