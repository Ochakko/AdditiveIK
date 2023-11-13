#ifndef MQOFILEH
#define MQOFILEH

//class ID3D12Device;


//#include <d3dx9.h>
#include <ChaVecCalc.h>

#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#include <coef.h>

#define DBGH
#include <dbg.h>
#include <map>

//using namespace std;

//class ID3D12Device;

class CModel;
class CMQOMaterial;
class CMQOObject;
class CMQOFace;

#define NAMELENG	1024

typedef enum tag_mqostate {
	BEGIN_NONE,
	BEGIN_FINDCHUNK,
	BEGIN_TRIALNOISE,
	BEGIN_SCENE,
	BEGIN_BACKIMAGE,
	BEGIN_MATERIAL,
	BEGIN_OBJECT,//子チャンクをもつ
	BEGIN_VERTEX,
	BEGIN_VERTEXATTR,//子チャンクをもつ
	BEGIN_BVERTEX,//子チャンクをもつ
	BEGIN_VECTOR,
	BEGIN_WEIT,
	BEGIN_COLOR,
	BEGIN_FACE,
	BEGIN_BLOB,
	BEGIN_FINISH,
	BEGIN_UNKCHUNK,
	BEGIN_MAX
} MQOSTATE;

#define LINECHARLENG 2048

class CMQOFile
{
public:
	CMQOFile();
	~CMQOFile();

	int LoadMQOFile( ID3D12Device* pdev, float multiple, WCHAR* filename, 
		ChaVector3 offsetpos, ChaVector3 rot, 
		CModel* srcmodel );

private:
/// for load

	int LoadMQOFile_aft( float multiple, ChaVector3 offsetpos, ChaVector3 rot );
	
	void InitLoadParams();

	int SetBuffer();
	int CheckFileVersion();
	int FindChunk( MQOSTATE* nextstate );
	int GetChunkType( MQOSTATE* type, char* chunkname, int nameleng );
	int SkipChunk();

	int GetLine( int* getlen );
	int GetBytes( unsigned char** dstuc, int getlen );

	int ReadScene( MQOSTATE* nextstate );
	int ReadMaterial( MQOSTATE* nextstate );
	int ReadObject( MQOSTATE* nextstate );
	int ReadVertex( MQOSTATE* nextstate );
	int ReadFace( MQOSTATE* nextstate );
	int ReadBVertex( MQOSTATE* nextstate );
	int ReadColor( MQOSTATE* nextstate );

	int GetFloat( float* dstfloat, char* srcchar, int pos, int srcleng, int* stepnum );


/// for load
	//int MakeTree();
	//int start();
	//int begin();
	//int end();
	//int AddShape2Tree();

	//int make_first_part();
	//int end_first_part();
	//	
	//int do_transform_mat();
	//int IsJoint();

	int Multiple();
	//int PickUpAnchor();

private:
	CModel* m_modelptr;
	int m_objcnt;
	char m_linechar[ LINECHARLENG ];
	WCHAR m_wline[ LINECHARLENG ];

	ID3D12Device* m_pdev;

	MQOSTATE m_state;
	MQOBUF mqobuf;	

	CMQOObject* currentobj;

	int m_offset;
	int m_groundflag;
////
	int totalpoly;
	char tempname[NAMELENG + 1];

	float m_multiple;

	ChaMatrix m_offsetmat;

	ChaVector4 m_scene_ambient;
};


#endif
