#ifndef MAFILEH
#define MAFILEH


#include <coef.h>

class CModel;
class CEventKey;

typedef enum tag_mastate {//�t�@�C�����o�͗p
	MA_NONE,
	MA_FINDCHUNK,
	MA_TRUNK,
	MA_BRANCH,
	MA_FULENG,
	MA_EVENTKEY,
	MA_FINISH,
	MA_UNKCHUNK,
	MA_MAX
} MASTATE;

typedef struct tag_matrunk//�t�@�C�����o�͗p
{
	char motname[MAX_PATH];
	char filename[MAX_PATH];
	int idling;
	int ev0idle;
	int commonid;
	int forbidnum;
	int* forbidid;
	int notfu;

	void Init() {
		ZeroMemory(motname, sizeof(char) * MAX_PATH);
		ZeroMemory(filename, sizeof(char) * MAX_PATH);
		idling = 0;
		ev0idle = 0;
		commonid = 0;
		forbidnum = 0;
		forbidid = nullptr;
		notfu = 0;
	};
	tag_matrunk() {
		Init();
	};
} MATRUNK;

typedef struct tag_mabranch//�t�@�C�����o�͗p
{
	char motname[MAX_PATH];
	int eventno;
	int frameno1;
	int frameno2;
	int notfu;
	int nottoidle;

	void Init() {
		ZeroMemory(motname, sizeof(char) * MAX_PATH);
		eventno = 0;
		frameno1 = 0;
		frameno2 = 0;
		notfu = 0;
		nottoidle = 0;
	};
	tag_mabranch() {
		Init();
	};
} MABRANCH;


#define MALINELENG 10000

//class CPanda;

class CMAFile
{
public:
	CMAFile();
	~CMAFile();

	int SaveMAFile( char* srcfilename, CModel* srcmodel, HWND srchwnd, int overwriteflag );


//#ifdef INEASY3D
//	int LoadMAFile( char* srcfilename, HWND srchwnd, CHandlerSet* srchs, float mvmult );
//#else
	int LoadMAFile( char* srcfilename, CModel* srcmodel );
//#endif

	//int LoadMAFileFromPnd( CPanda* panda, int pndid, char* srcfilename, HWND srchwnd, CHandlerSet* srchs, float mvmult );


	int CheckIdlingMotion( CMCHandler* mch );
	int CheckSameMotionName();
	int CheckCommonid( HWND srchwnd );

private:
	int InitParams();
	int DestroyObjs();


	int WriteFULeng();
	int WriteEventKey();
	int WriteTrunk();
	int WriteBranch();

	int WriteMAHeader();
	int WriteLinechar( int addreturn );
///////////
	int SetBuffer( char* srcfilename );
	//int SetBuffer( CPanda* panda, int pndid, char* srcfilename );
	int GetLine( int* getlen );
	int CheckFileVersion();

	int FindChunk();
	int GetChunkType( char* chunkname, int nameleng );
	int SkipChunk();

	int ReadFULeng();
	int ReadEventKey();
	//int ReadTrunk( CPanda* panda = 0, int pndid = 0 );
	int ReadTrunk();
	int ReadBranch();
	int SetMATrunk( char* srcline, MATRUNK* srctrunk );
	int SetMABranch( char* srcline, MABRANCH* srcbranch );

	int GetName( char* dstchar, int dstleng, char* srcchar, int pos, int srcleng, int* stepnum );
	int GetInt( int* dstint, char* srcchar, int pos, int srcleng, int* stepnum );
	int GetFloat( float* dstfloat, char* srcchar, int pos, int srcleng, int* stepnum );

	int LoadMAFile_aft( char* srcfilename );

	CMCHandler* GetMotChangeHandler();
	CEventKey* GetEventKey();

private:
	int m_loadtrunknum;
	int m_moaversion;
	int m_fuleng;


	MASTATE m_state;
	MABUF mabuf;	
	char m_linechar[ MALINELENG ];

	HWND m_apphwnd;
	CModel* m_model;

	char m_mediadir[ MAX_PATH ];

	float m_quamult;
};

#endif

