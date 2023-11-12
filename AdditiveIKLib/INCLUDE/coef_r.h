#ifndef COEFRH
#define COEFRH

#include <coef.h>

typedef struct RDBPoint
{
public:
	float x,y,z;

	// constructor
	inline RDBPoint() { x = 0.0f; y = 0.0f; z = 0.0f; }
	inline RDBPoint(float nx, float ny, float nz) {x=nx; y=ny; z=nz;}

	// operator
	inline RDBPoint& operator = (const RDBPoint& p)
		{x=p.x; y=p.y; z=p.z; return *this;}
	inline RDBPoint& operator = (const float& s)
		{x=y=z=s; return *this;}
	inline RDBPoint& operator += (const RDBPoint& p)
		{x+=p.x; y+=p.y; z+=p.z; return *this;}
	inline RDBPoint& operator += (const float& s)
		{x+=s; y+=s; z+=s; return *this;}
	inline RDBPoint& operator -= (const RDBPoint& p)
		{x-=p.x; y-=p.y; z-=p.z; return *this;}
	inline RDBPoint& operator -= (const float& s)
		{x-=s; y-=s; z-=s; return *this;}
	inline RDBPoint& operator *= (const RDBPoint& p)
		{x*=p.x; y*=p.y; z*=p.z; return *this;}
	inline RDBPoint& operator *= (float s)
		{x*=s; y*=s; z*=s; return *this;}
	inline RDBPoint& operator /= (float s)
		{x/=s; y/=s; z/=s; return *this;}
	friend inline RDBPoint operator + (const RDBPoint& p1, const RDBPoint& p2)
		{return RDBPoint(p1.x+p2.x, p1.y+p2.y, p1.z+p2.z);}
	friend inline RDBPoint operator + (const RDBPoint& p, const float& s)
		{return RDBPoint(p.x+s, p.y+s, p.z+s);}
	friend inline RDBPoint operator + (const float& s, const RDBPoint& p)
		{return RDBPoint(p.x+s, p.y+s, p.z+s);}
	friend inline RDBPoint operator - (const RDBPoint& p1, const RDBPoint& p2)
		{return RDBPoint(p1.x-p2.x, p1.y-p2.y, p1.z-p2.z);}
	friend inline RDBPoint operator - (const RDBPoint& p, const float& s)
		{return RDBPoint(p.x-s, p.y-s, p.z-s);}
	friend inline RDBPoint operator - (const float& s, const RDBPoint& p)
		{return RDBPoint(s-p.x, s-p.y, s-p.z);}
	friend inline RDBPoint operator * (const RDBPoint& p1, const RDBPoint& p2)
		{return RDBPoint(p1.x*p2.x, p1.y*p2.y, p1.z*p2.z);}
	friend inline RDBPoint operator * (const RDBPoint& p, const float& s)
		{return RDBPoint(p.x*s, p.y*s, p.z*s);}
	friend inline RDBPoint operator * (const float& s, const RDBPoint& p)
		{return RDBPoint(p.x*s, p.y*s, p.z*s);}
	friend inline RDBPoint operator / (const RDBPoint& p, const float& s)
		{return RDBPoint(p.x/s, p.y/s, p.z/s);}
	friend inline bool operator == (const RDBPoint& p1, const RDBPoint& p2)
		{return (p1.x==p2.x && p1.y==p2.y && p1.z==p2.z);}
	friend inline bool operator != (const RDBPoint& p1, const RDBPoint& p2)
		{return (p1.x!=p2.x || p1.y!=p2.y || p1.z!=p2.z);}

	inline void zero() { x=y=z=0.0f; }
	inline void one() { x=y=z=1.0f; }
	inline float norm() const { return x*x + y*y + z*z; }
	inline float abs() const { return (float)sqrt(norm()); }
	inline void normalize() {
		float a = abs();
		if(a > 0.0f) *this /= a;
		else zero();
	}
} RDBPoint;

typedef struct RDBMatrix
{
public:
	float _11, _12, _13, _14;
	float _21, _22, _23, _24;
	float _31, _32, _33, _34;
	float _41, _42, _43, _44;

	// constructor
	inline RDBMatrix(){
		_11 = 1.0f; _12 = 0.0f; _13 = 0.0f; _14 = 0.0f;
		_21 = 0.0f; _22 = 1.0f; _23 = 0.0f; _24 = 0.0f;
		_31 = 0.0f; _32 = 0.0f; _33 = 1.0f; _34 = 0.0f;
		_41 = 0.0f; _42 = 0.0f; _43 = 0.0f; _44 = 1.0f;
	}
	
	inline RDBMatrix& operator = (const RDBMatrix& m)
	{
		_11 = m._11; _12 = m._12; _13 = m._13; _14 = m._14;
		_21 = m._21; _22 = m._22; _23 = m._23; _24 = m._24;
		_31 = m._31; _32 = m._32; _33 = m._33; _34 = m._34;
		_41 = m._41; _42 = m._42; _43 = m._43; _44 = m._44;
		return *this;
	}

} RDBMatrix;

typedef struct RDBQuaternion
{
public:
	float x, y, z, w;
	
	inline RDBQuaternion(){
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
		w = 1.0f;
	}

	inline RDBQuaternion& operator = (const RDBQuaternion& q)
	{
		x = q.x;
		y = q.y;
		z = q.z;
		w = q.w;
		return *this;
	}

} RDBQuaternion;



#endif