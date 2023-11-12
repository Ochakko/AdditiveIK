#ifndef INFBONEH
#define INFBONEH

#include <coef.h>
#include <map>

class CMQOObject;

/*
�{�[���e���x�N���X�B
Inf��influence��Inf�B
*/

class CInfBone
{
public:

/**
 * @fn
 * CInfBone
 * @breaf �R���X�g���N�^�B
 * @return �Ȃ��B
 */
	CInfBone();

/**
 * @fn
 * ~CInfBone
 * @breaf �f�X�g���N�^�B
 * @return �Ȃ��B
 */
	~CInfBone();


/**
 * @fn
 * InitParams
 * @breaf �����o������������B
 * @return �O�B
 */
	int InitParams();

/**
 * @fn
 * ExistBone
 * @breaf �w�肵���{�[���̉e���x�����邩�ǂ����𒲂ׂ�B
 * @param (CMQOObject* srcobj) IN �e���x�𒲂ׂ�RD�I�u�W�F�N�g�B
 * @param (int srcboneno) IN �e���x�����邩���ׂ�{�[���̔ԍ��B
 * @return �w�肵���{�[���̉e�����Ȃ����-1��Ԃ��B�e��������΂��̉e���x�̃C���f�b�N�X��Ԃ��B
 */
	int ExistBone( CMQOObject* srcobj, int srcboneno );

/**
 * @fn
 * AddInfElem
 * @breaf �w��I�u�W�F�N�g�ɉe���x��o�^����B
 * @param (CMQOObject* srcobj) IN �e���x��ݒ肷��RD�I�u�W�F�N�g�B
 * @param (INFELEM srcie) IN �e���x�f�[�^�B
 * @return ����������O�B
 */
	int AddInfElem( CMQOObject* srcobj, INFELEM srcie );

/**
 * @fn
 * NormalizeInf
 * @breaf �e���x�𐳋K������B
 * @param (CMQOObject* srcobj) IN �e���x�f�[�^�����RD�I�u�W�F�N�g�B
 * @return ����������O�B
 */
	int NormalizeInf( CMQOObject* srcobj );


	INFDATA* GetInfData(CMQOObject* srcobj);

private:

/**
 * @fn
 * DestroyObjs
 * @breaf �A���P�[�g�������������J������B
 * @return �O�B
 */
	int DestroyObjs();

/**
 * @fn
 * InitElem
 * @breaf �e���x�f�[�^������������B
 * @param (INFDATA* infptr) IN �����������f�[�^�B
 * @return ����������O�B
 */
	int InitElem( INFDATA* infptr );

private:
	std::map<CMQOObject*, INFDATA*> m_infdata;//�e���x�f�[�^

};

#endif

