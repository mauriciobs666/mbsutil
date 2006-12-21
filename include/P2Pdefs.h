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

#ifndef MBSUTIL_P2PDEFS_H
#define MBSUTIL_P2PDEFS_H

//#define LOGAR_COMANDOS
//#define LOGAR_SOCKET

namespace Protocolo
{
	typedef unsigned short TAMANHO;
	typedef unsigned char COMANDO;

	//	Constantes diversas
	const int PORTA_PADRAO=6661;
	const int TAMNICK=15+1;
	const int VERSAOINFO=1;		//!<versao dos arquivos

	//!Resultados de IO de arquivo
	typedef enum
	{
		ERRO_IO,		//!<erro de leitura/escrita
		ERRO_VERSAO,	//!<versao incorreta do arquivo
		OK,				//!<tudo ok
	} IOArquivo;
};
#endif
