#ifndef BLENDSHAPEELEMH
#define BLENDSHAPEELEMH

#include <ChaVecCalc.h>
#include <coef.h>

#include <vector>
#include <string>
#include <unordered_map>

class CModel;
class CMQOObject;

class CBlendShapeElem
{
public:
	CBlendShapeElem();
	~CBlendShapeElem();
	int SetBlendShape(CModel* srcmodel, CMQOObject* srcmqoobj, int srcchannelindex);

	void Init();

	CBlendShapeElem operator= (CBlendShapeElem m);

public:
	bool validflag;
	CModel* model;
	CMQOObject* mqoobj;
	WCHAR targetname[256];
	int channelindex;
};

#endif