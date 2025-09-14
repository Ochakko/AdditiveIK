#ifndef REFILEH
#define REFILEH

//#include <d3dx9.h>
#include <ChaVecCalc.h>
#include <coef.h>
#include <XMLIO.h>

#include <unordered_map>
#include <string>

class CModel;
class CMQOObject;
class CBone;
class CRigidElem;

class CRigidElemFile : public CXMLIO
{
public:
	CRigidElemFile();
	virtual ~CRigidElemFile();

	int WriteRigidElemFile( WCHAR* strpath, CModel* srcmodel, int reindex );
	int LoadRigidElemFile( WCHAR* strpath, CModel* srcmodel );
//	int LoadRigidElemFileFromPnd( CPmCipherDll* cipher, int qubindex, CModel* srcmodel, int* newid );

private:
	virtual int InitParams();
	virtual int DestroyObjs();

	void WriteREReq( CBone* srcbone );
	int WriteRE( CBone* srcbone );

	int ReadBone( XMLIOBUF* xmliobuf );
	int ReadRE( XMLIOBUF* xmliobuf, CBone* srcbone );

//	int ReadMorphBase( XMLIOBUF* xmliobuf );
//	int ReadMorphKey( XMLIOBUF* xmliobuf, CMQOObject* srcbase );
//	int ReadBlendWeight( XMLIOBUF* xmliobuf, CMQOObject* setbase, CMorphKey* setmk );

public:
	std::string m_rename;
	float m_btgscale;

private:
	CModel* m_model;

};

#endif