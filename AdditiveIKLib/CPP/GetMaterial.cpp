#include "stdafx.h"
//#include <stdafx.h> //É_É~Å[

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <windows.h>

#define	DBGH
#include <dbg.h>

#include <crtdbg.h>

#include <GetMaterial.h>

//#include <mqomaterial.h>
//#include <map>

#include <algorithm>
#include <iostream>
#include <iterator>


CMQOMaterial* GetMaterialFromNo(std::map<int, CMQOMaterial*>& srcmaterial, int matno)
{
	CMQOMaterial* retmat = 0;

	std::map<int,CMQOMaterial*>::iterator itr= srcmaterial.find(matno); 
	if( itr == srcmaterial.end() ) { 
		return 0;
	}else{ 
		return (CMQOMaterial*)itr->second;
	} 
}

