/*
  Name: Vetores
  Copyright: Todos direitos reservados - 2005
  Author: Mauricio Bieze Stefani
  Date: 27/09/05 20:51
  Description: 
*/

#ifndef geometriaH
#define geometriaH

#include <ostream>

typedef float Real;
const Real PI=3.14159265;

class Vetor2D
{
public:
	Real x, y;
	Vetor2D(Real xx=0, Real yy=0) : x(xx), y(yy) {}
};

class Vetor3D
{
public:
	Real x,y,z;

    Vetor3D(Real xx=0, Real yy=0, Real zz=0) : x(xx), y(yy), z(zz) {}

    Vetor3D operator+(const Vetor3D& v) const;
    Vetor3D operator-(const Vetor3D& v) const;
    Vetor3D operator*(Real r) const;			//escala
    Vetor3D operator*(const Vetor3D& v) const;	//produto vetorial

    Vetor3D& operator+=(const Vetor3D& v);
    Vetor3D& operator-=(const Vetor3D& v);
    Vetor3D& operator*=(Real r);			//escala
	Vetor3D& operator*=(const Vetor3D& v);	//produto vetorial
	
	bool operator==(const Vetor3D& v) const { return ((x==v.x)&(y==v.y)&(z==v.z)); }
	bool operator!=(const Vetor3D& v) const { return ((x!=v.x)|(y!=v.y)|(z!=v.z)); }

	Real norma() const;
	Vetor3D unit() const;
	Vetor3D& normalizar();
    
    Real dist(const Vetor3D& v) const { return (*this-v).norma(); }
    Real dot(const Vetor3D& v) const { return (x*v.x + y*v.y + z*v.z); }

    Vetor3D gira(const Vetor3D& ang) const;			//angulos em graus
    Vetor3D gira(Real ax, Real ay, Real az) const;	//angulos em graus
    
	Real angulo(const Vetor3D& v) const;
	Real cosseno(const Vetor3D& v) const;
};
std::ostream& operator<<(std::ostream& os, const Vetor3D& v);

class Vetor4D : public Vetor3D
{
public:
	Real w;
	Vetor4D(Real xx=0, Real yy=0, Real zz=0, Real ww=1) : Vetor3D(xx,yy,zz), w(ww) {}
    Vetor4D(const Vetor3D& base, Real ww=1) : Vetor3D(base), w(ww) {}
};

class Plano
{
public:
    Vetor3D normal;
	Real d; //distancia da origem -> Ax + By + Cz + D = 0
};

#endif
