/* (C) Mauricio Stefani - 2010 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// http://tlt.its.psu.edu/suggestions/international/bylanguage/portuguese.html#htmlcodes

struct substitute
{
    char orig;
    char dest;
} subst[] =
{
    { 0xC0, 'A' },
    { 0xC1, 'A' },
    { 0xC2, 'A' },
    { 0xC3, 'A' },
    { 0xC9, 'E' },
    { 0xCA, 'E' },
    { 0xCD, 'I' },
    { 0xD3, 'O' },
    { 0xD4, 'O' },
    { 0xD5, 'O' },
    { 0xDA, 'U' },
    { 0xDC, 'U' },

    { 0xE0, 'a' },
    { 0xE1, 'a' },
    { 0xE2, 'a' },
    { 0xE3, 'a' },
    { 0xE9, 'e' },
    { 0xEA, 'e' },
    { 0xED, 'i' },
    { 0xF3, 'o' },
    { 0xF4, 'o' },
    { 0xF5, 'o' },
    { 0xFA, 'u' },
    { 0xFC, 'u' },

    { 0xC7, 'C' },
    { 0xE7, 'c' },
    { 0xBA, 'o' },
    { 0xAA, 'a' }
};

const int BUFFER_SIZE = 256;
const int PATH_SIZE = 256;

void printHelp(char *prg)
{
    printf("%s entrada.scr [saida.scr]\n", prg);
    exit(0);
}

int main(int argc, char *argv[])
{
    FILE *fdin = NULL;
    FILE *fdout = NULL;

    char *arqin = NULL;
    char *arqout = NULL;
    char arqbkp[PATH_SIZE];

    for(int argn=1; argn < argc; argn++)
    {
        if(argv[argn][0] == '-')
        {
            switch(argv[argn][1])
            {
            case '?':
            case 'h':
                printHelp(argv[0]);
                break;
            }
        }
        else
        {
            if( !arqin )
                arqin = argv[argn];
            else if( !arqout )
                arqout = argv[argn];
            else
            {
                printf("Excesso argumentos\n");
                exit(1);
            }
        }
    }

    if( !arqin )
        printHelp(argv[0]);

    printf("Entrada: %s\n", arqin);

    if( !arqout ) // cria backup
    {
        arqout = arqin;

        snprintf(arqbkp, PATH_SIZE, "%s.bkp", arqin);
        printf("Backup: %s\n", arqbkp);

        int rc = rename(arqin, arqbkp);
        if( rc != 0 )
        {
            printf("Erro fatal:\n");
            printf("rename(%s, %s)=%d\n", arqin, arqbkp, rc);
            exit(1);
        }

        arqin = arqbkp;
    }

    if( !(fdin = fopen(arqin,"rb")))
    {
        printf("Erro ao abrir arquivo de entrada %s\n", arqin);
        exit(1);
    }

    printf("Saida: %s\n", arqout);

    if( !(fdout = fopen(arqout,"wb")))
    {
        printf("Erro ao abrir arquivo de saida %s\n", arqout);
        exit(2);
    }

    size_t lidos;
    char linha[BUFFER_SIZE];
    while( (lidos=fread(linha,1,BUFFER_SIZE,fdin)) > 0 )
    {
        for(size_t i=0; i < lidos; i++)
            for(size_t j=0; j < (sizeof(subst) / sizeof(substitute)); j++)
                if(linha[i] == subst[j].orig)
                    linha[i] = subst[j].dest;
        fwrite(linha, 1, lidos, fdout);
    }

    fflush(fdout);
    fclose(fdout);
    fclose(fdin);

    exit(0);
}
