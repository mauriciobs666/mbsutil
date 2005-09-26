#ifndef MBSUTIL_BUFFER_H
#define MBSUTIL_BUFFER_H

class Buffer
{
    public:
        unsigned char *dados,*pntE,*pntL;
        int ocupados;

        Buffer();
        Buffer(int tam);
        unsigned long pegaTamanho();
        int mudaTamanho(int tam);
        ~Buffer();

        void resetPnt();        //pntL=dados,pntE=(dados+ocupados)
        int faltaTratar();      //pntE-pntL
        int livres();
        int append(Buffer *b);  //anexa faltaTratar() bytes de b->pntL em pntE
        int append(Buffer *b, int qtd); //anexa qtd bytes de b->pntL em pntE
    protected:
        int tamanho;
        void mata(unsigned char *&end);
        int aloca(unsigned char *&end, int qtd);
};
#endif
