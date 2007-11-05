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

#include "GerenciadorSlots.h"
#include "Parser.h"

using namespace std;

void logar(string frase);

//------------------------------------------------------------------------------
//      GerenciadorSlots
//------------------------------------------------------------------------------

GerenciadorSlots::GerenciadorSlots(int num)
{
	slots=NULL;
	numSlots=0;
	mudaNumSlots(num);
    serverSock.pai=this;
    serverSock.registraCallback(tratarServer);
}

GerenciadorSlots::~GerenciadorSlots()
{
    if((slots!=NULL)&&(numSlots>0))
        delete[] slots;
}

int GerenciadorSlots::mudaNumSlots(int num)
{
	if(num>0)
	{
		Slot *temp=new Slot[num];
		if(temp==NULL)
			return -1;		//erro de alocacao
		if((slots!=NULL)&&(numSlots>0))
			delete[] slots;
		slots=temp;
		for(int n=0;n<num;n++)
			slots[n].registraFrameHandler(this);
	}
	else if((slots!=NULL)&&(numSlots>0))
	{
		delete[] slots;
		slots=NULL;
	}
	numSlots=num;
	return 0;
}

Slot* GerenciadorSlots::at(int num) const
{
    if((num>=0)&(num<numSlots))
        return &slots[num];
    return NULL;
}

Slot* GerenciadorSlots::operator[](std::string id) const
{
    for(int x=0;x<numSlots;x++)
    	if(id==slots[x].id)
			return &slots[x];
    return NULL;
}

int GerenciadorSlots::aloca()
// busca e aloca (estado 0->1) slot livre
// retorna num do slot ou -1 caso todos estejam ocupados
{
    cs.trava();
    {
        for(int x=0;x<numSlots;x++)
        {
            if(slots[x].pegaEstado()==Slot::LIVRE)
            {
                if(slots[x].setaEstado(Slot::RESERVADO)==0)
                {
                    cs.destrava();
                    return x;
                }
            }
        }
    }
    cs.destrava();
    return -1;
}

int GerenciadorSlots::ouvir(unsigned short porta)
{
	int retorno=0;
	serverSock.desconectar();
	if((retorno=serverSock.ouvir(porta))==0)
		logar("Esperando conexoes na porta "+int2str(porta));
	else
		logar("Erro ao tentar abrir porta "+int2str(porta));
	return retorno;
}

int GerenciadorSlots::desconectar(int nslot)
{
	if(nslot<0)
		for(int x=0;x<numSlots;x++)
			slots[x].desconectar();
	else if(nslot<numSlots)
		slots[nslot].desconectar();
	else
		return -1;
    return 0;
}

int GerenciadorSlots::tratarServer(Conexao *con, long codeve, long coderro[])
{
	GerenciadorSlots *pai;
	if((pai=(GerenciadorSlots*)con->pai)==NULL)
		return -1;

	if(codeve & FD_ACCEPT)
	{
		#ifdef LOGAR_SOCKET
			logar("FD_ACCEPT");
		#endif
		int ns=pai->aloca();
		if(ns<0)
		{
			con->recusar();
			logar("Conexão recusada");
			return 0;
		}
		pai->slots[ns].conectar(con->aceitar());
	}
	if(codeve & FD_CLOSE)
		return 1;	//matar thread...
	return 0;
}
