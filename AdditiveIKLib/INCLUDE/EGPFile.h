#ifndef EGPFILEH
#define EGPFILEH

#include <fbxsdk.h>
#include <stdio.h>

class CBVHFile;

class CModel;

#ifdef EGPFILECPP
	char* g_egpbuf = 0;
	int CreateEGPBuf();
	void DestroyEGPBuf();
	int WriteEGPFile(CModel* pmodel, WCHAR* pfilename, char* fbxdate, int animno);
	bool LoadEGPFile(CModel* pmodel, WCHAR* pfilename, char* fbxdate, int animno);
#else
	extern char* g_egpbuf;
	extern int CreateEGPBuf();
	extern void DestroyEGPBuf();
	extern int WriteEGPFile(CModel* pmodel, WCHAR* pfilename, char* fbxdate, int animno);
	extern bool LoadEGPFile(CModel* pmodel, WCHAR* pfilename, char* fbxdate, int animno);
#endif

#endif

