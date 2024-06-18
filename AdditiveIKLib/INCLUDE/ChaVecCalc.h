#ifndef ChaCalcH
#define ChaCalcH

#include "../../MiniEngine/MiniEngine.h"
#include <DirectXMath.h>

class CQuaternion;
class btMatrix3x3;
class btVector3;
class ChaVector3;
class CBone;
class CMotionPoint;
class CEditRange;

//struct D3DXMATRIX;
//struct D3DXVECTOR2;
//struct D3DXVECTOR3;
//struct D3DXVECTOR4;
//struct D3DXQUATERNION;



#include <coef.h>



//2022/07/29   for FbxAMatrix
#include <fbxsdk.h>
#include <fbxsdk/scene/shading/fbxlayeredtexture.h>
#include <fbxsdk/scene/animation/fbxanimevaluator.h>




//class

//template<class T> struct Property {
//	T& r;
//	operator T() { return (T)r; }
//	void operator =(const T v) { r = v; }
//};

//namespace mc {
//	template<class T> struct property {
//		T _v;
//		operator T() { return _v; }
//		void operator=(const T& v) { _v = v; }
//		T* operator->() { return &_v; }
//		T& operator *() { return  _v; }
//		friend std::ostream& operator<<(std::ostream& os, const property& it) { return os << it._v; }
//	};
//}


//ChaMatrixのdata[]のindexをわかりやすく
enum {
	MATI_11,
	MATI_12,
	MATI_13,
	MATI_14,

	MATI_21,
	MATI_22,
	MATI_23,
	MATI_24,

	MATI_31,
	MATI_32,
	MATI_33,
	MATI_34,

	MATI_41,
	MATI_42,
	MATI_43,
	MATI_44,

	MATI_MAX
};

class ChaMatrix
{
public:
	ChaMatrix();
	ChaMatrix(float m11, float m12, float m13, float m14, float m21, float m22, float m23, float m24, float m31, float m32, float m33, float m34, float m41, float m42, float m43, float m44);

//#ifdef CONVD3DX11
	ChaMatrix(DirectX::XMMATRIX m);
	ChaMatrix(Matrix m);
//#endif

	~ChaMatrix();

	void SetParams(ChaMatrix srcmat) {
		data[MATI_11] = srcmat.data[MATI_11]; data[MATI_12] = srcmat.data[MATI_12]; data[MATI_13] = srcmat.data[MATI_13]; data[MATI_14] = srcmat.data[MATI_14];
		data[MATI_21] = srcmat.data[MATI_21]; data[MATI_22] = srcmat.data[MATI_22]; data[MATI_23] = srcmat.data[MATI_23]; data[MATI_24] = srcmat.data[MATI_24];
		data[MATI_31] = srcmat.data[MATI_31]; data[MATI_32] = srcmat.data[MATI_32]; data[MATI_33] = srcmat.data[MATI_33]; data[MATI_34] = srcmat.data[MATI_34];
		data[MATI_41] = srcmat.data[MATI_41]; data[MATI_42] = srcmat.data[MATI_42]; data[MATI_43] = srcmat.data[MATI_43]; data[MATI_44] = srcmat.data[MATI_44];
	};
	void SetParams(Matrix m) {
		data[MATI_11] = m._11; data[MATI_12] = m._12; data[MATI_13] = m._13; data[MATI_14] = m._14;
		data[MATI_21] = m._21; data[MATI_22] = m._22; data[MATI_23] = m._23; data[MATI_24] = m._24;
		data[MATI_31] = m._31; data[MATI_32] = m._32; data[MATI_33] = m._33; data[MATI_34] = m._34;
		data[MATI_41] = m._41; data[MATI_42] = m._42; data[MATI_43] = m._43; data[MATI_44] = m._44;
	};
	void SetParams(float m11, float m12, float m13, float m14, float m21, float m22, float m23, float m24, float m31, float m32, float m33, float m34, float m41, float m42, float m43, float m44) {
		data[MATI_11] = m11; data[MATI_12] = m12; data[MATI_13] = m13; data[MATI_14] = m14;
		data[MATI_21] = m21; data[MATI_22] = m22; data[MATI_23] = m23; data[MATI_24] = m24;
		data[MATI_31] = m31; data[MATI_32] = m32; data[MATI_33] = m33; data[MATI_34] = m34;
		data[MATI_41] = m41; data[MATI_42] = m42; data[MATI_43] = m43; data[MATI_44] = m44;
	};



	ChaMatrix operator= (ChaMatrix m);
//#ifdef CONVD3DX11
	ChaMatrix operator= (DirectX::XMMATRIX m);
//#endif

	ChaMatrix operator* (float srcw) const;
	ChaMatrix &operator*= (float srcw);
	ChaMatrix operator/ (float srcw) const;
	ChaMatrix &operator/= (float srcw);
	ChaMatrix operator* (double srcw) const;
	ChaMatrix& operator*= (double srcw);
	ChaMatrix operator/ (double srcw) const;
	ChaMatrix& operator/= (double srcw);
	ChaMatrix operator+ (const ChaMatrix &m) const;
	ChaMatrix &operator+= (const ChaMatrix &m);
	ChaMatrix operator- (const ChaMatrix &m) const;
	ChaMatrix &operator-= (const ChaMatrix &m);
	ChaMatrix operator* (const ChaMatrix &m) const;
	ChaMatrix &operator*= (const ChaMatrix &m);
	//ChaMatrix operator/ (const ChaMatrix &m) const;
	//ChaMatrix &operator/= (const ChaMatrix &m);
	ChaMatrix operator- () const;

	bool operator== (const ChaMatrix &m) const {
		bool result;
		result = data[0] == m.data[MATI_11] && data[1] == m.data[MATI_12] && data[2] == m.data[MATI_13] && data[3] == m.data[MATI_14] &&
			data[4] == m.data[MATI_21] && data[5] == m.data[MATI_22] && data[6] == m.data[MATI_23] && data[7] == m.data[MATI_24] &&
			data[8] == m.data[MATI_31] && data[9] == m.data[MATI_32] && data[10] == m.data[MATI_33] && data[11] == m.data[MATI_34] &&
			data[12] == m.data[MATI_41] && data[13] == m.data[MATI_42] && data[14] == m.data[MATI_43] && data[15] == m.data[MATI_44];
		return result;
	};
	bool operator!= (const ChaMatrix &m) const {
		return !(*this == m);
	};

//#ifdef CONVD3DX11
	DirectX::XMMATRIX D3DX();
	Matrix TKMatrix();
//#endif
	void SetIdentity();
	FbxAMatrix FBXAMATRIX();
	void SetTranslation(ChaVector3 srctra);//初期化しない
	void SetXYZRotation(CQuaternion* srcaxisq, ChaVector3 srceul);//初期化しない
	void SetXYZRotation(CQuaternion* srcaxisq, CQuaternion srcq);//初期化しない
	void SetScale(ChaVector3 srcscale);//初期化しない
	void SetBasis(ChaMatrix srcmat);//初期化しない　copy3x3
	void SetRotation(EFbxRotationOrder rotorder, CQuaternion* srcaxisq, ChaVector3 srceul);//初期化しない

	void SetTranslationZero();//初期化しない
	void SetForVectorTransform();//初期化しない
	void AddTranslation(ChaVector3 srctra);//初期化しない

	ChaVector3 GetRow(int rowindex);//行
	ChaVector3 GetCol(int colindex);//列
	ChaVector3 GetTranslation();//GetRow(3)
	void SetRow(int rowindex, ChaVector3 srcrow);//行
	void SetCol(int colindex, ChaVector3 srccol);//列

	CQuaternion GetRotQ();

	float* GetDataPtr();

public:
	union
	{
		float data[16];
		__m128 mVec[4];
	};


};

class ChaVectorDbl2
{
public:

	ChaVectorDbl2();
	ChaVectorDbl2(double srcx, double srcy);
	~ChaVectorDbl2();

	void SetParams(double srcx, double srcy) {
		x = srcx;
		y = srcy;
	};

	ChaVectorDbl2 operator= (ChaVectorDbl2 v);
	ChaVectorDbl2 operator* (double srcw) const;
	ChaVectorDbl2& operator*= (double srcw);
	ChaVectorDbl2 operator/ (double srcw) const;
	ChaVectorDbl2& operator/= (double srcw);
	ChaVectorDbl2 operator+ (const ChaVectorDbl2& v) const;
	ChaVectorDbl2& operator+= (const ChaVectorDbl2& v);
	ChaVectorDbl2 operator- (const ChaVectorDbl2& v) const;
	ChaVectorDbl2& operator-= (const ChaVectorDbl2& v);
	ChaVectorDbl2 operator* (const ChaVectorDbl2& v) const;
	//ChaVectorDbl2 &operator*= (const ChaVectorDbl2 &v);
	//ChaVectorDbl2 operator/ (const ChaVectorDbl2 &q) const;
	//ChaVectorDbl2 &operator/= (const ChaVectorDbl2 &q);
	ChaVectorDbl2 operator- () const;

	bool operator== (const ChaVectorDbl2& v) const { return x == v.x && y == v.y; };
	bool operator!= (const ChaVectorDbl2& v) const { return !(*this == v); };

	//#ifdef CONVD3DX11
	DirectX::XMFLOAT2 D3DX();
	//#endif

public:
	double x;
	double y;

};

class ChaVector2
{
public:

	ChaVector2();
	ChaVector2(float srcx, float srcy);
	~ChaVector2();

	void SetParams(float srcx, float srcy) {
		x = srcx; y = srcy;
	};
	void SetParams(ChaVector2 srcvec2) {
		x = srcvec2.x; y = srcvec2.y;
	};

	ChaVector2 operator= (ChaVector2 v);
	ChaVector2 operator* (float srcw) const;
	ChaVector2 &operator*= (float srcw);
	ChaVector2 operator/ (float srcw) const;
	ChaVector2 &operator/= (float srcw);
	ChaVector2 operator* (double srcw) const;
	ChaVector2& operator*= (double srcw);
	ChaVector2 operator/ (double srcw) const;
	ChaVector2& operator/= (double srcw);
	ChaVector2 operator+ (const ChaVector2 &v) const;
	ChaVector2 &operator+= (const ChaVector2 &v);
	ChaVector2 operator- (const ChaVector2 &v) const;
	ChaVector2 &operator-= (const ChaVector2 &v);
	ChaVector2 operator* (const ChaVector2 &v) const;

	ChaVector2 operator+ (const ChaVectorDbl2& v) const;
	ChaVector2 operator* (const ChaVectorDbl2& v) const;
	//ChaVector2 &operator*= (const ChaVector2 &v);
	//ChaVector2 operator/ (const ChaVector2 &q) const;
	//ChaVector2 &operator/= (const ChaVector2 &q);
	ChaVector2 operator- () const;

	bool operator== (const ChaVector2 &v) const { return x == v.x && y == v.y; };
	bool operator!= (const ChaVector2 &v) const { return !(*this == v); };

//#ifdef CONVD3DX11
	DirectX::XMFLOAT2 D3DX();
//#endif

public:
	 float x;
	float y;

};


class ChaVector3
{
public:

	ChaVector3();
	ChaVector3(float srcx, float srcy, float srcz);
	ChaVector3(DirectX::XMVECTOR v);
	ChaVector3(FbxDouble3 srcdouble3);
	ChaVector3(FbxVector4 srcvector4, bool divbyw);
	~ChaVector3();

	void SetZeroVec3();
	void SetParams(float srcx, float srcy, float srcz) {
		x = srcx; y = srcy; z = srcz;
	};
	void SetParams(ChaVector3 srcvec3) {
		x = srcvec3.x; y = srcvec3.y; z = srcvec3.z;
	};
	void SetParams(Vector3 srcvec3) {
		x = srcvec3.x; y = srcvec3.y; z = srcvec3.z;
	}
	void SetParams(FbxVector4 srcvec4) {
		x = (float)srcvec4[0]; y = (float)srcvec4[1]; z = (float)srcvec4[2];
	}
	void SetParams(FbxVector4 srcvector4, bool divbyw);
	void SetParams(FbxDouble3 srcdbl3) {
		x = (float)srcdbl3[0]; y = (float)srcdbl3[1]; z = (float)srcdbl3[2];
	};


	ChaVector3 operator= (ChaVector3 v);
	ChaVector3 operator* (float srcw) const;
	ChaVector3 &operator*= (float srcw);
	ChaVector3 operator/ (float srcw) const;
	ChaVector3 &operator/= (float srcw);
	ChaVector3 operator* (double srcw) const;
	ChaVector3& operator*= (double srcw);
	ChaVector3 operator/ (double srcw) const;
	ChaVector3& operator/= (double srcw);
	ChaVector3 operator+ (const ChaVector3 &v) const;
	ChaVector3 &operator+= (const ChaVector3 &v);
	ChaVector3 operator- (const ChaVector3 &v) const;
	ChaVector3 &operator-= (const ChaVector3 &v);
	//ChaVector3 operator* (const ChaVector3 &v) const;
	//ChaVector3 &operator*= (const ChaVector3 &v);
	//ChaVector3 operator/ (const ChaVector3 &q) const;
	//ChaVector3 &operator/= (const ChaVector3 &q);
	ChaVector3 operator- () const;

	bool operator== (const ChaVector3 &v) const { return x == v.x && y == v.y && z == v.z; };
	bool operator!= (const ChaVector3 &v) const { return !(*this == v); };

	ChaMatrix MakeTraMat();
	ChaMatrix MakeInvTraMat();
	ChaMatrix MakeXYZRotMat(CQuaternion* srcaxisq);
	ChaMatrix MakeScaleMat();
	
	FbxDouble3 ConvRotOrder2XYZ(EFbxRotationOrder rotorder);//src:*this, dst:return value


	void Clamp(float srcmin, float srcmax);
	void Normalize();

	//int GetRowNo(int srcrowno);



//#ifdef CONVD3DX11
	DirectX::XMFLOAT3 D3DX();
	DirectX::XMVECTOR XMVECTOR(float w);
//#endif
	COLORREF ColorRef();

public:
	float x;
	float y;
	float z;

};




typedef struct tag_befeul
{
	ChaVector3 befframeeul;
	ChaVector3 currentframeeul;

	void Init()
	{
		befframeeul.SetParams(0.0f, 0.0f, 0.0f);
		currentframeeul.SetParams(0.0f, 0.0f, 0.0f);
	};
	tag_befeul()
	{
		Init();
	};

}BEFEUL;



class ChaVector4
{
public:

	ChaVector4();
	ChaVector4(float srcx, float srcy, float srcz, float srcw);
	ChaVector4(ChaVector3 srcvec3, float srcw);
	~ChaVector4();

	void SetParams(float srcx, float srcy, float srcz, float srcw) {
		x = srcx; y = srcy; z = srcz; w = srcw;
	};
	void SetParams(ChaVector4 srcvec4) {
		x = srcvec4.x; y = srcvec4.y; z = srcvec4.z; w = srcvec4.w;
	};
	void SetParams(ChaVector3 srcvec3, float srcw) {
		x = srcvec3.x; y = srcvec3.y; z = srcvec3.z; w = srcw;
	};


	ChaVector4 operator= (ChaVector4 v);
	ChaVector4 operator* (float srcw) const;
	ChaVector4 &operator*= (float srcw);
	ChaVector4 operator/ (float srcw) const;
	ChaVector4 &operator/= (float srcw);
	ChaVector4 operator* (double srcw) const;
	ChaVector4& operator*= (double srcw);
	ChaVector4 operator/ (double srcw) const;
	ChaVector4& operator/= (double srcw);
	ChaVector4 operator+ (const ChaVector4 &v) const;
	ChaVector4 &operator+= (const ChaVector4 &v);
	ChaVector4 operator- (const ChaVector4 &v) const;
	ChaVector4 &operator-= (const ChaVector4 &v);
	ChaVector4 operator* (const ChaVector4 &v) const;
	ChaVector4 &operator*= (const ChaVector4 &v);
	ChaVector4 operator- () const;

	bool operator== (const ChaVector4 &v) const { return x == v.x && y == v.y && z == v.z && w == v.w; };
	bool operator!= (const ChaVector4 &v) const { return !(*this == v); };

	void Clamp(float srcmin, float srcmax);
	void Normalize();
	void SetZeroVec4(float srcw) {
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
		w = srcw;
	};


	int HSV_Add(ChaVector4 srcadd);
	int HSV_AddH(float addh);
	int HSV_AddS(float adds);
	int HSV_AddV(float addv);
	void HSV_Lerp(ChaVector4 befhsv, ChaVector4 afthsv, float t);
	ChaVector4 RGB2HSV();
	ChaVector4 HSV2RGB();

	void ClampHSV();


//#ifdef CONVD3DX11
	DirectX::XMFLOAT4 D3DX();
//#endif

public:
	float x;
	float y;
	float z;
	float w;

};


class ChaTexRGBA
{
public:
	ChaTexRGBA();
	ChaTexRGBA(unsigned char srcR, unsigned char srcG, unsigned char srcB, unsigned char srcA);
	ChaTexRGBA(float srcR, float srcG, float srcB, float srcA);
	ChaTexRGBA(ChaVector4 src);
	~ChaTexRGBA();

	void InitParams();
	void Clamp() {
		R = min(255, R);
		R = max(0, R);
		G = min(255, G);
		G = max(0, G);
		B = min(255, B);
		B = max(0, B);
		A = min(255, A);
		A = max(0, A);
	};
	
	void FromHSV(ChaVector4 srchsv);
	void FromRGBA(ChaVector4 srcrgba);

	void SetParams(unsigned char srcR, unsigned char srcG, unsigned char srcB, unsigned char srcA) {
		R = srcR;
		G = srcG;
		B = srcB;
		A = srcA;
		Clamp();
	};
	void SetParams(float srcR, float srcG, float srcB, float srcA) {
		R = (unsigned char)(srcR * 255.0f);
		G = (unsigned char)(srcG * 255.0f);
		B = (unsigned char)(srcB * 255.0f);
		A = (unsigned char)(srcA * 255.0f);
		Clamp();
	};
	void SetParams(ChaVector4 src) {
		SetParams(src.x, src.y, src.z, src.w);
		Clamp();
	};


	ChaTexRGBA operator= (ChaTexRGBA v);
	ChaTexRGBA operator* (float srcw) const;
	ChaTexRGBA& operator*= (float srcw);
	ChaTexRGBA operator/ (float srcw) const;
	ChaTexRGBA& operator/= (float srcw);
	ChaTexRGBA operator* (double srcw) const;
	ChaTexRGBA& operator*= (double srcw);
	ChaTexRGBA operator/ (double srcw) const;
	ChaTexRGBA& operator/= (double srcw);
	ChaTexRGBA operator+ (const ChaTexRGBA& v) const;
	ChaTexRGBA& operator+= (const ChaTexRGBA& v);
	ChaTexRGBA operator- (const ChaTexRGBA& v) const;
	ChaTexRGBA& operator-= (const ChaTexRGBA& v);
	ChaTexRGBA operator* (const ChaTexRGBA& v) const;
	ChaTexRGBA& operator*= (const ChaTexRGBA& v);

	bool operator== (const ChaTexRGBA& v) const { return R == v.R && G == v.G && B == v.B && A == v.A; };
	bool operator!= (const ChaTexRGBA& v) const { return !(*this == v); };

public:
	unsigned char R;
	unsigned char G;
	unsigned char B;
	unsigned char A;
};

class ChaPlane
{
public:
	ChaPlane();
	ChaPlane(float srca, float srcb, float srcc, float srcd);
	~ChaPlane();

	void SetParams(float srca, float srcb, float srcc, float srcd) {
		a = srca; b = srcb; c = srcc; d = srcd;
	};

	int FromPoints(ChaVector3 point1, ChaVector3 point2, ChaVector3 point3);
	int GetFootOnPlane(ChaVector3 srcpos, ChaVector3* dstpos);

	ChaPlane operator= (ChaPlane v);
	bool operator== (const ChaPlane& v) const { return a == v.a && b == v.b && c == v.c && d == v.d; };
	bool operator!= (const ChaPlane& v) const { return !(*this == v); };

public:
	float a;
	float b;
	float c;
	float d;
};



class CQuaternion
{
public:
	CQuaternion();
	CQuaternion(float srcw, float srcx, float srcy, float srcz);
	~CQuaternion();
	void InitParams();
	int SetParams(float srcw, float srcx, float srcy, float srcz);
//#ifdef CONVD3DX11
	int SetParams(DirectX::XMFLOAT4 srcxq);
//#endif

	int SetAxisAndRot(ChaVector3 srcaxis, float phai);
	int SetAxisAndRot(ChaVector3 srcaxis, double phai);

	int SetRotation(EFbxRotationOrder rotorder, CQuaternion* srcaxisq, ChaVector3 srceul);
	int SetRotationXYZ(CQuaternion* axisq, ChaVector3 srcdeg);
	int SetRotationRadXYZ(CQuaternion* axisq, ChaVector3 srcrad);
	int SetRotationZXY(CQuaternion* axisq, ChaVector3 srcdeg);
	int SetRotationXYZ(CQuaternion* axisq, double degx, double degy, double degz);
	int SetRotationRadXYZ(CQuaternion* axisq, double radx, double rady, double radz);
	int SetRotationZXY(CQuaternion* axisq, double degx, double degy, double degz);
	//int GetAxisAndRot(ChaVector3* axisvecptr, float* frad);
	//int QuaternionToAxisAngle(ChaVector3* dstaxis, float* dstrad);
	int CalcFBXEulXYZ(CQuaternion* axisq, BEFEUL befeul, ChaVector3* reteul, int isfirstbone, int isendbone, int notmodifyflag);
	//int CalcFBXEulZXY(CQuaternion* axisq, ChaVector3 befeul, ChaVector3* reteul, int isfirstbone);
	int IsInit();

	//void MakeFromBtMat3x3(btMatrix3x3 eulmat);
	void MakeFromD3DXMat(ChaMatrix eulmat);

	//CQuaternion operator= (const CQuaternion &q) const;
	CQuaternion operator= (CQuaternion q);
	CQuaternion operator* (float srcw) const;
	CQuaternion &operator*= (float srcw);
	CQuaternion operator/ (float srcw) const;
	CQuaternion &operator/= (float srcw);
	CQuaternion operator* (double srcw) const;
	CQuaternion& operator*= (double srcw);
	CQuaternion operator/ (double srcw) const;
	CQuaternion& operator/= (double srcw);
	CQuaternion operator+ (const CQuaternion &q) const;
	CQuaternion &operator+= (const CQuaternion &q);
	CQuaternion operator- (const CQuaternion &q) const;
	CQuaternion &operator-= (const CQuaternion &q);
	CQuaternion operator* (const CQuaternion &q) const;
	CQuaternion &operator*= (const CQuaternion &q);
	//CQuaternion operator/ (const CQuaternion &q) const;
	//CQuaternion &operator/= (const CQuaternion &q);
	CQuaternion operator- () const;
	//CQuaternion inv () const;
	CQuaternion normalize();

	int inv(CQuaternion* dstq);
	CQuaternion inverse();

	bool operator== (const CQuaternion &q) const { return w == q.w && x == q.x && y == q.y && z == q.z; }
	bool operator!= (const CQuaternion &q) const { return !(*this == q); }

	//float DotProduct(CQuaternion srcq);
	double DotProduct(CQuaternion srcq);
	double CalcRad(CQuaternion srcq);

	CQuaternion Slerp(CQuaternion endq, int framenum, int frameno);
	int Slerp2(CQuaternion endq, double srcrate, CQuaternion* dstq);

	//int Squad(CQuaternion q0, CQuaternion q1, CQuaternion q2, CQuaternion q3, float t);

	ChaMatrix MakeRotMatX();

	int RotationArc(ChaVector3 srcvec0, ChaVector3 srcvec1);
	int Rotate(ChaVector3* dstvec, ChaVector3 srcvec);

//#ifdef CONVD3DX11
	int Q2X(DirectX::XMFLOAT4* dstx);
	int Q2X(DirectX::XMFLOAT4* dstx, CQuaternion srcq);
//#endif

	int transpose(CQuaternion* dstq);

	int CalcSym(CQuaternion* dstq);
	ChaMatrix CalcSymX2();


	//左ねじ
	//int Q2EulZXY(CQuaternion* axisq, ChaVector3 befeul, ChaVector3* reteul);
	//int Q2EulYXZ(CQuaternion* axisq, ChaVector3 befeul, ChaVector3* reteul);
	int Q2EulXYZusingMat(int rotorder, CQuaternion* axisq, ChaVector3 befeul, ChaVector3* reteul, int notmodify180flag);//bulletもXYZの順
	int Q2EulXYZusingQ(bool srcunderIKRot, bool srcunderRetarget, 
		CQuaternion* axisq, BEFEUL befeul, ChaVector3* reteul, int isfirstbone, int isendbone, int notmodify180flag);//bulletもXYZの順
	//int Q2EulZYX(int needmodifyflag, CQuaternion* axisq, ChaVector3 befeul, ChaVector3* reteul);


	//int Q2Eul(CQuaternion* axisq, ChaVector3 befeul, ChaVector3* reteul);
	//int Q2EulBt(ChaVector3* reteul);

	void RotationMatrix(ChaMatrix srcmat);

	//inout : srcdstq
	int InOrder(CQuaternion* srcdstq);

	//ChaModifyEuler360へ
	//int ModifyEuler360(ChaVector3* eulerA, ChaVector3* eulerB, int notmodify180flag, float throundX, float throundY, float throundZ);
	
	int ModifyEulerXYZ(ChaVector3* eulerA, ChaVector3* eulerB, int isfirstbone, int isendbone, int notmodifyflag);

private:

	double vecDotVec(ChaVector3* vec1, ChaVector3* vec2);
	double lengthVec(ChaVector3* vec);
	double aCos(double dot);
	int vec3RotateY(ChaVector3* dstvec, double deg, ChaVector3* srcvec);
	int vec3RotateX(ChaVector3* dstvec, double deg, ChaVector3* srcvec);
	int vec3RotateZ(ChaVector3* dstvec, double deg, ChaVector3* srcvec);

	//int ModifyEuler(ChaVector3* eulerA, ChaVector3* eulerB);
	//int ModifyEulerXYZ(ChaVector3* eulerA, ChaVector3* eulerB, int isfirstbone, int isendbone, int notmodifyflag);//publicに移動
	int GetRound(float srcval);

	//ChaGetRoundThreshold()へ
	//int GetRoundThreshold(float srcval, float degth);

	float QuaternionLimitPhai(float srcphai);
	double QuaternionLimitPhai(double srcphai);

public:
	 float x;
	float y;
	float z;
	float w;
};


//struct

typedef  struct tag_rpselem
{
	int framecnt;
	int skelno;
	float confidence;
	ChaVector3 pos;
	int twistflag;
	tag_rpselem() {
		framecnt = 0;
		skelno = 0;
		confidence = 0.0f;
		pos.SetZeroVec3();
		twistflag = 0;
	};
}RPSELEM;



typedef  struct tag_ui_pickinfo
{
	int buttonflag;
	//WM_LBUTTONDOWN-->PICK_L, WM_RBUTTONDOWN-->PICK_R, WM_MBUTTONDOWN-->PICK_M。押していないとき-->PICK_NONE。
	//以下、buttonflagがPICK_NONE以外の時に意味を持つ。
	POINT mousepos;
	POINT mousebefpos;
	POINT clickpos;
	ChaVector2 diffmouse;
	ChaVector2 firstdiff;
	int winx;
	int winy;
	int pickrange;
	int pickobjno;
	ChaVector3 objscreen;
	ChaVector3 objworld;

	tag_ui_pickinfo() {
		buttonflag = 0;
		mousepos.x = 0;
		mousepos.y = 0;
		mousebefpos = mousepos;
		clickpos = mousepos;
		firstdiff.x = 0;
		firstdiff.y = 0;
		winx = 0;
		winy = 0;
		pickrange = 0;
		pickobjno = 0;
		objscreen.SetZeroVec3();
		objworld.SetZeroVec3();
	};
}UIPICKINFO;


enum {
	NUMKEYPICK_MQOOBJECT,
	NUMKEYPICK_MQOMATERIAL,
	NUMKEYPICK_MAX
};


typedef  struct tag_cpmot
{
	int boneno;
	double frame;
	ChaVector3 eul;
	ChaVector3 tra;
	tag_cpmot() {
		boneno = 0;
		frame = 0.0;
		eul.SetZeroVec3();
		tra.SetZeroVec3();
	};
}CPMOT;


typedef  struct tag_texv
{
	ChaVector3 pos;
	ChaVector2 uv;
	tag_texv() {
		pos.SetZeroVec3();
		uv.x = 0.0f;
		uv.y = 0.0f;
	};
}TEXV;

typedef struct  tag_spritev {
	ChaVector4 pos;
	ChaVector2 uv;
	tag_spritev() {
		pos.x = 0.0f;
		pos.y = 0.0f;
		pos.z = 0.0f;
		pos.w = 0.0f;
		uv.x = 0.0f;
		uv.y = 0.0f;
	};
} SPRITEV;


typedef struct tag_verface
{
	int			faceno;
	int			orgfaceno;
	int			materialno;
	ChaVector3	facenormal;

	tag_verface() {
		faceno = 0;
		orgfaceno = 0;
		materialno = 0;
		facenormal.SetZeroVec3();
	};
}PERFACE;

typedef  struct tag_pervert
{
	int				indexno;//3角の順番
	int				vno;
	int				uvnum;
	ChaVector2		uv[2];
	int				vcolflag;
	DWORD			vcol;
	ChaVector3 smnormal;

	int				createflag;
	//頂点を作成しない場合０
	//UV, VCOL, Materialnoの違いにより作成する場合は１を足す
	//normalにより作成する場合は２を足す

	tag_pervert() {
		indexno = 0;
		vno = 0;
		uvnum = 0;
		uv[0].x = 0.0f;
		uv[0].y = 0.0f;
		uv[1].x = 0.0f;
		uv[1].y = 0.0f;
		vcolflag = 0;
		vcol = 0;
		smnormal.SetZeroVec3();
		createflag = 0;
	};

}PERVERT;

typedef  struct tag_pm3optv
{
	int orgvno;
	int orgfaceno;
	ChaVector3 pos;
	int materialno;
	ChaVector3 normal;
	int				uvnum;
	ChaVector2		uv[2];
	int				vcolflag;
	DWORD			vcol;

	tag_pm3optv() {
		orgvno = 0;
		orgfaceno = 0;
		pos.SetZeroVec3();
		materialno = 0;
		normal.SetZeroVec3();
		uvnum = 0;
		uv[0].x = 0.0f;
		uv[0].y = 0.0f;
		uv[1].x = 0.0f;
		uv[1].y = 0.0f;
		vcolflag = 0;
		vcol = 0;
	};

}PM3OPTV;

//typedef  struct tag_pm3dispv
//{
//	ChaVector4		pos;
//	ChaVector4		normal;
//	ChaVector2		uv;
//	tag_pm3dispv() {
//		pos.x = 0.0f;
//		pos.y = 0.0f;
//		pos.z = 0.0f;
//		pos.w = 1.0f;
//		normal.x = 0.0f;
//		normal.y = 0.0f;
//		normal.z = 0.0f;
//		normal.w = 0.0f;
//		uv.x = 0.0f;
//		uv.y = 0.0f;
//	};
//}PM3DISPV;

typedef struct tag_binormaldispv
{
	ChaVector4		pos;
	ChaVector4		normal;
	ChaVector4		tangent;
	ChaVector4		binormal;
	ChaVector2		uv[2];
	ChaVector4		projpos;
	tag_binormaldispv() {
		pos.x = 0.0f;
		pos.y = 0.0f;
		pos.z = 0.0f;
		pos.w = 1.0f;
		normal.x = 0.0f;
		normal.y = 0.0f;
		normal.z = 0.0f;
		normal.w = 0.0f;
		tangent.x = 0.0f;
		tangent.y = 0.0f;
		tangent.z = 0.0f;
		tangent.w = 0.0f;
		binormal.x = 0.0f;
		binormal.y = 0.0f;
		binormal.z = 0.0f;
		binormal.w = 0.0f;
		uv[0].x = 0.0f;
		uv[0].y = 0.0f;
		uv[1].x = 0.0f;
		uv[1].y = 0.0f;
		projpos.x = 0.0f;
		projpos.y = 0.0f;
		projpos.z = 0.0f;
		projpos.w = 1.0f;
	};
}BINORMALDISPV;



typedef  struct tag_extlinev
{
	ChaVector4 pos;
	tag_extlinev() {
		pos.x = 0.0f;
		pos.y = 0.0f;
		pos.z = 0.0f;
		pos.w = 1.0f;
	};
}EXTLINEV;


typedef  struct tag_modelbound
{
	ChaVector3 min;
	ChaVector3 max;
	ChaVector3 center;
	float		r;
	bool validflag;

	void Init()
	{
		min.SetParams(FLT_MAX, FLT_MAX, FLT_MAX);
		max.SetParams(-FLT_MAX, -FLT_MAX, -FLT_MAX);
		center.SetZeroVec3();
		validflag = false;
		r = 0.0f;
	};
	tag_modelbound() {
		Init();
	};
	void SetIsValid(bool srcflag) {
		validflag = srcflag;
	};
	bool IsValid() {
		return validflag;
	};
}MODELBOUND;




#ifdef CHACALCCPP
BOOL IsValidNewEul(ChaVector3 srcneweul, ChaVector3 srcbefeul);
ChaMatrix ChaMatrixTranspose(ChaMatrix srcmat);

int CalcTangentAndBinormal(int srcuvnum, BINORMALDISPV* vert_0, BINORMALDISPV* vert_1, BINORMALDISPV* vert_2);
double ChaVector3LengthDbl(ChaVector3* psrc);
double ChaVector3DotDbl(const ChaVector3* psrc1, const ChaVector3* psrc2);


//float ChaVector3LengthDbl(ChaVector3* psrc);
void ChaVector3Normalize(ChaVector3* pdst, const ChaVector3* psrc);
float ChaVector3Dot(const ChaVector3* psrc1, const ChaVector3* psrc2);
void ChaVector3Cross(ChaVector3* pdst, const ChaVector3* psrc1, const ChaVector3* psrc2);
void ChaVector3TransformCoord(ChaVector3* pdst, ChaVector3* psrc, ChaMatrix* pmat);
double ChaVector3LengthSqDbl(ChaVector3* psrc);
ChaVector3* ChaVector3TransformNormal(ChaVector3 *pOut, const ChaVector3* pV, const ChaMatrix* pM);


void ChaMatrixIdentity(ChaMatrix* pdst);
ChaMatrix ChaMatrixScale(ChaMatrix srcmat);//スケール成分だけの行列にする
ChaMatrix ChaMatrixRot(ChaMatrix srcmat);//回転成分だけの行列にする
ChaMatrix ChaMatrixTra(ChaMatrix srcmat);//移動成分だけの行列にする
ChaVector3 ChaMatrixScaleVec(ChaMatrix srcmat);//スケール成分のベクトルを取得
ChaVector3 ChaMatrixRotVec(ChaMatrix srcmat, int notmodify180flag);//回転成分のベクトルを取得
ChaVector3 ChaMatrixTraVec(ChaMatrix srcmat);//移動成分のベクトルを取得
CQuaternion ChaMatrix2Q(ChaMatrix srcmat);//ChaMatrixを受け取って　CQuaternionを返す
void ChaMatrixNormalizeRot(ChaMatrix* pdst);
void ChaMatrixInverse(ChaMatrix* pdst, float* pdet, const ChaMatrix* psrc);
void ChaMatrixTranslation(ChaMatrix* pdst, float srcx, float srcy, float srcz);
void ChaMatrixTranspose(ChaMatrix* pdst, ChaMatrix* psrc);
void ChaMatrixRotationAxis(ChaMatrix* pdst, ChaVector3* srcaxis, float srcrad);
void ChaMatrixScaling(ChaMatrix* pdst, float srcx, float srcy, float srcz);
void ChaMatrixLookAtRH(ChaMatrix* dstviewmat, ChaVector3* camEye, ChaVector3* camtar, ChaVector3* camUpVec);
ChaMatrix* ChaMatrixOrthoOffCenterRH(ChaMatrix* pOut, float l, float r, float t, float b, float zn, float zf);
ChaMatrix* ChaMatrixPerspectiveFovRH(ChaMatrix* pOut, float fovY, float Aspect, float zn, float zf);
ChaMatrix* ChaMatrixRotationYawPitchRoll(ChaMatrix* pOut, float srcyaw, float srcpitch, float srcroll);
ChaMatrix* ChaMatrixRotationX(ChaMatrix* pOut, float srcrad);
ChaMatrix* ChaMatrixRotationY(ChaMatrix* pOut, float srcrad);
ChaMatrix* ChaMatrixRotationZ(ChaMatrix* pOut, float srcrad);


const ChaMatrix* ChaMatrixRotationQuaternion(ChaMatrix* dstmat, CQuaternion* srcq);

void CQuaternionIdentity(CQuaternion* dstq);
CQuaternion CQuaternionInv(CQuaternion srcq);


//int qToEulerAxis(CQuaternion axisQ, CQuaternion* srcq, ChaVector3* Euler);
//int modifyEuler(ChaVector3* eulerA, ChaVector3* eulerB);// new, old

double vecDotVec(ChaVector3* vec1, ChaVector3* vec2);
double lengthVec(ChaVector3* vec);
double aCos(double dot);
int vec3RotateY(ChaVector3* dstvec, double deg, ChaVector3* srcvec);
int vec3RotateX(ChaVector3* dstvec, double deg, ChaVector3* srcvec);
int vec3RotateZ(ChaVector3* dstvec, double deg, ChaVector3* srcvec);

size_t AlignmentSize(size_t size, size_t alignment);
int GetRound(float srcval);
int IsInitRot(ChaMatrix srcmat);
int IsSameMat(ChaMatrix srcmat1, ChaMatrix srcmat2);
int IsSameEul(ChaVector3 srceul1, ChaVector3 srceul2);

//void InitAngleLimit(ANGLELIMIT* dstal);
//void SetAngleLimitOff(ANGLELIMIT* dstal);


bool IsJustEqualTime(double srctime1, double srctime2);
double RoundingTime(double srctime);
int IntTime(double srctime);
bool IsEqualRoundingTime(double srctime1, double srctime2);
int Float2Int(float srcfloat);


double VecLength(ChaVector3 srcvec);
void GetSRTMatrix(ChaMatrix srcmat, ChaVector3* svecptr, ChaMatrix* rmatptr, ChaVector3* tvecptr);
void GetSRTMatrix2(ChaMatrix srcmat, ChaMatrix* smatptr, ChaMatrix* rmatptr, ChaMatrix* tmatptr);
void GetSRTandTraAnim(ChaMatrix srcmat, ChaMatrix srcnodemat, ChaMatrix* smatptr, ChaMatrix* rmatptr, ChaMatrix* tmatptr, ChaMatrix* tanimmatptr);//For Local Posture
ChaMatrix ChaMatrixFromSRT(bool sflag, bool tflag, ChaMatrix srcnodemat, ChaMatrix* srcsmat, ChaMatrix* srcrmat, ChaMatrix* srctmat);//For Local Posture
ChaMatrix ChaMatrixFromSRTraAnim(bool sflag, bool tanimflag, ChaMatrix srcnodemat, ChaMatrix* srcsmat, ChaMatrix* srcrmat, ChaMatrix* srctanimmat);//For Local Posture
ChaMatrix GetS0RTMatrix(ChaMatrix srcmat);//拡大縮小を初期化したRT行列を返す
ChaMatrix ChaMatrixKeepScale(ChaMatrix srcmat, ChaVector3 srcsvec);

ChaMatrix TransZeroMat(ChaMatrix srcmat);
ChaMatrix ChaMatrixFromBtMat3x3(btMatrix3x3* srcmat3x3);
ChaMatrix ChaMatrixFromBtTransform(btMatrix3x3* srcmat3x3, btVector3* srcpipot);
ChaMatrix ChaMatrixInv(ChaMatrix srcmat);


CQuaternion QMakeFromBtMat3x3(btMatrix3x3* eulmat);

ChaMatrix MakeRotMatFromChaMatrix(ChaMatrix srcmat);

ChaMatrix ChaMatrixFromFbxAMatrix(FbxAMatrix srcmat);
ChaMatrix CalcAxisMatX(ChaVector3 vecx, ChaVector3 srcpos, ChaMatrix srcmat);

#else
extern BOOL IsValidNewEul(ChaVector3 srcneweul, ChaVector3 srcbefeul);
extern ChaMatrix ChaMatrixTranspose(ChaMatrix srcmat);

extern int CalcTangentAndBinormal(int srcuvnum, BINORMALDISPV* vert_0, BINORMALDISPV* vert_1, BINORMALDISPV* vert_2);
extern double ChaVector3LengthDbl(ChaVector3* psrc);
extern double ChaVector3DotDbl(const ChaVector3* psrc1, const ChaVector3* psrc2);

//extern float ChaVector3LengthDbl(ChaVector3* psrc);
extern void ChaVector3Normalize(ChaVector3* pdst, const ChaVector3* psrc);
extern float ChaVector3Dot(const ChaVector3* psrc1, const ChaVector3* psrc2);
extern void ChaVector3Cross(ChaVector3* pdst, const ChaVector3* psrc1, const ChaVector3* psrc2);
extern void ChaVector3TransformCoord(ChaVector3* pdst, ChaVector3* psrc, ChaMatrix* pmat);
extern double ChaVector3LengthSqDbl(ChaVector3* psrc);
extern ChaVector3* ChaVector3TransformNormal(ChaVector3 *pOut, const ChaVector3* pV, const ChaMatrix* pM);

extern void ChaMatrixIdentity(ChaMatrix* pdst);
extern ChaMatrix ChaMatrixScale(ChaMatrix srcmat);//スケール成分だけの行列にする
extern ChaMatrix ChaMatrixRot(ChaMatrix srcmat);//回転成分だけの行列にする
extern ChaMatrix ChaMatrixTra(ChaMatrix srcmat);//移動成分だけの行列にする
extern ChaVector3 ChaMatrixScaleVec(ChaMatrix srcmat);//スケール成分のベクトルを取得
extern ChaVector3 ChaMatrixRotVec(ChaMatrix srcmat, int notmodify180flag);//回転成分のベクトルを取得
extern ChaVector3 ChaMatrixTraVec(ChaMatrix srcmat);//移動成分のベクトルを取得
extern CQuaternion ChaMatrix2Q(ChaMatrix srcmat);//ChaMatrixを受け取って　CQuaternionを返す
extern void ChaMatrixNormalizeRot(ChaMatrix* pdst);
extern void ChaMatrixInverse(ChaMatrix* pdst, float* pdet, const ChaMatrix* psrc);
extern void ChaMatrixTranslation(ChaMatrix* pdst, float srcx, float srcy, float srcz);
extern void ChaMatrixTranspose(ChaMatrix* pdst, ChaMatrix* psrc);
extern void ChaMatrixRotationAxis(ChaMatrix* pdst, ChaVector3* srcaxis, float srcrad);
extern void ChaMatrixScaling(ChaMatrix* pdst, float srcx, float srcy, float srcz);
extern void ChaMatrixLookAtRH(ChaMatrix* dstviewmat, ChaVector3* camEye, ChaVector3* camtar, ChaVector3* camUpVec);
extern ChaMatrix* ChaMatrixOrthoOffCenterRH(ChaMatrix* pOut, float l, float r, float t, float b, float zn, float zf);
extern ChaMatrix* ChaMatrixPerspectiveFovRH(ChaMatrix* pOut, float fovY, float Aspect, float zn, float zf);
extern ChaMatrix* ChaMatrixRotationYawPitchRoll(ChaMatrix* pOut, float srcyaw, float srcpitch, float srcroll);
extern ChaMatrix* ChaMatrixRotationX(ChaMatrix* pOut, float srcrad);
extern ChaMatrix* ChaMatrixRotationY(ChaMatrix* pOut, float srcrad);
extern ChaMatrix* ChaMatrixRotationZ(ChaMatrix* pOut, float srcrad);

extern const ChaMatrix* ChaMatrixRotationQuaternion(ChaMatrix* dstmat, CQuaternion* srcq);

extern void CQuaternionIdentity(CQuaternion* dstq);
extern CQuaternion CQuaternionInv(CQuaternion srcq);


//extern int qToEulerAxis(CQuaternion axisQ, CQuaternion* srcq, ChaVector3* Euler);
//extern int modifyEuler(ChaVector3* eulerA, ChaVector3* eulerB);// new, old

extern double vecDotVec(ChaVector3* vec1, ChaVector3* vec2);
extern double lengthVec(ChaVector3* vec);
extern double aCos(double dot);
extern int vec3RotateY(ChaVector3* dstvec, double deg, ChaVector3* srcvec);
extern int vec3RotateX(ChaVector3* dstvec, double deg, ChaVector3* srcvec);
extern int vec3RotateZ(ChaVector3* dstvec, double deg, ChaVector3* srcvec);

extern size_t AlignmentSize(size_t size, size_t alignment);
extern int GetRound(float srcval);
extern int IsInitRot(ChaMatrix srcmat);
extern int IsSameMat(ChaMatrix srcmat1, ChaMatrix srcmat2);
extern int IsSameEul(ChaVector3 srceul1, ChaVector3 srceul2);

//extern void InitAngleLimit(ANGLELIMIT* dstal);
//extern void SetAngleLimitOff(ANGLELIMIT* dstal);


extern bool IsJustEqualTime(double srctime1, double srctime2);
extern double RoundingTime(double srctime);
extern int IntTime(double srctime);
extern bool IsEqualRoundingTime(double srctime1, double srctime2);
extern int Float2Int(float srcfloat);

extern double VecLength(ChaVector3 srcvec);
extern void GetSRTMatrix(ChaMatrix srcmat, ChaVector3* svecptr, ChaMatrix* rmatptr, ChaVector3* tvecptr);
extern void GetSRTMatrix2(ChaMatrix srcmat, ChaMatrix* smatptr, ChaMatrix* rmatptr, ChaMatrix* tmatptr);
extern void GetSRTandTraAnim(ChaMatrix srcmat, ChaMatrix srcnodemat, ChaMatrix* smatptr, ChaMatrix* rmatptr, ChaMatrix* tmatptr, ChaMatrix* tanimmatptr);//For Local Posture
extern ChaMatrix ChaMatrixFromSRT(bool sflag, bool tflag, ChaMatrix srcnodemat, ChaMatrix* srcsmat, ChaMatrix* srcrmat, ChaMatrix* srctmat);//For Local Posture
extern ChaMatrix ChaMatrixFromSRTraAnim(bool sflag, bool tanimflag, ChaMatrix srcnodemat, ChaMatrix* srcsmat, ChaMatrix* srcrmat, ChaMatrix* srctanimmat);//For Local Posture
extern ChaMatrix GetS0RTMatrix(ChaMatrix srcmat);//拡大縮小を初期化したRT行列を返す
extern ChaMatrix ChaMatrixKeepScale(ChaMatrix srcmat, ChaVector3 srcsvec);

extern ChaMatrix TransZeroMat(ChaMatrix srcmat);
extern ChaMatrix ChaMatrixFromBtMat3x3(btMatrix3x3* srcmat3x3);
extern ChaMatrix ChaMatrixFromBtTransform(btMatrix3x3* srcmat3x3, btVector3* srcpipot);
extern ChaMatrix ChaMatrixInv(ChaMatrix srcmat);


extern CQuaternion QMakeFromBtMat3x3(btMatrix3x3* eulmat);

extern ChaMatrix MakeRotMatFromChaMatrix(ChaMatrix srcmat);
extern ChaMatrix ChaMatrixFromFbxAMatrix(FbxAMatrix srcmat);
extern ChaMatrix CalcAxisMatX(ChaVector3 vecx, ChaVector3 srcpos, ChaMatrix srcmat);

#endif

class N3SM;

class N3P
{
public:
	N3P();
	~N3P();
	void InitParams();
	PERFACE*	perface;
	PERVERT*	pervert;
	N3SM*		n3sm;
};//n*3

class N3SM
{
public:
	N3SM();
	~N3SM();
	void InitParams();
	int smfacenum;
	//void** ppsmface;//N3Pのポインタの配列
	N3P** ppsmface;//*(ppsmface + smfaceno) --> pointer which is allocateed(malloc) at other place
};



class ChaFrustumInfo
{
public:
	ChaFrustumInfo();
	~ChaFrustumInfo();

	void InitParams();

	//int UpdateFrustum(ChaMatrix matVP);
	int ChkInView(int srclodnum, int srclodno, MODELBOUND srcmb, ChaMatrix matWorld);

	bool GetVisible() {
		return m_visible;
	};
	void SetVisible(bool srcflag) {
		m_visible = srcflag;
	};
	bool GetInShadow() {
		return m_inshadow;
	};
	void SetInShadow(bool srcflag) {
		m_inshadow = srcflag;
	};

	void SetDistFromCamera(double srcdist) {
		m_distfromcamera = srcdist;
	};
	double GetDistFromCamera() {
		return m_distfromcamera;
	};


private:
	bool m_visible;
	bool m_inshadow;

	ChaVector3 m_vecFrustum[8];
	ChaVector3 m_vecTraFrustum[8];
	ChaPlane m_planeFrustum[6];
	ChaVector3 m_footpos[6];

	ChaMatrix m_matVP;
	double m_distfromcamera;
};

class CFogParams
{
public:
	CFogParams() {
		InitParams();
	};
	~CFogParams() {
		DestroyObjs();
	};

	void InitParams() {
		m_fogkind = 0;

		m_distcolor.SetParams(1.0f, 1.0f, 1.0f, 1.0f);
		m_distparams.SetParams(10.0f, 10000.0f, 0.6f);

		m_heightcolor.SetParams(1.0f, 1.0f, 1.0f, 1.0f);
		m_heightparams.SetParams(0.0f, 140.0f, 0.6f);
	};

private:
	void DestroyObjs() {
	};

public:
	void SetFogKind(int srckind) {
		m_fogkind = srckind;
	};
	int GetFogKind() {
		return m_fogkind;
	};


	void SetDistColor(ChaVector4 srcval) {
		m_distcolor = srcval;
	}
	ChaVector4 GetDistColor() {
		return m_distcolor;
	};
	void SetDistNear(float srcval) {
		m_distparams.x = srcval;
	};
	float GetDistNear() {
		return m_distparams.x;
	};
	void SetDistFar(float srcval) {
		m_distparams.y = srcval;
	};
	float GetDistFar() {
		return m_distparams.y;
	};
	void SetDistRate(float srcval) {
		m_distparams.z = srcval;
	};
	float GetDistRate() {
		return m_distparams.z;
	};


	void SetHeightColor(ChaVector4 srcval) {
		m_heightcolor = srcval;
	}
	ChaVector4 GetHeightColor() {
		return m_heightcolor;
	};
	void SetHeightHigh(float srcval) {
		m_heightparams.y = srcval;
	};
	float GetHeightHigh() {
		return m_heightparams.y;
	}
	void SetHeightRate(float srcval) {
		m_heightparams.z = srcval;
	};
	float GetHeightRate() {
		return m_heightparams.z;
	};

private:
	int m_fogkind;//0:no fog, 1:dist fog, 2:height fog

	ChaVector4 m_distcolor;
	ChaVector3 m_distparams;

	ChaVector4 m_heightcolor;
	ChaVector3 m_heightparams;
};


#endif



