#ifndef MBSUTIL_P2PDEFS_H
#define MBSUTIL_P2PDEFS_H

//#define LOGAR_COMANDOS
//#define LOGAR_SOCKET

namespace Protocolo
{
	typedef unsigned short TAMANHO;
	typedef unsigned short COMANDO;

	//	Constantes diversas
	const int PORTA_PADRAO=6661;
	const int TAMNICK=15+1;
	const int VERSAOINFO=1;		//versao dos arquivos

	typedef enum
	{
		ERRO_IO,		//erro de leitura/escrita
		ERRO_VERSAO,	//versao incorreta do arquivo
		OK,				//tudo ok
	} IOArquivo;
};
#endif
