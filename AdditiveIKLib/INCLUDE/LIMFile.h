#ifndef LIMFILEH
#define LIMFILEH

#include <Windows.h>

class CModel;

#ifdef LIMFILECPP
	int CreateLIMBuf();
	void DestroyLIMBuf();
	int WriteLIMFile(CModel* pmodel, WCHAR* pfilename, char* fbxdate, int srcmotid, int animno);
	bool LoadLIMFile(CModel* pmodel, WCHAR* pfilename, char* fbxdate, int animno, int* newmotid);
#else
	extern int CreateLIMBuf();
	extern void DestroyLIMBuf();
	extern int WriteLIMFile(CModel* pmodel, WCHAR* pfilename, char* fbxdate, int srcmotid, int animno);
	extern bool LoadLIMFile(CModel* pmodel, WCHAR* pfilename, char* fbxdate, int animno, int* newmotid);
#endif

#endif

