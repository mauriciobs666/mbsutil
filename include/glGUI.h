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

#ifndef GLGUI_H
#define GLGUI_H
#include <windows.h>
#include "glfonte.h"
#include "geometria.h"
#include <list>
#include <vector>
#include <string>
#include "glimagem.h"

/**
	\brief Eventinho meia-boca, soh pra testes
*/
class guiEvento
{
	public:
		bool *keyboard;		//!<array com estados das teclas (indexado por VK_xxxx)
		guiEvento()
			{ keyboard=NULL; }
		virtual ~guiEvento()
			{}

};

/**
	\brief Interface a ser implementada pelos event handlers
*/
class guiTratadorEvento
{
	public:
		virtual int guiTrataEvento(guiEvento &e)
			{ return 0; }
};

/**
	\brief Classe base de todos componentes GUI
*/
class glWidget : public guiTratadorEvento
{
	public:
		Vetor2i pos;		//!<posicao do widget: sempre relativa ao pai
		Vetor2i tam;		//!<tamanho do widget
		glCor cor;			//!<cor
		bool hide;			//!<esconde o widget e todos os filhos

		glWidget() : hide(false)
			{}
		virtual ~glWidget()	//!mata todos os filhos
			{
				std::list<glWidget*>::iterator i;
				for(i=filhos.begin();i!=filhos.end();i++)
					delete (*i);
			}
		void add(glWidget* w)		//!insere um filho na lista
			{ filhos.push_back(w); }
		virtual int desenha();
		virtual int guiTrataEvento(guiEvento &e);
	protected:
		std::list<glWidget*> filhos;//!<filhos
};

/**
	\brief Gerenciador de janelas
*/
class glGUI : private glWidget
{
	public:
		GLFonte fonte;			//!<nao eh usada aqui, apenas pra fins de armazenagem

		glGUI()
			{ id=0; foco=NULL; }
		virtual ~glGUI()
			{}
		void add(glWidget* w)
			{ filhos.push_back(w); }
		int desenha();
		int guiTrataEvento(guiEvento &e);
		glWidget *foco;
	protected:
	private:
		int id;					//!<ID unico e sequencial das janelas
};

class glLabel : public glWidget
{
	public:
		GLFonte& fonte;			//!<referencia pra algum objeto de fonte
		std::string caption;	//!<texto a ser mostrado

		glLabel(GLFonte& fon) : fonte(fon) {}
		glLabel(GLFonte& fon, std::string texto) : fonte(fon), caption(texto) {}
		virtual int desenha();
	private:
		void attach(glWidget* w);
};

class glPanel : public glWidget
{
	public:
		glPanel(int largura=16, int altura=16, int posX=0, int posY=0)
			{ tam.x=largura; tam.y=altura; pos.x=posX; pos.y=posY; }
		virtual int desenha();
};

/**
	\brief Memo, read-only
*/
class glMemo : public glWidget
{
	public:
		GLFonte& fonte;			//!<referencia pra algum objeto de fonte
		glCor corFonte;			//!<cor do texto

		glMemo(GLFonte& fon) : fonte(fon) {}
		int clear()
			{ linhas.clear(); return 0; }
		int println(std::string linha)
			{ linhas.push_back(linha); return 0; }
		virtual int desenha();
	private:
		std::list<std::string> linhas;
		void attach(glWidget* w);
};

/**
	\brief Menu estilo videogame
*/
class glMenu : public glWidget
{
	public:
		GLFonte& fonte;			//!<referencia pra algum objeto de fonte
		int selecionado;		//!<item selecionado
		glCor corFonte;			//!<cor dos items
		glCor corSeletor;		//!<cor de fundo do seletor

		glMenu(GLFonte& fon) : fonte(fon), selecionado(0) {}
		int addItem(std::string item)
			{ items.push_back(item); return items.size()-1; }
		int nItems()
			{ return items.size(); }
		virtual int desenha();
		virtual int guiTrataEvento(guiEvento &e);
		void sobe()
			{ if(selecionado>0) selecionado--; }
		void desce()
			{ if(selecionado<(nItems()-1)) selecionado++; }
	private:
		std::vector<std::string> items;
		void attach(glWidget* w);
};
#endif // GLGUI_H
