#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>

#define LIMFILECPP
#include <LIMFile.h>

#define	DBGH
#include <dbg.h>

#include <crtdbg.h>
#include <coef.h>

#include <Model.h>
#include <Bone.h>
#include <MotionPoint.h>

#include <GlobalVar.h>

#include <unordered_map>
using namespace std;

typedef struct tag_limaniminfo
{
	int motid;
	int orgindex;
	int maxframe;
	char* engmotname;
	//FbxAnimLayer* animlayer;
}LIMANIMINFO;




typedef struct tag_limheader
{
	char magicstr[32];//EvaluateGlobalPosition
	char version[16];
	char fbxdate[256];
	int animno;
	int jointnum;
	int framenum;
	int reserved;
}LIMHEADER;

typedef struct tag_limjointheader
{
	int jointindex;
	int framenum;
	char jointname[256];
}LIMJOINTHEADER;

typedef struct tag_limelem
{
	int jointindex;
	int frameno;
	ChaMatrix limitedworld;
}LIMELEM;

static CModel* s_model = 0;

static int WriteLIMFileHeader(LIMHEADER* dstegph, HANDLE file, CModel* srcmodel, 
	char* fbxdate, int srcmotid, int animno);
static bool ValidateLIMFile(LIMHEADER* dstegph, char* srcbuf, DWORD bufleng, CModel* pmodel, 
	char* fbxdate, int srcmotid, int srcanimno);


int WriteLIMFile(CModel* pmodel, WCHAR* pfilename, char* fbxdate, int srcmotid, int animno)
{
	s_model = pmodel;

	CallF( pmodel->MakeEnglishName(), return 1 );

	WCHAR outfilename[MAX_PATH] = { 0L };
	swprintf_s(outfilename, MAX_PATH, L"%s_anim%d.lif", pfilename, animno);


	HANDLE hfile = CreateFile(outfilename, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (hfile == INVALID_HANDLE_VALUE) {
		DbgOut(L"LIMFile : WriteLIMFile : file open error !!!\n");
		_ASSERT(0);
		return 1;
	}

	
	SetEndOfFile(hfile);//すでにファイルが存在していた場合にまずは０サイズにする



	LIMHEADER limheader;
	::ZeroMemory(&limheader, sizeof(LIMHEADER));

	int result;
	result = WriteLIMFileHeader(&limheader, hfile, pmodel, fbxdate, srcmotid, animno);
	if (result != 0) {
		_ASSERT(0);
		return 1;
	}
	
	int jointindex;
	int frameno;
	for (jointindex = 0; jointindex < limheader.jointnum; jointindex++) {
		CBone* curbone = pmodel->GetBoneByZeroBaseIndex(jointindex);
		if (!curbone) {
			_ASSERT(0);
			return 1;
		}

		LIMJOINTHEADER jointheader;
		::ZeroMemory(&jointheader, sizeof(LIMJOINTHEADER));
		jointheader.jointindex = jointindex;
		jointheader.framenum = limheader.framenum;
		strcpy_s(jointheader.jointname, 256, curbone->GetBoneName());
		DWORD writesize1 = 0;
		WriteFile(hfile, &jointheader, sizeof(LIMJOINTHEADER), &writesize1, NULL);
		if (writesize1 != sizeof(LIMJOINTHEADER)) {
			_ASSERT(0);
			return 1;
		}

		for (frameno = 0; frameno < limheader.framenum; frameno++) {
			CMotionPoint* curmp = curbone->GetMotionPoint(srcmotid, (double)frameno);
			if (curmp) {
				ChaMatrix limitedwm;
				limitedwm = curmp->GetLimitedWM();

				LIMELEM limelem;
				::ZeroMemory(&limelem, sizeof(LIMELEM));
				limelem.jointindex = jointindex;
				limelem.frameno = frameno;
				limelem.limitedworld = limitedwm;
				DWORD writesize2 = 0;
				WriteFile(hfile, &limelem, sizeof(LIMELEM), &writesize2, NULL);
				if (writesize2 != sizeof(LIMELEM)) {
					_ASSERT(0);
					return 1;
				}
			}
			else {
				_ASSERT(0);
				return 1;
			}
		}
	}

	FlushFileBuffers(hfile);
	SetEndOfFile(hfile);


	CloseHandle(hfile);


	return 0;
}

int WriteLIMFileHeader(LIMHEADER* dstlimh, HANDLE hfile, CModel* srcmodel, 
	char* fbxdate, int srcmotid, int animno)
{
	if ((hfile == INVALID_HANDLE_VALUE) || !srcmodel || !fbxdate || !dstlimh) {
		_ASSERT(0);
		return 1;	
	}
	s_model = srcmodel;

	MOTINFO curmi = srcmodel->GetMotInfo(srcmotid);
	if (curmi.motid <= 0) {
		_ASSERT(0);
		return 1;
	}

	int framenum;
	framenum = IntTime(curmi.frameleng);
	if (framenum <= 0) {
		_ASSERT(0);
		return 1;
	}


	int jointnum;
	jointnum = srcmodel->GetBoneListSize();
	if (jointnum <= 0) {
		_ASSERT(0);
		return 1;
	}

	::ZeroMemory(dstlimh, sizeof(LIMHEADER));

	strcpy_s(dstlimh->magicstr, 32, "LimitedWorldMat");
	strcpy_s(dstlimh->version, 16, "0.0.0.1");//ver0001 : 2023/02/06
	strcpy_s(dstlimh->fbxdate, 256, fbxdate);
	dstlimh->animno = animno;
	dstlimh->jointnum = jointnum;
	dstlimh->framenum = framenum;
	dstlimh->reserved = 0;

	DWORD wleng = 0;
	WriteFile(hfile, dstlimh, sizeof(LIMHEADER), &wleng, NULL);
	if (wleng != sizeof(LIMHEADER)) {
		return 1;
	}

	return 0;
}



bool ValidateLIMFile(LIMHEADER* dstlimh, char* srcbuf, DWORD bufleng, CModel* pmodel, 
	char* fbxdate, int srcanimno)
{
	if (!dstlimh || !srcbuf || (bufleng <= 0) || !pmodel) {
		return false;
	}

	if (bufleng <= sizeof(LIMHEADER)) {
		return false;
	}

	::MoveMemory(dstlimh, srcbuf, sizeof(LIMHEADER));

	int magicstrlen;
	dstlimh->magicstr[32 - 1] = 0;
	magicstrlen = (int)strlen(dstlimh->magicstr);
	if ((magicstrlen <= 0) || (magicstrlen >= 32)) {
		return false;
	}
	int cmp;
	cmp = strcmp(dstlimh->magicstr, "LimitedWorldMat");
	if (cmp != 0) {
		return false;
	}

	int versionstrlen;
	dstlimh->version[16 - 1] = 0;
	versionstrlen = (int)strlen(dstlimh->version);
	if ((versionstrlen <= 0) || (versionstrlen >= 16)) {
		return false;
	}
	int cmp2;
	cmp2 = strcmp(dstlimh->version, "0.0.0.1");//ver0001 : 2023/02/06
	if (cmp2 != 0) {
		return false;
	}
	
	int fbxdatelen;
	dstlimh->fbxdate[256 - 1] = 0;
	fbxdatelen = (int)strlen(dstlimh->fbxdate);
	if ((fbxdatelen <= 0) || (fbxdatelen >= 256)) {
		return false;
	}
	int cmp3;
	cmp3 = strcmp(dstlimh->fbxdate, fbxdate);
	if (cmp3 != 0) {
		return false;
	}


	//MOTINFO* curmi;
	//curmi = pmodel->GetMotInfo(srcmotid);
	//if (!curmi) {
	//	return false;
	//}
	//int framenum;
	//framenum = (int)curmi->frameleng;
	//if (framenum <= 0) {
	//	return false;
	//}

	int jointnum;
	jointnum = pmodel->GetBoneListSize();
	if (jointnum <= 0) {
		return false;
	}


	if (dstlimh->animno != srcanimno) {
		return false;
	}
	//if (dstlimh->framenum != framenum) {
	//	return false;
	//}
	if (dstlimh->framenum <= 1) {
		return false;
	}
	if (dstlimh->reserved != 0) {
		return false;
	}

	size_t buffersize = 0;
	buffersize = sizeof(LIMHEADER) +
		(size_t)jointnum * sizeof(LIMJOINTHEADER) +
		(size_t)jointnum * (size_t)dstlimh->framenum * sizeof(LIMELEM);

	if ((bufleng < 0) || (bufleng != buffersize)) {
		return false;
	}

	return true;
}


bool LoadLIMFile(CModel* pmodel, WCHAR* pfilename, char* fbxdate, int animno, int* newmotid)
{
	if (!pmodel || !pfilename || !fbxdate || !newmotid) {
		_ASSERT(0);
		return false;
	}
	*newmotid = -1;


	WCHAR infilename[MAX_PATH] = { 0L };
	swprintf_s(infilename, MAX_PATH, L"%s_anim%d.lif", pfilename, animno);
	char motionname[MAX_PATH] = { 0 };
	sprintf_s(motionname, MAX_PATH, "anim%d", animno);

	HANDLE hfile;
	hfile = CreateFile(infilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (hfile == INVALID_HANDLE_VALUE) {
		//_ASSERT(0);
		return false;
	}


	DWORD sizehigh;
	DWORD bufleng;
	bufleng = GetFileSize(hfile, &sizehigh);
	if (bufleng <= 0) {
		_ASSERT(0);
		return false;
	}
	if (sizehigh != 0) {
		_ASSERT(0);
		return false;
	}


	char* newbuf;
	newbuf = (char*)malloc(sizeof(char) * ((size_t)bufleng + 1));//bufleng + 1
	if (!newbuf) {
		_ASSERT(0);
		return false;
	}

	::ZeroMemory(newbuf, sizeof(char) * (bufleng + 1));
	DWORD rleng, readleng;
	rleng = bufleng;
	BOOL bsuccess;
	bsuccess = ReadFile(hfile, (void*)newbuf, rleng, &readleng, NULL);
	if (!bsuccess || (rleng != readleng)) {
		_ASSERT(0);
		CloseHandle(hfile);
		free(newbuf);
		return false;
	}

	LIMHEADER limheader;
	::ZeroMemory(&limheader, sizeof(limheader));
	bool isvalid;
	isvalid = ValidateLIMFile(&limheader, newbuf, bufleng, pmodel, fbxdate, animno);
	if (!isvalid) {
		//_ASSERT(0);
		CloseHandle(hfile);
		free(newbuf);
		return false;
	}

	int motid = -1;
	int result1 = pmodel->AddMotion(motionname, infilename, (double)limheader.framenum, &motid);
	if (result1 != 0) {
		_ASSERT(0);
		CloseHandle(hfile);
		free(newbuf);
		return false;
	}
	*newmotid = motid;

	MOTINFO newmi = pmodel->GetMotInfo(motid);
	if (newmi.motid <= 0) {
		_ASSERT(0);
		CloseHandle(hfile);
		free(newbuf);
		return false;
	}
	if (IntTime(newmi.frameleng) != limheader.framenum) {
		_ASSERT(0);
		CloseHandle(hfile);
		free(newbuf);
		return false;
	}

	unsigned int jointindex;
	unsigned int frameno;
	for (jointindex = 0; jointindex < (unsigned int)limheader.jointnum; jointindex++) {
		size_t curheaderpos;
		curheaderpos = sizeof(LIMHEADER) + (size_t)jointindex * sizeof(LIMJOINTHEADER) + 
			(size_t)jointindex * (size_t)limheader.framenum * sizeof(LIMELEM);
		if (curheaderpos >= ((size_t)bufleng - sizeof(LIMJOINTHEADER))) {
			CloseHandle(hfile);
			free(newbuf);
			return false;
		}
		LIMJOINTHEADER jointheader;
		::ZeroMemory(&jointheader, sizeof(LIMJOINTHEADER));
		::MoveMemory(&jointheader, newbuf + curheaderpos, sizeof(LIMJOINTHEADER));

		if (jointheader.jointindex != jointindex) {
			CloseHandle(hfile);
			free(newbuf);
			return false;
		}

		int jointnamelen;
		jointheader.jointname[256 - 1] = 0;
		jointnamelen = (int)strlen(jointheader.jointname);
		if ((jointnamelen <= 0) || (jointnamelen >= 256)) {
			CloseHandle(hfile);
			free(newbuf);
			return false;
		}
		CBone* curbone = 0;
		curbone = pmodel->FindBoneByName(jointheader.jointname);
		if (!curbone) {
			CloseHandle(hfile);
			free(newbuf);
			return false;
		}
		
		for (frameno = 0; frameno < (unsigned int)limheader.framenum; frameno++) {
			size_t limbufpos;
			limbufpos = curheaderpos + sizeof(LIMJOINTHEADER) + frameno * sizeof(LIMELEM);
			if ((limbufpos <= 0) || (limbufpos > ((size_t)bufleng - sizeof(LIMELEM)))) {
				CloseHandle(hfile);
				free(newbuf);
				return false;
			}

			LIMELEM limelem;
			::ZeroMemory(&limelem, sizeof(LIMELEM));
			::MoveMemory(&limelem, newbuf + limbufpos, sizeof(LIMELEM));

			if (limelem.jointindex != jointindex) {
				CloseHandle(hfile);
				free(newbuf);
				return false;
			}

			if (limelem.frameno != frameno) {
				CloseHandle(hfile);
				free(newbuf);
				return false;
			}

			ChaMatrix curwm;
			curwm = limelem.limitedworld;

			//limitdegflag == trueで　SetWorldMat
			bool limitdegflag = true;//limitedworldmatにセット
			curbone->SetWorldMat(limitdegflag, motid, (double)frameno, curwm, 0);
			
		}

	}

	CloseHandle(hfile);

	return true;
}
