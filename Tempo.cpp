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

#include "Tempo.h"
#include <ctime>
#include <windows.h>

using namespace std;

Tempo::Tempo()
{
	reset();
	logarEventos=(EVE_TEMPO_100MS|EVE_TEMPO_1S);
}

Tempo::Tempo(unsigned long quaisEventos)
{
	reset();
	logarEventos=quaisEventos;
}

void Tempo::reset()
{
	ultimo=agora=getMilissegundos();
	ttotal=T1ds=0;				//segundo atual e decimos do segundo atual
	frames=fps=fpstemp=0;
}

void Tempo::loop()
{
	frames++;
	fpstemp++;
	agora=getMilissegundos();
}

unsigned long Tempo::getFrames()
{
	return frames;
}

unsigned long Tempo::getFPS()
{
	return fps;
}

unsigned long Tempo::getFPSMedio()
{
	if(ttotal>0)
		return (frames/ttotal);
	return 0;
}

unsigned long Tempo::trataEvento()
{
	if(T1ds>=10)
  	{	//uma a cada 1 seg
  		T1ds-=10;
  		ttotal++;
		fps=fpstemp;
		fpstemp=0;
		if(logarEventos&EVE_TEMPO_1S)
			return EVE_TEMPO_1S;
  	}
	if((agora-ultimo)>100)
	{	//uma a cada 100ms
		ultimo+=100;
  		T1ds++;
  		if(logarEventos&EVE_TEMPO_100MS)
	  		return EVE_TEMPO_100MS;
	}
	return 0;
}

unsigned long Tempo::getTempoTotal()
{
	return ttotal;
}

inline unsigned long Tempo::getMilissegundos()
{
	return (clock()/(CLOCKS_PER_SEC/1000));
}
