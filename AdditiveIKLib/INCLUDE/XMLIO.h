#ifndef XMLIOFILEH
#define XMLIOFILEH

//#include <d3dx9.h>
#include <ChaVecCalc.h>
#include <coef.h>

class CQuaternion;
class CModel;
class CBone;

class CXMLIO
{
public:
	CXMLIO();
	virtual ~CXMLIO();

protected:
	virtual int InitParams();
	virtual int DestroyObjs();

	int Write2File( const char* lpFormat, ... );
	int WriteVoid2File( void* pvoid, unsigned int srcleng );


	int SetBuffer();
//	int SetBuffer( CPmCipherDll* cipher, int dataindex );

	int Read_Int( XMLIOBUF* xmliobuf, const char* startpat, const char* endpat, int* dstint );
	int Read_Float( XMLIOBUF* xmliobuf, const char* startpat, const char* endpat, float* dstfloat );
	int Read_Vec3( XMLIOBUF* xmliobuf, const char* startpat, const char* endpat, ChaVector3* dstvec );
	int Read_Q( XMLIOBUF* xmliobuf, const char* startpat, const char* endpat, CQuaternion* dstq );
	int Read_Matrix(XMLIOBUF* xmliobuf, const char* startpat, const char* endpat, ChaMatrix* dstmat);
	int Read_Str( XMLIOBUF* xmliobuf, const char* startpat, const char* endpat, char* dststr, int arrayleng );

	int GetInt( int* dstint, char* srcchar, int pos, int srcleng, int* stepnum );
	int GetFloat( float* dstfloat, char* srcchar, int pos, int srcleng, int* stepnum );
	int GetName( char* dstchar, int dstleng, const char* srcchar, int pos, int srcleng );

	int SetXmlIOBuf( XMLIOBUF* srcbuf, const char* startpat, const char* endpat, XMLIOBUF* dstbuf, int delpatflag = 1 );
	
	CBone* FindBoneByName(CModel* srcmodel, char* bonename, int srcleng);

protected:
	int m_mode;
	HANDLE m_hfile;
	XMLIOBUF m_xmliobuf;

};

#endif