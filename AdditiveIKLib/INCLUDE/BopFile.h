#ifndef BopFileH
#define BopFileH

//#include <d3dx9.h>
#include <coef.h>
#include <XMLIO.h>

#include <map>

class CModel;
class CMQOObject;
class CBone;
class CMotionPoint;
class CQuaternion;
class CPmCipherDll;
class CMorphKey;

class CBopFile : public CXMLIO
{
public:
	CBopFile();
	virtual ~CBopFile();

	int LoadBopFile( WCHAR* strpath, CModel* srcmodel );
private:
	virtual int InitParams();
	virtual int DestroyObjs();

//	int WriteFileInfo();
//	void WriteBoneReq( CBone* srcbone, int broflag );
//	int WriteMorphBase( CMQOObject* srcbase );

//	int CheckFileVersion( XMLIOBUF* xmliobuf );
//	int ReadMotionInfo( WCHAR* wfilename, XMLIOBUF* xmliobuf );
	int ReadBopElem( XMLIOBUF* xmliobuf );
//	int ReadMotionPoint( XMLIOBUF* xmliobuf, CBone* srcbone );

//	int ReadMorphBase( XMLIOBUF* xmliobuf );
//	int ReadMorphKey( XMLIOBUF* xmliobuf, CMQOObject* srcbase );
//	int ReadBlendWeight( XMLIOBUF* xmliobuf, CMQOObject* setbase, CMorphKey* setmk );

public:
	int m_fileversion;
	CModel* m_model;
	CBone* m_topbone;
	int m_bonenum;
	int m_basenum;
	int m_motid;
	MOTINFO* m_motinfo;
};

#endif