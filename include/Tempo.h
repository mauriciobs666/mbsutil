#ifndef tempoH
#define tempoH

#define EVE_TEMPO_1S	0x0001
#define EVE_TEMPO_100MS	0x0002

class Tempo
{
public:
	Tempo();
    Tempo(unsigned long quaisEventos);
    void loop();
    void reset();
    unsigned long trataEvento();
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
