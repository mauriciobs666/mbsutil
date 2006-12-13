/*
	Copyright 2006 - Mauricio Bieze Stefani
*/

#include "Geometria.h"
#include <math.h>

using namespace std;

/*
	Vetor2i
*/

Vetor2i Vetor2i::operator+(const Vetor2i& v) const
{
	Vetor2i n=*this;
	n.x+=v.x;
	n.y+=v.y;
	return n;
}

Vetor2i Vetor2i::operator-(const Vetor2i& v) const
{
	Vetor2i n=*this;
	n.x-=v.x;
	n.y-=v.y;
	return n;
}

Vetor2i Vetor2i::operator*(int i) const
{
	Vetor2i n=*this;
	n.x*=i;
	n.y*=i;
	return n;
}

Vetor2i Vetor2i::operator/(int i) const
{
	Vetor2i n=*this;
	n.x/=i;
	n.y/=i;
	return n;
}

Vetor2i& Vetor2i::operator+=(const Vetor2i& v)
{
	x+=v.x;
	y+=v.y;
	return *this;
}

Vetor2i& Vetor2i::operator-=(const Vetor2i& v)
{
	x-=v.x;
	y-=v.y;
	return *this;
}

Vetor2i& Vetor2i::operator*=(int i)
{
	x*=i;
	y*=i;
	return *this;
}

Vetor2i& Vetor2i::operator/=(int i)
{
	x/=i;
	y/=i;
	return *this;
}

/*
	Vetor3D
*/

Vetor3D Vetor3D::operator+(const Vetor3D& v) const
{
	Vetor3D n=*this;
	n.x+=v.x;
	n.y+=v.y;
	n.z+=v.z;
	return n;
}

Vetor3D Vetor3D::operator-(const Vetor3D& v) const
{
	Vetor3D n=*this;
	n.x-=v.x;
	n.y-=v.y;
	n.z-=v.z;
	return n;
}

Vetor3D Vetor3D::operator*(Real r) const
{
	Vetor3D n=*this;
	n.x*=r;
	n.y*=r;
	n.z*=r;
	return n;
}

Vetor3D Vetor3D::operator*(const Vetor3D& v) const
{
	Vetor3D n;
	n.x= y*v.z - v.y*z;
	n.y=-x*v.z + v.x*z;
	n.z= x*v.y - y*v.x;
	return n;
}

Vetor3D& Vetor3D::operator+=(const Vetor3D& v)
{
	x+=v.x;
	y+=v.y;
	z+=v.z;
	return *this;
}

Vetor3D& Vetor3D::operator-=(const Vetor3D& v)
{
	x-=v.x;
	y-=v.y;
	z-=v.z;
	return *this;
}

Vetor3D& Vetor3D::operator*=(Real r)
{
    x*=r;
    y*=r;
    z*=r;
	return *this;
}

Vetor3D& Vetor3D::operator*=(const Vetor3D& v)
{
	Vetor3D n=*this;
	*this=n*v;
	return *this;
}

Real Vetor3D::norma() const
{
	return sqrt(x*x+y*y+z*z);
}

Vetor3D Vetor3D::unit() const
{
	Vetor3D n=*this;
	return n.normalizar();
}

Vetor3D& Vetor3D::normalizar()
{
	Real n=norma();
	x/=n;
	y/=n;
	z/=n;
	return *this;
}

Vetor3D Vetor3D::gira(const Vetor3D& ang) const
{
	return gira(ang.x,ang.y,ang.z);
}

Vetor3D Vetor3D::gira(Real ax, Real ay, Real az) const
{
	Vetor3D n=*this;
	Real xtmp,ztmp;

	ax=(ax*PI)/180;
	ay=(ay*PI)/180;
	az=(az*PI)/180;

	Real senx=sin(ax);
	Real cosx=cos(ax);
	Real seny=sin(ay);
	Real cosy=cos(ay);
	Real senz=sin(az);
	Real cosz=cos(az);

	//rotacao em torno do eixo X
	n.y=y*cosx-z*senx;
	ztmp=n.z=z*cosx+y*senx;

	//rotacao em torno do eixo Y
	n.z=n.z*cosy-x*seny;
	xtmp=n.x=x*cosy+ztmp*seny;

	//rotacao em torno do eixo  Z
	n.x=n.x*cosz-n.y*senz;
	n.y=n.y*cosz+xtmp*senz;

	return n;
}

// TODO: Testar Vetor3D::angulo e cos p/ vetores nao normalizados
Real Vetor3D::angulo(const Vetor3D& v) const
{
	return acos(cosseno(v));
}

Real Vetor3D::cosseno(const Vetor3D& v) const
{
	return (this->dot(v))/(norma()*v.norma());
}

ostream& operator<<(ostream& os, const Vetor3D& v)
{
	return os << "(" << v.x << "," << v.y << "," << v.z << ")";
}
