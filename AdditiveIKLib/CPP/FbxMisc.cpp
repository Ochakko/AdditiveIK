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

#include <Bone.h>

#define DBGH
#include <dbg.h>

#define FBXMISCCPP
#include <FbxMisc.h>

#include <GlobalVar.h>


using namespace std;


bool IsNullAndChildIsCameraNode(FbxNode* pNode)
{
	if (!pNode) {
		return false;
	}

	FbxNodeAttribute* pAttrib = pNode->GetNodeAttribute();
	if (pAttrib) {
		FbxNodeAttribute::EType type = (FbxNodeAttribute::EType)(pAttrib->GetAttributeType());
		if (type == FbxNodeAttribute::eNull) {

			int childNodeNum;
			childNodeNum = pNode->GetChildCount();
			for (int i = 0; i < childNodeNum; i++)
			{
				FbxNode* pChild = pNode->GetChild(i);  // 子ノードを取得
				if (pChild) {
					FbxNodeAttribute* pAttrib2 = pChild->GetNodeAttribute();
					if (pAttrib2) {
						FbxNodeAttribute::EType type2 = (FbxNodeAttribute::EType)(pAttrib2->GetAttributeType());
						if (type2 == FbxNodeAttribute::eCamera) {
							return true;//!!!!!!!!!!
						}
					}
				}
			}
		}
		else {
			return false;
		}
	}
	else {
		//attrib無しもeNullとする

		int childNodeNum;
		childNodeNum = pNode->GetChildCount();
		for (int i = 0; i < childNodeNum; i++)
		{
			FbxNode* pChild = pNode->GetChild(i);  // 子ノードを取得
			if (pChild) {
				FbxNodeAttribute* pAttrib2 = pChild->GetNodeAttribute();
				if (pAttrib2) {
					FbxNodeAttribute::EType type2 = (FbxNodeAttribute::EType)(pAttrib2->GetAttributeType());
					if (type2 == FbxNodeAttribute::eCamera) {
						return true;//!!!!!!!!!!
					}
				}
			}
		}
	}

	return false;
}

int IntRotationOrder(EFbxRotationOrder srcorder)
{
	int retorder = ROTORDER_XYZ;

	switch (srcorder) {
	case eEulerXYZ:
		retorder = ROTORDER_XYZ;
		break;
	case eEulerXZY:
		retorder = ROTORDER_XZY;
		break;
	case eEulerYZX:
		retorder = ROTORDER_YZX;
		break;
	case eEulerYXZ:
		retorder = ROTORDER_YXZ;
		break;
	case eEulerZXY:
		retorder = ROTORDER_ZXY;
		break;
	case eEulerZYX:
		retorder = ROTORDER_ZYX;
		break;
	case eSphericXYZ:
		retorder = ROTORDER_XYZ;
		break;
	default:
		_ASSERT(0);
		retorder = ROTORDER_XYZ;
		break;
	}

	return retorder;
}


//srcnameの最後の.以降の文字列をdstnameに格納
int TrimBlendShapeName(const char* srcname, char* dstname, int dstlen)
{
	if (!srcname || !dstname || (dstlen <= 0)) {
		_ASSERT(0);
		return 1;
	}
	if (*srcname <= 0) {
		_ASSERT(0);
		return 1;
	}
	if (dstlen > 1024) {
		_ASSERT(0);
		return 1;
	}


	int offsettargetname = 0;
	char outname[1024];
	ZeroMemory(outname, sizeof(char) * 1024);
	strcpy_s(outname, 1024, srcname);
	const char* dottargetname = strrchr(outname, '.');
	if (dottargetname) {
		int tmpoffset = min(1023, max(0, (int)(dottargetname - outname + 1)));
		if (outname[tmpoffset] != 0) {
			offsettargetname = tmpoffset;
		}
		else {//.が最後の文字の場合
			offsettargetname = 0;
		}
	}
	strcpy_s(dstname, dstlen, (outname + offsettargetname));

	return 0;
}
