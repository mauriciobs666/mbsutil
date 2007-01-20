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

#include "glgui.h"
#include <gl/gl.h>
#include <gl/glu.h>

using namespace std;

// ============================================================================
// GLWIDGET
// ============================================================================

int glWidget::guiTrataEvento(guiEvento &e)
{
	return 0;
}

int glWidget::desenha()
{
	int ret=0;
	glWidget *tmp;
	std::list<glWidget*>::iterator i;

	for(i=filhos.begin();i!=filhos.end();i++)	//percorre toda lista de filhos
	{
		tmp=*i;
		if(!tmp->hide)							//soh desenha sub-arvore se hide==false
		{
			glPushMatrix();						//entrando espaco do filho
				glTranslatef(tmp->pos.x,tmp->pos.y,0);
				glColor4ubv(tmp->cor.ubv());	//cor do filho
				ret=tmp->desenha();				//desenha o filho
				if(0==ret)						//se tudo OK
					tmp->glWidget::desenha();	//desenha os netos
			glPopMatrix();						//voltando
		}
		if(ret)
			break;
	}
	return ret;
}

int glWidget::aplicaTema(glTema &t)
{
	cor=t.cor;
	fonteCor=t.fonteCor;
	borda=t.borda;
	fonte=t.fonte;
	return 0;
}

// ============================================================================
// GLWINDOW
// ============================================================================

int glWindow::desenha()
{
	glPushMatrix();
		glTranslatef(pos.x,pos.y,0);
		if(!titulo.hide&&titulo.altura>0)
		{
			glColor4ubv(titulo.cor.ubv());
			glBegin(GL_POLYGON);
				glVertex2i(0,0);
				glVertex2i(tam.x,0);
				glVertex2i(tam.x,titulo.altura);
				glVertex2i(0,titulo.altura);
			glEnd();
			glColor4ubv(titulo.fonteCor.ubv());
			glRasterPos2f(borda,((fonte.altura*3)/4));
			fonte.printf(titulo.caption.c_str());
			glTranslatef(0,titulo.altura,0);
		}
		glWidget::desenha();
	glPopMatrix();
	return 0;
}

int glWindow::guiTrataEvento(guiEvento &e)
{
	if(foco)
		foco->guiTrataEvento(e);
	return 0;
}

// ============================================================================
// GLGUI
// ============================================================================

int glGUI::guiTrataEvento(guiEvento &e)
{
	if(foco)
		foco->guiTrataEvento(e);
	return 0;
}

int glGUI::desenha()
{
	//seta modo de video
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, 640, 480, 0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(0.0f,0.0f,-1.0f);

	//ativa blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glShadeModel(GL_FLAT);

	std::list<glWindow*>::iterator i;
	for(i=janelas.begin();i!=janelas.end();i++)	//desenha todas as janelas
		if(!(*i)->hide)
			(*i)->desenha();					//chama funcao sobrecarregada de glWindow

	glDisable(GL_BLEND);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	return 0;
}

// ============================================================================
// GLLABEL
// ============================================================================

int glLabel::desenha()
{
	fonte.printf(caption.c_str());
	return 0;
}

// ============================================================================
// GLPANEL
// ============================================================================

int glPanel::desenha()
{
	glBegin(GL_POLYGON);
		glVertex2i(0,0);
		glVertex2i(tam.x,0);
		glVertex2i(tam.x,tam.y);
		glVertex2i(0,tam.y);
	glEnd();
	return 0;
}

// ============================================================================
// GLMEMO
// ============================================================================

int glMemo::desenha()
{
	glBegin(GL_POLYGON);
		glVertex2i(0,0);
		glVertex2i(tam.x,0);
		glVertex2i(tam.x,tam.y);
		glVertex2i(0,tam.y);
	glEnd();

	glColor4ubv(fonteCor.ubv());
	list<string>::iterator i;
	int x=0;
	for(i=linhas.begin();i!=linhas.end();i++)
	{
		glRasterPos2f(borda,x+((fonte.altura*3)/4));
		fonte.printf((*i).c_str());
		x+=fonte.altura;
	}
	return 0;
}

// ============================================================================
// GLMENU
// ============================================================================

void glMenu::selecionar(int i)
{
	sel=i;
	if(sel>=nItems())
		sel=nItems()-1;
	if(sel<0)
		sel=0;

	if(topo>sel)
		topo=sel;
	int cabem=tam.y/fonte.altura;
	if(topo<(sel-(cabem-1)))
		topo=(sel-(cabem-1));
}

int glMenu::desenha()
{
	//desenha o fundo (falta borda)
	glBegin(GL_POLYGON);
		glVertex2i(0	,0);
		glVertex2i(tam.x,0);
		glVertex2i(tam.x,tam.y);
		glVertex2i(0	,tam.y);
	glEnd();

	int y=0;								//lado superior da linha atual
	const int cabem=tam.y/fonte.altura;		//qtas linhas cabem no widget

	//desenha menu com scroll
	for(int i=topo;((i<nItems())&&(i<(topo+cabem)));i++)
	{
		if(i==sel)	//desenha seletor
		{
			glColor4ubv(corSeletor.ubv());
			glBegin(GL_POLYGON);
				glVertex2i(borda		, y+borda);
				glVertex2i(tam.x-borda	, y+borda);
				glVertex2i(tam.x-borda	, y+(fonte.altura-borda));
				glVertex2i(borda		, y+(fonte.altura-borda));
			glEnd();
			glColor4ubv(cor.ubv());			//inverte cor da fonte
		}
		else
			glColor4ubv(fonteCor.ubv());

		//texto do item
		glRasterPos2f(borda+1,y+((fonte.altura*3)/4));
		fonte.printf(items[i].c_str());

		//desce uma linha
		y+=fonte.altura;
	}
	return 0;
}

int glMenu::guiTrataEvento(guiEvento &e)
{
	if(e.keyboard)
	{
		if(e.keyboard[VK_UP])
		{
			e.keyboard[VK_UP]=false;
			sobe();
		}
		if(e.keyboard[VK_DOWN])
		{
			e.keyboard[VK_DOWN]=false;
			desce();
		}
	}
	return 0;
}
