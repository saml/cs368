#ifndef GRAPHICUTILS_CPP
#define GRAPHICUTILS_CPP
//graphicUtils.cpp
//stores functions and data types used in opengl project
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;
namespace slee17 {
const double MYPI = 3.14159265;

static double precision(double d, int p) {//returns truncated double at position p
    //p should NOT be negative integer
    int divisor = 1;
    for( int i=0; i<p; i++ ) {
        divisor *= 10;
    }//for
    d *= divisor;
    d = (int)ceil(d);
    //cout<<d<<"/"<<divisor<<" = "<<d/divisor<<endl;
    return d/divisor;
}//precision

static bool equalDouble( double d1, double d2, double offset ) {
	//returns true if d1 and d2 are no more than offset different
	return ( abs(d1-d2) <= offset  );
}//equalDouble

struct rgba {//red green blue alpha
    double red; double green; double blue; double alpha;
    rgba() { red = 0.0; green = 0.0; blue = 0.0; alpha = 0.0; }//default
    rgba( double r, double g, double b, double a ) {
        red = r; green = g; blue = b; alpha = a;
    }//constructor
};//rgba

struct xyz {//a vector in 3d space
    double x; double y; double z;
    xyz() { x = 0.0; y = 0.0; z = 0.0; }//default constructor
    xyz( double xValue, double yValue, double zValue ) {
        x = xValue; y = yValue; z = zValue;
    }//constructor
    void normalize() {
        double magnitude = sqrt( x*x + y*y + z*z );
        x /= magnitude;
        y /= magnitude;
        z /= magnitude;
    }//normalize
    static double dotProduct( const xyz& vec1, const xyz& vec2 ) {
        return (vec1.x*vec2.x + vec1.y*vec2.y + vec1.z*vec2.z);
    }//dotProduct
    static xyz crossProduct( const xyz& vec1, const xyz& vec2 ) {
        return xyz( (vec1.y*vec2.z - vec1.z*vec2.y)
                    , (vec1.z*vec2.x - vec1.x*vec2.z), (vec1.x*vec2.y - vec1.y*vec2.x) );
    }//crossProduct
	bool operator ==( const xyz& rHand ) const {
		/*
		return ( equalDouble(x, rHand.x, 0.000005)
			&& equalDouble(y,rHand.y, 0.000005)
			&& equalDouble(z,rHand.z,0.000005) );
		*/
		
		return ( precision(x,2) == precision(rHand.x,2)
			&& precision(y,2) == precision(rHand.y,2)
			&& precision(z,2) == precision(rHand.z,2) );
		
	}//==
	bool operator <=( const xyz& rHand ) const {
		/*
		return ( equalDouble(x, rHand.x, 0.000005)
			&& equalDouble(y,rHand.y, 0.000005)
			&& equalDouble(z,rHand.z,0.000005) );
		*/
		
		return ( precision(x,2) <= precision(rHand.x,2)
			&& precision(y,2) <= precision(rHand.y,2)
			&& precision(z,2) <= precision(rHand.z,2) );
		
	}//==
	xyz operator *( const xyz& rHand ) const {
		return xyz( x*rHand.x, y*rHand.y, z*rHand.z );
	}//*
	xyz& operator +=( const xyz& rHand ) {
		x += rHand.x;
		y += rHand.y;
		z += rHand.z;
		return *this;
	}//+=
    xyz operator -( const xyz& rHand ) const {
        return xyz( x - rHand.x, y - rHand.y, z - rHand.z );
    }//-
    xyz operator -() const {
        return xyz( -x, -y, -z );
    }//-
    void operator =( const xyz& rHand ) {
        x = rHand.x; y = rHand.y; z = rHand.z;
    }//=
    friend ostream& operator <<(ostream& os, const xyz& p) {
        //prints out x y z
        os<<p.x<<" "<<p.y<<" "<<p.z<<" ";
        return os;
    }//<<
    friend istream& operator >>(istream& is, xyz& p) {
        //input order: x y z
        is>>p.x>>p.y>>p.z;
        return is;
    }//>>
};//xyz

struct point {//a point has its location and normal
    //normal of a point is always normalized...
    xyz where;//where the point is located
    xyz normal;//surface normal of the point
    point() {
        where = xyz(); normal = xyz();
    }//default constructor
    point( const xyz& l, const xyz& n ) {
        where = l;//location
        normal = n;
        //normal.normalize();//normalizes normal by default..
    }//constructor
    point( const point& cp ) {
        where = cp.where;
        normal = cp.normal;
        //normal.normalize();
    }//copy constructor
	bool operator ==( const point& rHand ) const {
		return ( where == rHand.where && normal == rHand.normal );
	}//==
    friend ostream& operator <<(ostream& os, const point& p) {
        //prints out x y z Nx Ny Nz
        os<<p.where.x<<" "<<p.where.y<<" "<<p.where.z<<" "
        <<p.normal.x<<" "<<p.normal.y<<" "<<p.normal.z<<" ";
        return os;
    }//<<
    friend istream& operator >>(istream& is, point& p) {
        //input order: x y z Nx Ny Nz
        is>>p.where.x>>p.where.y>>p.where.z
        >>p.normal.x>>p.normal.y>>p.normal.z;
        p.normal.normalize();
        return is;
    }//>>
};//point

struct triangle {//three points make up a triangle
    point a, b, c;
    triangle() { a = point(); b = point(); c = point(); }//default constructor
    triangle(const point& first, const point& second, const point& third) {
        a = first; b = second; c = third;
    }//constructor
    triangle( const triangle& cp ) {
        a = cp.a; b = cp.b; c = cp.c;
    }//copy constructor

	bool operator ==( const triangle& rHand ) const {
		return ( a == rHand.a && b == rHand.b && c == rHand.c );
	}//==
    //note: for << and >> operators, 'normal' field of point is disregarded.
    friend ostream& operator <<(ostream& os, const triangle& t) {
        //prints out Ax Ay Az Bx By Bz Cx Cy Cz
        os<<t.a.where<<" "<<t.b.where<<" "<<t.c.where<<" ";
        return os;
    }//<<
    friend istream& operator >>(istream& is, triangle& t) {
        //the input order should be Ax Ay Az Bx By Bz Cx Cy Cz
        is>>(t.a).where.x>>(t.a).where.y>>(t.a).where.z
        >>(t.b).where.x>>(t.b).where.y>>(t.b).where.z
        >>(t.c).where.x>>(t.c).where.y>>(t.c).where.z;
        return is;
    }//>>
    xyz aveN() const {
        //returns average or three point's normal
        return xyz( (a.normal.x + b.normal.x + c.normal.x)/3,
                    (a.normal.y + b.normal.y + c.normal.y)/3,
                    (a.normal.z + b.normal.z + c.normal.z)/3 );
    }//aveN
    xyz triN() const {
        //returns of triangle's calculated normal
        //(c-a crossproduct b-a)
        //assumming the triangle is formed a->b->c clockwise and the view
        //point is at the origin.
        return -(xyz::crossProduct( c.where-a.where, b.where-a.where ));
    }//triN
};//triangle

struct perspectiveViewVolume {
    double angle;
    double aspect;//x to y
    double close;//along z axis
    double distant;//along z axis
    perspectiveViewVolume() {
        angle = 45.0;
        aspect = 1.0;
        close = 0.0;
        distant = 100.0;
    }//default constructor
    perspectiveViewVolume(double a, double as, double n, double f) {
        angle = a;
        aspect = as;
        close = n;
        distant = f;
    }//default constructor
};//perspectiveViewVolume

struct orthographicViewVolume {
    double left, right, bottom, top, close, distant;
    orthographicViewVolume() {
        left = -1.0;//x
        right = 1.0;
        bottom = -1.0;//y
        top = -1.0;
        close = 0.0;//z
        distant = -100.0;
    }//default constructor
    orthographicViewVolume(double l, double r, double b, double t,
                           double n, double f) {
        left = l;//x
        right = r;
        bottom = b;//y
        top = t;
        close = n;//z
        distant = f;
    }//constructor
};//orthographicViewVolume

//------------------------- matrix -------------------------
template<typename T>
struct matrix {
    vector< vector<T> > data_;

    matrix( unsigned r, unsigned c ) : data_(r) {
		for( unsigned i=0; i<r; ++i ) {
			data_[i].resize(c);
		}//for
    }//constructor

	unsigned getRow() const {
		return data_.size();
	}//getRow
	unsigned getCol() const {
		return data_[0].size();
	}//getCol

	T& operator()(unsigned r, unsigned c) {
		return data_[r][c];
	}//[][]
	const T& operator()(unsigned r, unsigned c) const {
		return data_[r][c];
	}//[][]

	const matrix<T> operator *(const matrix<T>& m) const {
		if( getCol() != m.getRow() )
			return matrix<T>(0, 0);
		matrix<T> result( getRow(), m.getCol() );
		for( unsigned row=0; row<result.getRow(); ++row ) {
			for( unsigned col=0; col<result.getCol(); ++col ) {
				for( unsigned i=0; i<getCol(); ++i ) {//this.column
					result.data_[row][col] +=
						( data_[row][i] * m.data_[i][col] );
				}//for
			}//for
		}//for
		return result;
	}//*

    friend ostream& operator<<(ostream& os, const matrix<T>& t) {
        for( unsigned i=0; i<t.getRow(); ++i ){
            for( unsigned j=0; j<t.getCol(); ++j ) {
                os<<t.data_[i][j]<<"\t";
            }//for
            os<<endl;
        }//for
        return os;
    }//<<
};//matrix

}//slee17
#endif

