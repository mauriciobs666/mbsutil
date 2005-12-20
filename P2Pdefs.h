#ifndef MBSUTIL_P2PDEFS_H
#define MBSUTIL_P2PDEFS_H

namespace Protocolo
{
	typedef unsigned short TAMANHO;
	typedef unsigned short COMANDO;

	typedef enum CmdCamada1
	{
		DIRETA,
		/*
		*/
		PING,
		/*	Ping
			[unsigned long timestamp]
		*/
		PONG,
		/*	Resposta ao ping
			[unsigned long timestamp] (copia do recebido em ping)
		*/
		BROADCAST,
		/*
			[unsigned char TTL][Noh remetente][unsigned long seqno][dados]
		*/
		ROTEAR,
		/*	Pedido de roteamento do pacote pra cliente com id baixa
			[Noh remetente][Noh destinatario][dados]
		*/
		ROTEAR_ERRO,
		/*	Nao foi possivel rotear pacote (Usuario desconhecido)
		*/
		ROTEAR_BLOQUEADO,
		/*	Cliente nao aceita rotear este comando
		*/
	};

	typedef enum CmdCamada2
	{
		MENSAGEM,
		/*	Mensagem instantanea (texto/chat)
			[char[] (nao-ASCIIZ)]
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
		RETORNO,
		/*	Pedido de conexao de callback
		*/
		BUSCAR,
		/*	Broadcast de busca
		*/
		HIT,
		/*	Encontrado
		*/
		ACK,
		/*	Acknowledge (pra chat p.e)
		*/
		USER,
		/*	protocolo definido pelo usuario
			[unsigned char codigo do protocolo][dados]
		*/
	};

	//	Constantes diversas
	const int PORTA_PADRAO=6661;
	const int TAMNICK=15+1;
	const int VERSAOINFO=1;		//versao dos arquivos
	const int TIMEOUT_RX=60;	//timeout de recepcao de frames (em segundos)

	typedef enum
	{
		ERRO_IO,		//erro de leitura/escrita
		ERRO_VERSAO,	//versao incorreta do arquivo
		OK,				//tudo ok
	} IOArquivo;
};
#endif
