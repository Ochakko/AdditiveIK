#ifndef MBPLUGINH
#define MBPLUGINH


#ifndef MBPLUGIN_VERSION
#define MBPLUGIN_VERSION	1009
#endif


#define MBPLUGIN_EXPORT extern "C" __declspec(dllexport)
#define MBPLUGIN_API	extern "C" 

#include <math.h>


typedef struct MBPoint
{
public:
	float x,y,z;

	// constructor
	inline MBPoint() { }
	inline MBPoint(float nx, float ny, float nz) {x=nx; y=ny; z=nz;}

	// operator
	inline MBPoint& operator = (const MBPoint& p)
		{x=p.x; y=p.y; z=p.z; return *this;}
	inline MBPoint& operator = (const float& s)
		{x=y=z=s; return *this;}
	inline MBPoint& operator += (const MBPoint& p)
		{x+=p.x; y+=p.y; z+=p.z; return *this;}
	inline MBPoint& operator += (const float& s)
		{x+=s; y+=s; z+=s; return *this;}
	inline MBPoint& operator -= (const MBPoint& p)
		{x-=p.x; y-=p.y; z-=p.z; return *this;}
	inline MBPoint& operator -= (const float& s)
		{x-=s; y-=s; z-=s; return *this;}
	inline MBPoint& operator *= (const MBPoint& p)
		{x*=p.x; y*=p.y; z*=p.z; return *this;}
	inline MBPoint& operator *= (float s)
		{x*=s; y*=s; z*=s; return *this;}
	inline MBPoint& operator /= (float s)
		{x/=s; y/=s; z/=s; return *this;}
	friend inline MBPoint operator + (const MBPoint& p1, const MBPoint& p2)
		{return MBPoint(p1.x+p2.x, p1.y+p2.y, p1.z+p2.z);}
	friend inline MBPoint operator + (const MBPoint& p, const float& s)
		{return MBPoint(p.x+s, p.y+s, p.z+s);}
	friend inline MBPoint operator + (const float& s, const MBPoint& p)
		{return MBPoint(p.x+s, p.y+s, p.z+s);}
	friend inline MBPoint operator - (const MBPoint& p1, const MBPoint& p2)
		{return MBPoint(p1.x-p2.x, p1.y-p2.y, p1.z-p2.z);}
	friend inline MBPoint operator - (const MBPoint& p, const float& s)
		{return MBPoint(p.x-s, p.y-s, p.z-s);}
	friend inline MBPoint operator - (const float& s, const MBPoint& p)
		{return MBPoint(s-p.x, s-p.y, s-p.z);}
	friend inline MBPoint operator * (const MBPoint& p1, const MBPoint& p2)
		{return MBPoint(p1.x*p2.x, p1.y*p2.y, p1.z*p2.z);}
	friend inline MBPoint operator * (const MBPoint& p, const float& s)
		{return MBPoint(p.x*s, p.y*s, p.z*s);}
	friend inline MBPoint operator * (const float& s, const MBPoint& p)
		{return MBPoint(p.x*s, p.y*s, p.z*s);}
	friend inline MBPoint operator / (const MBPoint& p, const float& s)
		{return MBPoint(p.x/s, p.y/s, p.z/s);}
	friend inline bool operator == (const MBPoint& p1, const MBPoint& p2)
		{return (p1.x==p2.x && p1.y==p2.y && p1.z==p2.z);}
	friend inline bool operator != (const MBPoint& p1, const MBPoint& p2)
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
} MBPoint;

typedef struct MBMatrix
{
public:
	float _11, _12, _13, _14;
	float _21, _22, _23, _24;
	float _31, _32, _33, _34;
	float _41, _42, _43, _44;

	// constructor
	inline MBMatrix(){
		_11 = 1.0f; _12 = 0.0f; _13 = 0.0f; _14 = 0.0f;
		_21 = 0.0f; _22 = 1.0f; _23 = 0.0f; _24 = 0.0f;
		_31 = 0.0f; _32 = 0.0f; _33 = 1.0f; _34 = 0.0f;
		_41 = 0.0f; _42 = 0.0f; _43 = 0.0f; _44 = 1.0f;
	}
	
	inline MBMatrix& operator = (const MBMatrix& m)
	{
		_11 = m._11; _12 = m._12; _13 = m._13; _14 = m._14;
		_21 = m._21; _22 = m._22; _23 = m._23; _24 = m._24;
		_31 = m._31; _32 = m._32; _33 = m._33; _34 = m._34;
		_41 = m._41; _42 = m._42; _43 = m._43; _44 = m._44;
		return *this;
	}

} MBMatrix;

typedef struct MBQuaternion
{
public:
	float x, y, z, w;
	
	inline MBQuaternion(){
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
		w = 1.0f;
	}

	inline MBQuaternion& operator = (const MBQuaternion& q)
	{
		x = q.x;
		y = q.y;
		z = q.z;
		w = q.w;
		return *this;
	}

} MBQuaternion;

#ifndef USERCOEFH
typedef struct MBColor3f
{
public:
	float r;
	float g;
	float b;
} MBColor3f;
#endif


#ifndef MBPLUGIN_EXPORTS
//Version
MBPLUGIN_API int (*MBGetVersion)( int* verptr );
MBPLUGIN_API int (*MBCreateMotionBrush)( double srcstartframe, double srcendframe, float* dstvalue);

#endif


#endif