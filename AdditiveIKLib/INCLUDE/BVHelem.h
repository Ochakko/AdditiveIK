#ifndef BVHELEMH
#define BVHELEMH

#include <coef.h>
//#include <D3DX9.h>
#include <ChaVecCalc.h>
#include <fbxsdk.h>

class CQuaternion;

class CBVHElem
{
public:
	CBVHElem();
	~CBVHElem();

	int SetName( char* srcname );
	int SetIsRoot( int srcisroot );
	int SetOffset( char* srcchar );
	int SetChanels( char* srcchar );

	int CreateMotionObjs( int srcframes );

	int SetMotionParams( int srcframeno, float* srcfloat );

	int DbgOutBVHElem( int srcdepth, int outmotionflag );
	int SetPosition();

	int Mult( float srcmult );
	int ConvertRotate2Q();
	int CheckNotAlNumName( char** ppdstname );

	int CalcDiffTra( int frameno, ChaVector3* pdifftra );
	//int ConvZxyRot();
	int ConvXYZRot();

private:
	int InitParams();
	int DestroyObjs();

	char* GetDigit( char* srcstr, int* dstint, int* dstsetflag );
	char* GetFloat( char* srcstr, float* dstfloat, int* dstsetflag );
	char* GetChanelType( char* srcstr, int* dstint, int* dstsetflag );

public:
	//accesser
	CBVHElem* GetParent(){
		return parent;
	};
	void SetParent( CBVHElem* srcval ){
		parent = srcval;
	};

	CBVHElem* GetChild(){
		return child;
	};
	void SetChild( CBVHElem* srcval ){
		child = srcval;
	};

	CBVHElem* GetBrother(){
		return brother;
	};
	void SetBrother( CBVHElem* srcval ){
		brother = srcval;
	};

	int GetChanelNum(){
		return chanelnum;
	};

	CQuaternion* GetTreeQ(){
		return treeq;
	};

	CQuaternion* GetQPtr(){
		return qptr;
	};
	ChaMatrix* GetTransMat(){
		return transmat;
	};

	int GetSameNameBoneSeri(){
		return samenameboneseri;
	};

	ChaVector3 GetPosition(){
		return position;
	};

	int GetFrameNum(){
		return framenum;
	}

	//ChaVector3* GetRotate(){
	//	return rotate;
	//};

	//float GetZxyRot(int srcframeno, int srcaxis)
	float GetXYZRot(int srcframeno, int srcaxis)
	{
		if ((srcframeno >= 0) && (srcframeno < framenum)){
			if ((srcaxis >= ROTAXIS_X) && (srcaxis <= ROTAXIS_Z)){
				switch (srcaxis){
				case ROTAXIS_X:
					return (xyzrot + srcframeno)->x;
					break;
				case ROTAXIS_Y:
					return (xyzrot + srcframeno)->y;
					break;
				case ROTAXIS_Z:
					return (xyzrot + srcframeno)->z;
					break;
				default:
					_ASSERT(0);
					return 0.0f;
					break;
				}
			}
			else{
				_ASSERT(0);
				return 0.0f;
			}
		}
		else{
			_ASSERT(0);
			return 0.0f;
		}
	};

	float GetRotate(int srcframeno, int srcaxis)
	{
		if ((srcframeno >= 0) && (srcframeno < framenum)){
			if ((srcaxis >= ROTAXIS_X) && (srcaxis <= ROTAXIS_Z)){
				switch (srcaxis){
				case ROTAXIS_X:
					return (rotate + srcframeno)->x;
					break;
				case ROTAXIS_Y:
					return (rotate + srcframeno)->y;
					break;
				case ROTAXIS_Z:
					return (rotate + srcframeno)->z;
					break;
				default:
					_ASSERT(0);
					return 0.0f;
					break;
				}
			}
			else{
				_ASSERT(0);
				return 0.0f;
			}
		}
		else{
			_ASSERT(0);
			return 0.0f;
		}
	};

	int GetTrans(int srcframeno, ChaVector3* dsttra)
	{
		if ((srcframeno >= 0) && (srcframeno < framenum)){
			*dsttra = *(trans + srcframeno);
		}
		else{
			return -1;
		}
		return 0;
	}

	const char* GetName(){
		return name;
	};

	int GetBoneNum(){
		return bonenum;
	};
	int GetBroNo(){
		return brono;
	};
	int SetBoneNum( int srcbonenum ){
		bonenum = srcbonenum;
		return 0;
	};
	int SetBroNo( int srcbrono ){
		brono = srcbrono;
		return 0;
	};

	EFbxRotationOrder GetFBXRotationOrder(){
		if (rotorder[0] == ROTAXIS_X){
			if (rotorder[1] == ROTAXIS_Y){
				//return eEulerXYZ;
				return eEulerZYX;
			}
			else if (rotorder[1] == ROTAXIS_Z){
				//return eEulerXZY;
				return eEulerYZX;
			}
			else{
				_ASSERT(0);
				return eEulerZXY;
			}
		}
		else if (rotorder[0] == ROTAXIS_Y){
			if (rotorder[1] == ROTAXIS_X){
				//return eEulerYXZ;
				return eEulerZXY;
			}
			else if (rotorder[1] == ROTAXIS_Z){
				//return eEulerYZX;
				return eEulerXZY;
			}
			else{
				_ASSERT(0);
				return eEulerZXY;
			}
		}
		else if (rotorder[0] == ROTAXIS_Z){
			if (rotorder[1] == ROTAXIS_X){
				//return eEulerZXY;
				return eEulerYXZ;
			}
			else if (rotorder[1] == ROTAXIS_Y){
				//return eEulerZYX;
				return eEulerXYZ;
			}
			else{
				_ASSERT(0);
				return eEulerZXY;
			}
		}
		else{
			_ASSERT(0);
			return eEulerZXY;
		}
	};
private:
	int isroot;

	int describeno;
	int serialno;
	int mqono;

	char name[ PATH_LENG ];
	ChaVector3 offset;
	int chanelnum;
	int chanels[ CHANEL_MAX ];

	int framenum;
	float frametime;

	ChaVector3* trans;
	ChaVector3* rotate;
	ChaVector3* xyzrot;
	CQuaternion* qptr;
	CQuaternion* transpose;//転置後のクォータニオン
	CQuaternion* treeq;//親の影響を考慮したクォータニオン。自分を含まず。

	ChaVector3* partransptr;
	ChaVector3 position;

	ChaMatrix* transmat;//グローバルの変換行列


	int samenameboneseri;//sigに、同じ名前のボーンがあった場合にselem->serialnoをセット、無かったら-1。

	int rotordercnt;
	int rotorder[ROTAXIS_MAX];

	CBVHElem* parent;
	CBVHElem* child;
	CBVHElem* brother;

	int bonenum;
	int brono;

};

#endif