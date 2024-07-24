#ifndef STRUCTHISTORYH
#define STRUCTHISTORYH


#include <Windows.h>

#include <ChaVecCalc.h>

#include <vector>
#include <map>
#include <string>


#define HISTORYCOMMENTLEN	32

typedef struct tag_cpinfo
{
	WCHAR fbxname[MAX_PATH];
	WCHAR motionname[MAX_PATH];
	double startframe;
	double framenum;
	int bvhtype;//0:undef, 1-144:bvh1 - bvh144, -1:bvh_other
	int importance;//0:undef, 1:tiny, 2:alittle, 3:normal, 4:noticed, 5:imortant, 6:very important
	WCHAR comment[HISTORYCOMMENTLEN];//WCHAR * 31文字まで。３２文字目は終端記号


	bool operator== (const tag_cpinfo& chk) const {
		if ((wcscmp(fbxname, chk.fbxname) == 0) &&
			(wcscmp(motionname, chk.motionname) == 0) &&
			(startframe == chk.startframe) && 
			(framenum == chk.framenum) && 
			(bvhtype == chk.bvhtype) && 
			(importance == chk.importance)// &&
			//(wcscmp(comment, chk.comment) == 0)
		) {
			return true;
		}
		else {
			return false;
		}
	};


	void Init() {
		ZeroMemory(fbxname, sizeof(WCHAR) * MAX_PATH);
		ZeroMemory(motionname, sizeof(WCHAR) * MAX_PATH);
		startframe = 0.0;
		framenum = 0.0;
		bvhtype = 0;
		importance = 0;
		ZeroMemory(comment, sizeof(WCHAR) * HISTORYCOMMENTLEN);
	};

	tag_cpinfo() {
		Init();
	};
}CPMOTINFO;

typedef struct tag_historyelem
{
	FILETIME filetime;
	WCHAR wfilename[MAX_PATH];
	int hascpinfo;//cpinfoにセットした場合1, セットしていない場合0
	CPMOTINFO cpinfo;

	bool operator< (const tag_historyelem& right) const {
		LONG lRet = CompareFileTime(&filetime, &(right.filetime));
		if (lRet < 0) {
			return true;
		}
		else if (lRet > 0) {
			return false;
		}
		else {
			return (std::wstring(wfilename) < std::wstring(right.wfilename));
		}
	};
	bool operator== (const tag_historyelem& chk) const { 
		if ((wcscmp(wfilename, chk.wfilename) == 0) && 
			(filetime.dwHighDateTime == chk.filetime.dwHighDateTime) &&
			(filetime.dwLowDateTime == chk.filetime.dwLowDateTime) &&
			(cpinfo == chk.cpinfo)
		) {
			return true;
		}
		else {
			return false;
		}
	};


	void Init() {
		ZeroMemory(&filetime, sizeof(FILETIME));
		ZeroMemory(wfilename, sizeof(WCHAR) * MAX_PATH);
		hascpinfo = 0;
		cpinfo.Init();
	};

	tag_historyelem() {
		Init();
	};
}HISTORYELEM;

typedef struct tag_dollyelem
{
	FILETIME filetime;
	WCHAR wfilename[MAX_PATH];

	bool validflag;
	ChaVector3 camerapos;
	ChaVector3 cameratarget;
	WCHAR comment[HISTORYCOMMENTLEN];//WCHAR * 31文字まで。３２文字目は終端記号

	bool operator< (const tag_dollyelem& right) const {
		LONG lRet = CompareFileTime(&filetime, &(right.filetime));
		if (lRet < 0) {
			return true;
		}
		else if (lRet > 0) {
			return false;
		}
		else {
			return (std::wstring(wfilename) < std::wstring(right.wfilename));
		}
	};

	void Init() {
		ZeroMemory(&filetime, sizeof(FILETIME));
		ZeroMemory(wfilename, sizeof(WCHAR) * MAX_PATH);
		ZeroMemory(comment, sizeof(WCHAR) * HISTORYCOMMENTLEN);

		validflag = false;
		camerapos.SetParams(0.0f, 0.0f, 10.0f);
		cameratarget.SetParams(0.0f, 0.0f, 0.0f);
	};

	tag_dollyelem() {
		Init();
	};
}DOLLYELEM;


#endif