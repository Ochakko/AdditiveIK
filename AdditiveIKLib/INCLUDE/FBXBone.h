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
CFBXBone��FBX��BVH�������o���ۂɁA�{�[����FBX��BVH�p�̊K�w�\���ɂ��邽�߂Ɏg���B
���ۂ̃{�[����CBone�ł���B
*/


class CFBXBone
{
public:

/**
 * @fn
 * CFBXBone
 * @breaf �R���X�g���N�^�B
 * @return �Ȃ��B
 */
	CFBXBone();

/**
 * @fn
 * ~CFBXBone
 * @breaf �f�X�g���N�^�B
 * @return �Ȃ��B
 */
	~CFBXBone();

/**
 * @fn
 * AddChild
 * @breaf �{�[���̊K�w�\����ݒ肷��B
 * @param (CFBXBone* childptr) this�̎q���ɂ���{�[�����w�肷��B
 * @return ����������O�B
 */
	int AddChild( CFBXBone* childptr );

private:

/**
 * @fn
 * InitParams
 * @breaf �����o������������B
 * @return �O�B
 */
	int InitParams();

/**
 * @fn
 * DestroyObjs
 * @breaf �A���P�[�g�������������J������B
 * @return �O�B
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
	int m_type;//�{�[���̃^�C�v
	CBone* m_bone;//�{�[��
	CBVHElem* m_pbe;//BVH�̃W���C���g
	FbxNode* m_skelnode;//FBX�̃m�[�h
	int m_bunkinum;//���̃{�[���ɂ��Ă̕���{�[���̐�

	CQuaternion m_axisq;//�{�[���̎���\���N�H�[�^�j�I��

	CFBXBone* m_parent;
	CFBXBone* m_child;
	CFBXBone* m_brother;
};


#endif