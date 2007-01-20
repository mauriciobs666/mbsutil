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
	\brief Tema padrao pros componentes e janelas
*/
class glTema
{
	public:
		glCor cor;			//!<cor de fundo

		GLFonte fonte;		//!<fonte pra renderizar texto comum
		glCor fonteCor;		//!<cor da fonte pra texto comum

		int borda;			//!<tamanho da borda
};

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
		GLFonte& fonte;		//!<fonte pra texto comum
		glCor fonteCor;		//!<cor da fonte pra texto comum
		glCor cor;			//!<cor
		int borda;			//!<tamanho da borda
		glTema &temaAtual;	//!<referencia pro tema em uso (pra propriedades mais especificas)
		bool hide;			//!<esconde o widget e todos os filhos

		glWidget(glTema &tema) : fonte(tema.fonte), temaAtual(tema), hide(false)
			{ aplicaTema(tema); }
		virtual ~glWidget()	//!mata todos os filhos
			{
				std::list<glWidget*>::iterator i;
				for(i=filhos.begin();i!=filhos.end();i++)
					delete (*i);
			}
		void add(glWidget* w)		//!insere um filho na lista
			{ filhos.push_back(w); }
		virtual int desenha();		//!<interface pros filhos desenharem
		virtual int aplicaTema(glTema &t);//!aplica tema
		virtual int guiTrataEvento(guiEvento &e);	//!<implementacao da interface herdada de guiTratadorEvento
	protected:
		std::list<glWidget*> filhos;//!<filhos
};

/**
	\brief Container basico, menor unidade gerenciavel pelo glGUI
*/
class glWindow : public glWidget
{
	public:
		struct BarraDeTitulo
		{
			BarraDeTitulo() : hide(false), altura(0) {}
			bool hide;
			std::string caption;
			int altura;
			glCor cor;
			glCor fonteCor;
		} titulo;


		glWindow(glTema &tema) : glWidget(tema), foco(NULL), id(0)
			{}
		virtual ~glWindow() {}

		int desenha();
		int getId() { return id; }
		int guiTrataEvento(guiEvento &e);
	protected:
		glWidget *foco;
	private:
		friend class glGUI;
		int id;
};

/**
	\brief Gerenciador de janelas
*/
class glGUI
{
	public:
		glTema temaPadrao;

		glGUI()
			{ contador=0; foco=NULL; }
		virtual ~glGUI()
			{}
		void add(glWindow* w)
			{
				w->id=contador++;
				janelas.push_back(w);
			}
		int desenha();
		glWindow* operator[](int id);

		int guiTrataEvento(guiEvento &e);
		glWidget* foco;
	protected:
	private:
		int contador;			//!<ID unico e sequencial das janelas
		std::list<glWindow*> janelas;
};

/**
	\brief Texto simples
*/
class glLabel : public glWidget
{
	public:
		std::string caption;	//!<texto a ser mostrado

		glLabel(glTema &tema) : glWidget(tema)
			{}
		glLabel(glTema &tema, std::string texto) : glWidget(tema), caption(texto)
			{}
		virtual int desenha();
	private:
		void add(glWidget* w);	//!<esteril
};

/**
	\brief Container
*/
class glPanel : public glWidget
{
	public:
		glPanel(glTema &tema, int largura=16, int altura=16, int posX=0, int posY=0) : glWidget(tema)
			{ tam.x=largura; tam.y=altura; pos.x=posX; pos.y=posY; }
		virtual int desenha();
};

/**
	\brief Memo, read-only
*/
class glMemo : public glWidget
{
	public:
		glMemo(glTema &tema) : glWidget(tema)
			{ }
		int clear()
			{ linhas.clear(); return 0; }
		int println(std::string linha)
			{ linhas.push_back(linha); return 0; }
		virtual int desenha();
	private:
		std::list<std::string> linhas;
		void add(glWidget* w);	//!<esteril
};

/**
	\brief Menu estilo videogame
*/
class glMenu : public glWidget
{
	public:
		glCor corSeletor;		//!<cor de fundo do seletor

		glMenu(glTema &tema) : glWidget(tema), sel(0), topo(0)
			{ }
		int addItem(std::string item)
			{ items.push_back(item); return items.size()-1; }
		int nItems()
			{ return items.size(); }
		virtual int desenha();
		virtual int guiTrataEvento(guiEvento &e);
		void selecionar(int i);
		std::string selecionado()
			{ return items[sel]; }
		void sobe()
			{ selecionar(sel-1); }
		void desce()
			{ selecionar(sel+1); }
	private:
		int sel;				//!<item selecionado
		int topo;				//!<indice do item mostrado no topo do componente
		std::vector<std::string> items;
		void add(glWidget* w);	//!<esteril
};
#endif // GLGUI_H
