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

int glWindow::guiTrataEvento(guiEvento &e)
{
	if(foco)
		foco->guiTrataEvento(e);
	return 0;
}

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
		(*i)->desenha();						//chama funcao herdada de glWidget

	glDisable(GL_BLEND);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	return 0;
}

int glLabel::desenha()
{
	glRasterPos2f(0,-fonte.altura/2);
	fonte.printf(caption.c_str());
	return 0;
}

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

int glMemo::desenha()
{
	glBegin(GL_POLYGON);
		glVertex2i(0,0);
		glVertex2i(tam.x,0);
		glVertex2i(tam.x,tam.y);
		glVertex2i(0,tam.y);
	glEnd();

	glColor4ubv(corFonte.ubv());
	list<string>::iterator i;
	int x=0;
	for(i=linhas.begin();i!=linhas.end();i++)
	{
		glRasterPos2f(1,x-fonte.altura/2);
		fonte.printf((*i).c_str());
		x-=fonte.altura;
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
	if(topo<(sel-cabem))
		topo=(sel-cabem);
}

int glMenu::desenha()
{
	glBegin(GL_POLYGON);
		glVertex2i(0,0);
		glVertex2i(tam.x,0);
		glVertex2i(tam.x,tam.y);
		glVertex2i(0,tam.y);
	glEnd();

	int y=0;
	int n=0;
	int cabem=tam.y/fonte.altura;

	for(int i=topo;(i<nItems())&(i<(topo+cabem));i++)
	{
		if(n==sel)	//desenha seletor
		{
			glColor4ubv(corSeletor.ubv());
			glBegin(GL_POLYGON);
				glVertex2i(2,y-2);
				glVertex2i(tam.x-2,y-2);
				glVertex2i(tam.x-2,y-fonte.altura-2);
				glVertex2i(2,y-fonte.altura-2);
			glEnd();
		}

		glColor4ubv(corFonte.ubv());
		glRasterPos2f(1,y-fonte.altura/2);
		fonte.printf(items[i].c_str());

		y-=fonte.altura;
		n++;
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
