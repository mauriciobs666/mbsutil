#include "ProtocoloP2P.h"
#include <ios>
#include <fstream>

namespace
{
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
};

using namespace std;
using namespace Protocolo;

ClienteP2PUI *p2pui=NULL;
Mutex mutexMostrarMsg;
Mutex mutexLogar;

void mostrarMsg(Hash128 *user, string frase)
{
	mutexMostrarMsg.trava();
	if(p2pui!=NULL)
		p2pui->mostraMensagemChat(user,frase);
	else
	{
		cerr << "ClienteP2PUI::mostraMensagemChat(" << user->toString();
		cerr << ", " << frase << ")" << endl;
	}
	mutexMostrarMsg.destrava();
}

void logar(string frase)
{
	mutexLogar.trava();
	if(p2pui!=NULL)
		p2pui->relatorio(frase);
	else
		cerr << "ClienteP2PUI::relatorio(" << frase << ")" << endl;
	mutexLogar.destrava();
}

//------------------------------------------------------------------------------
//      Cliente
//------------------------------------------------------------------------------

ClienteP2P::ClienteP2P()
{
    strcpy(iU.nick,"Noob");
	iU.Hash128::random();
	slots.registraPacketHandler(this);
}

ClienteP2P::~ClienteP2P()
{
}

void ClienteP2P::registraUI(ClienteP2PUI *ui)
{
	p2pui=ui;
}

int ClienteP2P::abrir(std::string dir)
{
	ifstream arq;
	IOArquivo ret;
	string arquivo;

	arquivo="id.dat";
	ret=abrirID(arquivo);
	if(ERRO_VERSAO==ret)
		logar("Versao incorreta do arquivo "+arquivo+"\n");
	else if(ERRO_IO==ret)
		logar("Erro ao ler o arquivo "+arquivo+"\n");

//TODO:	iC.read(arq);

	arquivo="usuarios.dat";
	arq.open(arquivo.c_str(),ios::binary|ios::in);
	if(arq.good())
		usuarios.read(arq);
	else
		logar("Erro ao ler o arquivo "+arquivo+"\n");
    arq.close();
    arq.clear();

	arquivo="clientes.dat";
	arq.open(arquivo.c_str(),ios::binary|ios::in);
	if(arq.good())
		slots.nohs.read(arq);
	else
		logar("Erro ao ler o arquivo "+arquivo+"\n");
    arq.close();
    arq.clear();

	return 0;
}

int ClienteP2P::salvar(std::string dir)
{
	ofstream arq;
	IOArquivo ret;
	string arquivo;

	arquivo="id.dat";
	ret=salvarID(arquivo);
	if(ERRO_IO==ret)
		logar("Erro ao gravar o arquivo "+arquivo+"\n");

//TODO:	iC.write(arq);

	arquivo="usuarios.dat";
	arq.open(arquivo.c_str(),ios::binary|ios::out|ios::trunc);
	if(arq.good())
		usuarios.write(arq);
	else
		logar("Erro ao gravar o arquivo "+arquivo+"\n");
    arq.close();
    arq.clear();

    arquivo="clientes.dat";
	arq.open(arquivo.c_str(),ios::binary|ios::out|ios::trunc);
	if(arq.good())
		slots.nohs.write(arq);
	else
		logar("Erro ao gravar o arquivo "+arquivo+"\n");
    arq.close();
    arq.clear();

	return 0;
}

IOArquivo ClienteP2P::abrirID(std::string arquivo)
{
	ifstream arq;

	arq.open(arquivo.c_str(),ios::binary|ios::in);
	if(arq.good())
	{
		int ver;
		if(arq.read((char*)&ver,sizeof(ver)))
		{
			if(VERSAOINFO==ver)
			{
				if(iU.read(arq))
				{
					amigos.limpa();
					if(amigos.read(arq))
						return OK;
				}
			}
			else
				return ERRO_VERSAO;
		}
	}
	return ERRO_IO;
}

IOArquivo ClienteP2P::salvarID(std::string arquivo)
{
	ofstream arq;

	arq.open(arquivo.c_str(),ios::binary|ios::out|ios::trunc);
	if(arq.good())
	{
		int v=VERSAOINFO;
		if(arq.write((char*)&v,sizeof(v)))
			if(iU.write(arq))
				if(amigos.write(arq))
					return OK;
	}
	return ERRO_IO;
}

int ClienteP2P::enviarMsg(string msg, const Hash128* user)
{
	return enviarMsg(msg.c_str(),user);
}

int ClienteP2P::enviarMsg(const char *msg, const Hash128* user)
//  -1 = desconectado
//  -2 = erro
{
//TODO: begin gambia
    Slot *s=slots[*user];
    if(s==NULL)
		return -1;
	Noh n=s->iC;
//TODO: end gambia

    unsigned short tam=strlen(msg);
    Buffer *p=new Buffer(sizeof(COMANDO)+tam);	//comando + dados
    if(p==NULL)
        return -2;
    p->writeByte((COMANDO)MENSAGEM);
    memcpy(p->pntE,msg,tam);
    p->pntE+=tam;

    if(slots.enviar(p,n))
        return -2;
    return 0;
}

int ClienteP2P::enviaLogin(const Noh& n)
{
	Buffer *login=new Buffer(55);
	login->writeByte((COMANDO)LOGIN);
	slots.iC.write(login->pntE);
	iU.write(login->pntE);
	if(slots.enviar(login,n))
		return -1;
	return 0;
}

int ClienteP2P::IPH_tratar(Buffer *pacote, const Noh& n)
{
	Slot *slot=slots[n];
	COMANDO comando=pacote->readByte();
    switch(comando)
    {
		case LOGIN:
			#ifdef LOGAR_COMANDOS
				logar("CMD_LOGIN");
			#endif
			slot->iC.read(pacote->pntL);
			slot->iU.read(pacote->pntL);
			slot->setaEstado(Slot::CONECTADO);
			Usuario *tmp=usuarios.busca(slot->iU);
			if(tmp==NULL)
				usuarios.insere(slot->iU,new Usuario(slot->iU));
		break;
		case MENSAGEM:
			#ifdef LOGAR_COMANDOS
				logar("CMD_MENSAGEMDIRETA");
			#endif
			mostrarMsg(&slot->iU,string((char*)pacote->pntL,(size_t)pacote->disponiveis()));
		break;
		default:
			logar("CMD_INVALIDO");
		break;
    }
    delete pacote;
    return 0;
}

int ClienteP2P::IPH_conectado(const Noh& n)
{
	enviaLogin(n);
	return 0;
}

int ClienteP2P::IPH_desconectado(const Noh& n)
{
	return 0;
}

