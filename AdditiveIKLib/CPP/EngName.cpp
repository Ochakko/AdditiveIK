#include "stdafx.h"
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>
#include <crtdbg.h>

#define ENGNAMECPP
#include <EngName.h>

#define DBGH
#include <dbg.h>

int TermJointRepeats(char* dstname)
{

	//ジョイント名の後ろに付けていた_Jointという文字列が複数ある場合１つで終端
	//現在は_Jointという文字列を付加しないので　あまり意味がなくなった

	*(dstname + 256 - 1) = 0;

	char pat1[256] = "_Joint";
	char* pat1ptr = strstr(dstname, pat1);
	if (pat1ptr){
		char* pat2ptr = strstr(pat1ptr + strlen(pat1), pat1);
		if (pat2ptr){
			*pat2ptr = 0;
		}
	}
	return 0;
}


int ConvEngName( int type, char* srcname, int srcleng, char* dstname, int dstleng )
{

	//この関数は現在も呼び出しているが　CBone::GetEngBoneName()を保存時に使用しなくなったので　あまり意味が無い状態
	//ノードの名前には最初から英数字を使う前提に変更


	static char headname[256] = "ReplacedName";
	static int replaceno = 0;

	char namepm[256];
	char tempname[256];
	char tempprinta[1024];
	WCHAR wtempprinta[1024];

	//文字列長は２５６を想定。
	if( (srcleng > 256) || (dstleng < 256) ){
		_ASSERT( 0 );
		return 1;
	}
	*(srcname + 256 - 1) = 0;
	*(dstname + 256 - 1) = 0;


	strcpy_s( namepm, 256, srcname );


	char* lptr;
	lptr = strstr( namepm, "[L]" );
	if( lptr ){
		int restleng = 256 - (int)(lptr - namepm);
		strncpy_s( lptr, restleng, "_L_ ", 3 );
	}
	char* rptr;
	rptr = strstr( namepm, "[R]" );
	if( rptr ){
		int restleng = 256 - (int)(rptr - namepm);
		strncpy_s( rptr, restleng,  "_R_", 3 );
	}


	int findilleagal = 0;
	size_t namelen = strlen( srcname );
	if ((namelen > 0) && (namelen < dstleng)) {
		size_t nameno;
		int alnum;
		char curc;
		for (nameno = 0; nameno < namelen; nameno++) {
			curc = namepm[nameno];
			if (((curc >= 'a') && (curc <= 'z')) || ((curc >= 'A') && (curc <= 'Z')) || ((curc >= '0') && (curc <= '9')) ||
				(curc == '_') || 
				(curc == '[') || (curc == ']')//2025/02/08 for [L], [R]
				) {
				alnum = 1;
			}
			else {
				alnum = 0;
			}

			if (alnum == 0) {
				findilleagal = 1;
				break;
			}
		}
	}
	else {
		_ASSERT(0);
		findilleagal = 1;
	}

	if( findilleagal != 0 ){
		if( type == ENGNAME_BONE ){
			char pat1[256] = "_Joint";
			char* patptr = strstr(headname, pat1);
			if (!patptr){
				sprintf_s(tempname, 256, "%s%d_Joint", headname, replaceno);
			}
			else{
				sprintf_s(tempname, 256, "%s%d", headname, replaceno);
			}
		}else if( type == ENGNAME_DISP ){
			sprintf_s( tempname, 256, "%s%d", headname, replaceno );
		}else if( type == ENGNAME_MOTION ){
			sprintf_s(tempname, 256, "Motion_%d", replaceno);
		}else{
			_ASSERT( 0 );
			return 1;
		}
			
		sprintf_s( tempprinta, 1024, "Xファイル用パーツ名変更\r\n\t%s ---> %s\r\n", srcname, tempname );
		MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, tempprinta, 1024, wtempprinta, 1024 );
		DbgOut( wtempprinta );

		strcpy_s( dstname, dstleng, tempname );
		replaceno++;

	}else{
		if( type == ENGNAME_BONE ){
			char* strjoint = strstr( dstname, "_Joint" );
			if( !strjoint ){
				sprintf_s( dstname, dstleng, "%s_Joint", namepm );
			}else{
				strcpy_s( dstname, dstleng, namepm );
			}
		}else{
			strcpy_s( dstname, dstleng, namepm );
		}
	}

	*(dstname + 256 - 1) = 0;
	TermJointRepeats(dstname);


	return 0;
}
