#ifndef BVHFILEH
#define BVHFILEH

#include <coef.h>
//#include <quaternion.h>
//#include <basedat.h>
//#include <matrix2.h>
//#include "motionpoint2.h"



class CBVHElem;
class CQuaternion;

//class CTreeHandler2;
//class CShdHandler;
//class CShdElem;
//class CMotHandler;
//class CMotionCtrl;

#ifndef BVHLINELENG
#define BVHLINELENG	20000
#endif

class CBVHFile
{
public:
	CBVHFile();
	~CBVHFile();

	int LoadBVHFile( HWND apphwnd, WCHAR* srcname, float srcmult );


private:
	int InitParams();
	int DestroyObjs();
	int SetBuffer( WCHAR* filename );
	
	int CheckFileHeader();
	int GetState();

	int CreateNewElem();

	int LoadOffset();
	int LoadChanels();
	int BeginBrace();
	int EndBrace();
	int LoadFrames();
	int LoadFrameTime();
	int LoadMotionParams();

	int GetLine();

	int GetStateAndLine();

	char* GetDigit( char* srcstr, int* dstint, int* dstsetflag );
	char* GetFloat( char* srcstr, float* dstfloat, int* dstsetflag );

	void DbgOutBVHElemReq( CBVHElem* outbe, int srcdepth );
	void SetBVHElemPositionReq( CBVHElem* srcbe );
	void CountJointNumReq( CBVHElem* srcbe, int* jointnumptr, int* bonenumptr );
	void MultBVHElemReq( CBVHElem* srcbe, float srcmult );

	void CalcBVHTreeQReq( CBVHElem* srcbe );
	void CalcTransMatReq(CBVHElem* srcbe);

	int SetBVHSameNameBoneSeri();
	int CalcMPQ( CBVHElem* beptr, int fno, CQuaternion* dstq );
/////////////
//	void FindFirstJointReq( CShdElem* selem, CShdElem** ppfirstjoint );
//	void WriteBVHTreeReq( CShdElem* selem, int* pwriteno, int depth, int broflag, int* perrorflag, int* wno2seri );
	//int WriteBVHMotion( int* wno2seri, int motid, float srcmult );

	int Write2File( char* lpFormat, ... );
	int WriteTab( int tabnum );

public:
	//accesser
	CBVHElem* GetBVHElem(){
		return m_behead;
	};
	int GetJointNum()
	{
		return m_jointnum;
	}

private:
	//fileëÄçÏóp
	HANDLE m_hfile;
	char* m_buf;
	DWORD m_pos;
	DWORD m_bufleng;
	int m_state;


	HANDLE m_hwfile;//èoóÕóp
	int m_writenum;

	int m_motcookie;

	CBVHElem* m_behead;
	CBVHElem* m_curbe;
	CBVHElem* m_parbe;

	char m_linechar[ BVHLINELENG ];

	int m_frames;
	float m_frametime;


	int m_motframenum;//bvhÇÃÉtÉåÅ[ÉÄî‘çÜÇÇQî{Ç…ÇµÇƒäiî[Ç∑ÇÈÅB


	int			m_benum;
	CBVHElem** m_bearray;

	float m_tempparam[ CHANEL_MAX ];
	HWND m_appwnd;

	int m_jointnum;
	int m_bonenum;
};


#endif