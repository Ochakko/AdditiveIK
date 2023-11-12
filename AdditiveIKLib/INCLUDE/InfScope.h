#ifndef INFSCOPEH
#define INFSCOPEH

#include <coef.h>
//#include <d3dx9.h>

class CMQOObject;
class CMQOFace;
class CMQOMaterial;
class CBone;

class CInfScope
{
public:
	CInfScope();
	~CInfScope();

	int CalcCenter();
	int CheckInCnt( ChaVector3* vptr, int* incntptr, int* justptr );

private:
	int InitParams();
	int DestroyObjs();

public:
	int m_serialno;
	int m_materialno;
	int m_validflag;

	int m_facenum;
	CMQOFace** m_ppface;

	int m_vnum;
	ChaVector3* m_pvert;

	int m_applyboneno;
	CMQOObject* m_targetobj;
	CBone* m_tmpappbone;

	ChaVector3 m_minv;
	ChaVector3 m_maxv;
	ChaVector3 m_center;

	int m_settminmax;
	ChaVector3 m_tminv;
	ChaVector3 m_tmaxv;
	ChaVector3 m_tcenter;
	float m_tmaxdist;

};

#endif