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

#ifndef tempoH
#define tempoH

#define EVE_TEMPO_1S	0x0001
#define EVE_TEMPO_100MS	0x0002

class Tempo
{
public:
	Tempo();
    Tempo(unsigned long quaisEventos);
    void reset();
	unsigned long loop();
//TODO:    unsigned long trataEvento();
    unsigned long getFrames();
    unsigned long getFPS();
    unsigned long getFPSMedio();
    unsigned long getTempoTotal();
protected:
	unsigned long logarEventos;
	unsigned long ttotal;
	unsigned long ultimo,agora;
    unsigned long T1ds;
	unsigned long frames;
	unsigned long fpstemp,fps;
private:
	unsigned long getMilissegundos();
};
#endif
