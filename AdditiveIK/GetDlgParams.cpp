#include "stdafx.h"
//#include "stdafx.h"

//#define NOTWINDOWSH

#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

#include <coef.h>
#include <crtdbg.h>

#include "GetDlgParams.h"

#define DBGH
#include <dbg.h>

/***
int GetText( CWindow srcwnd, char* dstptr, int maxleng )
{
	int textleng;
	textleng = srcwnd.GetWindowTextLength();


	if( textleng == 0 ){
		*dstptr = 0;
		return 0;
	}

	if( textleng >= maxleng )
		return 1;

	srcwnd.GetWindowText( dstptr, textleng + 1 );

	return 0;
}
***/
int GetTextW( CWindow srcwnd, WCHAR* dstptr, int maxnum )
{
	int textleng;
	textleng = srcwnd.GetWindowTextLength();


	if( textleng == 0 ){
		*dstptr = 0;
		return 0;
	}

	if( textleng >= maxnum )
		return 1;

	srcwnd.GetWindowText( dstptr, textleng + 1 );

	return 0;
}

/***
int GetName( CWindow srcwnd, char* dstptr )
{
	int textleng;
	textleng = srcwnd.GetWindowTextLength();
	if( textleng >= _MAX_PATH )
		return 1;

	if( textleng == 0 ){
		*dstptr = 0;
		return 0;
	}

	srcwnd.GetWindowText( dstptr, textleng + 1 );

	return 0;
}
***/
/***
int GetFloat( CWindow srcwnd, float* dstptr )
{
	int textleng;
	textleng = srcwnd.GetWindowTextLength();
	if( (textleng == 0) || (textleng >= 50) )
		return 1;

	char tempchar[50];
	ZeroMemory( tempchar, 50 );
	srcwnd.GetWindowText( tempchar, textleng + 1 );


	*dstptr = (float)atof( tempchar );

	return 0;

}

int GetDouble( CWindow srcwnd, double* dstptr )
{
	int textleng;
	textleng = srcwnd.GetWindowTextLength();
	if( (textleng == 0) || (textleng >= 50) )
		return 1;

	char tempchar[50];
	ZeroMemory( tempchar, 50 );
	srcwnd.GetWindowText( tempchar, textleng + 1 );

	*dstptr = atof( tempchar );

	return 0;
}

int GetInt( CWindow srcwnd, int* dstptr )
{
	int textleng;
	textleng = srcwnd.GetWindowTextLength();
	if( (textleng == 0) || (textleng >= 50) )
		return 1;

	char tempchar[50];
	ZeroMemory( tempchar, 50 );
	srcwnd.GetWindowText( tempchar, textleng + 1 );
	
	*dstptr = atoi( tempchar );

	return 0;
}
***/



