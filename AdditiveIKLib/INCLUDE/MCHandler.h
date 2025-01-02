#ifndef MCHANDLERH
#define MCHANDLERH

#include <coef.h>

class CModel;

class CMCHandler
{
public:
	CMCHandler(CModel* srcparemtmodel, int srcfillupleng );
	~CMCHandler();

	//int InitMCArray(int srcidlingid);
	int GetTotalMCNum();

	int Setno2MotID( int srcsetno );
	int MotID2Setno( int srcmotid );
	MCELEM* Setno2MCElem( int srcsetno, MCELEM** parentmce );


	int AddChild( int parentsetno, int srccookie, int srcalways, int srcframeno1, int srcframeno2, int srceventno1, int srcnotfu, int srcnottoidle );
	int SetMCElem( int srcsetno, int srccookie, int srcalways, int srcframeno1, int srcframeno2, int srceventno1, int srcnotfu, int srcidling, int srcnottoidle );
	int DeleteMCElem( int srcsetno );

	int DestroyObjs();

//	int AddParentMC( int addcookie, int srcidling, int srcev0idle, int srccommonid, int srcforbidcommonid );
	int AddParentMC( int addcookie, int srcidling, int srcev0idle, int srccommonid, int srcforbidnum, int* srcforbidid, int srcnotfu );


	int SetIdlingMotion(CModel* srcmodel, int exceptfirst);
	int ChangeIdlingMotion(CModel* srcmodel, int srcmotid);
	int GetIdlingMotID(char* dstname, int dstleng);

	int GetNextMotion( int srccurmotid, int srccurframe, int srceventno, int* nextmotidptr, int* nextframeptr, int* notfuptr, int* nottoidleptr );
	int GetEv0Idle( int srcmotid, int* dstev0idle );


	int GetTrunkInfo( int motid, int* infoptr );
	int GetTrunkInfo( int motid, MOATRUNKINFO* trunkptr );
	int GetTrunkNotComID( int motid, int arrayleng, int* dstid, int* getnum );
	int GetBranchInfo( int motid, int* infoptr, int branchnum, int* getnum );
	int GetBranchInfo( int motid, MOABRANCHINFO* branchptr, int branchnum, int* getnum );
	int SetBranchFrame1( int tmotid, int bmotid, int frame1 );
	int SetBranchFrame2( int tmotid, int bmotid, int frame2 );
	int SetTrunkNotComID( int motid, int arrayleng, int* srcid );


public:
	int GetMCNum() {
		return m_mcnum;
	};
	MCELEM* GetMCElem(int mcindex) {
		if ((mcindex < 0) || (mcindex >= m_mcnum)) {
			_ASSERT(0);
			return nullptr;
		}

		if (m_mcarray) {
			return (m_mcarray + mcindex);
		}
		else {
			_ASSERT(0);
			return nullptr;
		}
	};
	void SetFillUpLeng(int srcval) {
		m_fillupleng = srcval;
	};
	int GetFillUpLeng() {
		return m_fillupleng;
	};

private:
	int InitParams();
	void ReorderSetno();

	int DeleteChild( MCELEM* parmce, MCELEM* chilmce );

	int QSortChild( MCELEM* parmce );

private:
	//CMotHandler* m_mh;
	CModel* m_parentmodel;

	int m_mcnum;
	MCELEM* m_mcarray;
	
	int m_fillupleng;
	//int m_totalmcnum;
};

#endif