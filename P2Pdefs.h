#ifndef MBSUTIL_P2PDEFS_H
#define MBSUTIL_P2PDEFS_H

namespace Protocolo
{
	typedef unsigned short TAMANHO;
	typedef unsigned short COMANDO;

	typedef enum CmdCamada1
	{
		PING,
		/*	Ping
			[unsigned long timestamp]
		*/
		PONG,
		/*	Resposta ao ping
			[unsigned long timestamp] (copia do recebido em ping)
		*/
		ROTEAR,
		/*	Pedido de roteamento do pacote pra cliente com id baixa
			[remetente][destinatario][dados]
			[Noh][Hash128]
		*/
		ROTEAR_ERRO,
		/*	Nao foi possivel rotear pacote (Usuario desconectado)
		*/
		ROTEAR_BLOQUEADO,
		/*	Cliente nao aceita rotear este comando
		*/
		RETORNO,
		/*	Pedido de conexao de callback
		*/
		BUSCAR,
		/*	Broadcast de busca
			[unsigned char TTL][criterio]
		*/
		HIT,
		/*
			Encontrado
		*/
		ACK,
		/*	Acknowledge (pra chat p.e)
		*/
		LOGIN,
		/*	pacote com informacoes sobre o cliente e usuario
			[infoCliente][Usuario]
				infoCliente = [versao][opcoes][Noh][mtu][mru]
					Noh = [ip][porta][id]
						sizeof(Noh)=10
					sizeof(infoCliente)=12+sizeof(Noh)=22
				infoUsuario = [Hash128][Nick]
					sizeof(infoUsuario)=16+TAMNICK=32
			sizeof(dados)=54
		*/
		MENSAGEM,
		/*	Mensagem instantanea (texto/chat)
			[char[] (nao-ASCIIZ)]
		*/
	};

	typedef enum CmdCamada2
	{
		USER,
		/*	protocolo definido pelo usuario
			[unsigned char codigo do protocolo][dados]
		*/
	};

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
