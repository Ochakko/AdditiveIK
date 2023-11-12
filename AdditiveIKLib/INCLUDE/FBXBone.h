#ifndef FBXBONE0H
#define FBXBONE0H

#include <coef.h>

//#include <D3DX9.h>
#include <ChaVecCalc.h>
#include <crtdbg.h>
#include <fbxsdk.h>
//#include <quaternion.h>

class CBone;
class CBVHElem;


/*
CFBXBoneはFBXとBVHを書き出す際に、ボーンをFBXやBVH用の階層構造にするために使う。
実際のボーンはCBoneである。
*/


class CFBXBone
{
public:

/**
 * @fn
 * CFBXBone
 * @breaf コンストラクタ。
 * @return なし。
 */
	CFBXBone();

/**
 * @fn
 * ~CFBXBone
 * @breaf デストラクタ。
 * @return なし。
 */
	~CFBXBone();

/**
 * @fn
 * AddChild
 * @breaf ボーンの階層構造を設定する。
 * @param (CFBXBone* childptr) thisの子供にするボーンを指定する。
 * @return 成功したら０。
 */
	int AddChild( CFBXBone* childptr );

private:

/**
 * @fn
 * InitParams
 * @breaf メンバを初期化する。
 * @return ０。
 */
	int InitParams();

/**
 * @fn
 * DestroyObjs
 * @breaf アロケートしたメモリを開放する。
 * @return ０。
 */
	int DestroyObjs();

public:
	//accesser
	int GetType(){
		return m_type;
	};
	void SetType( int srctype ){
		m_type = srctype;
	};

	CBone* GetBone(){
		return m_bone;
	};
	void SetBone( CBone* srcbone ){
		m_bone = srcbone;
	};

	CBVHElem* GetBvhElem(){
		return m_pbe;
	};
	void SetBvhElem( CBVHElem* srcelem ){
		m_pbe = srcelem;
	};

	FbxNode* GetSkelNode(){
		return m_skelnode;
	};
	void SetSkelNode( FbxNode* srcnode ){
		m_skelnode = srcnode;
	};

	int GetBunkiNum(){
		return m_bunkinum;
	};
	void SetBunkiNum( int srcnum ){
		m_bunkinum = srcnum;
	};

	CQuaternion GetAxisQ(){
		return m_axisq;
	};
	void SetAxisQ( CQuaternion srcq ){
		m_axisq = srcq;
	};

	CFBXBone* GetParent(){
		return m_parent;
	};
	void SetParent( CFBXBone* srcparent ){
		m_parent = srcparent;
	};

	CFBXBone* GetChild(){
		return m_child;
	};
	void SetChild( CFBXBone* srcchild ){
		m_child = srcchild;
	};

	CFBXBone* GetBrother(){
		return m_brother;
	};
	void SetBrother( CFBXBone* srcbrother ){
		m_brother = srcbrother;
	};


private:
	int m_type;//ボーンのタイプ
	CBone* m_bone;//ボーン
	CBVHElem* m_pbe;//BVHのジョイント
	FbxNode* m_skelnode;//FBXのノード
	int m_bunkinum;//このボーンについての分岐ボーンの数

	CQuaternion m_axisq;//ボーンの軸を表すクォータニオン

	CFBXBone* m_parent;
	CFBXBone* m_child;
	CFBXBone* m_brother;
};


#endif