#ifndef IMPFILEH
#define IMPFILEH

//#include <d3dx9.h>
#include <ChaVecCalc.h>
#include <coef.h>
#include <XMLIO.h>

#include <map>
#include <string>

class CModel;
class CMQOObject;
class CBone;
class CRigidElem;


/*
�C���p���X�t�@�C���̃N���X�B
Imp��impulse��imp�B
*/


class CImpFile : public CXMLIO
{
public:

/**
 * @fn
 * CImpFile
 * @breaf �R���X�g���N�^�B
 * @return �Ȃ��B
 */
	CImpFile();

/**
 * @fn
 * ~CImpFile
 * @breaf �f�X�g���N�^�B
 * @return �Ȃ��B
 * @detail �h���N���X�̃f�X�g���N�^�Ȃ̂�virtual�B
 */
	virtual ~CImpFile();


/**
 * @fn
 * WriteImpFile
 * @breaf �C���p���X�t�@�C���������o���B
 * @param (WCHAR* strpath) IN �����o���t�@�C���̃p�X�B
 * @param (CModel* srcmodel) IN �����o�����f���f�[�^�B
 * @return ����������O�B
 * @detail srcmodel�ɐݒ肳��Ă���C���p���X�������o���B
 */
	int WriteImpFile( WCHAR* strpath, CModel* srcmodel );


/**
 * @fn
 * LoadImpFile
 * @breaf �C���p���X�t�@�C����ǂݍ��ށB
 * @param (WCHAR* strpath) IN �ǂݍ��ރt�@�C���̃p�X�B
 * @param (CModel* srcmodel) IN �ǂݍ��񂾃C���p���X�p�����[�^��ݒ肷�郂�f���B
 * @return ����������O�B
 */
	int LoadImpFile( WCHAR* strpath, CModel* srcmodel );


private:

/**
 * @fn
 * InitParams
 * @breaf �����o������������B
 * @return �O�B
 */
	virtual int InitParams();
	
/**
 * @fn
 * DestroyObjs
 * @breaf �A���P�[�g�������������J������B
 * @return �O�B
 */
	virtual int DestroyObjs();

/**
 * @fn
 * WriteImpReq
 * @breaf �C���p���X�f�[�^�����o���֐����{�[���Ɋւ��čċA�I�ɌĂяo���B
 * @param (CBone* srcbone) IN �C���p���X�p�����[�^�����{�[���B
 * @return �Ȃ��B
 * @detail ���ꂼ��̃{�[���ɂ��Ă̏����o���֐���WriteImp�ł���B
 */
	void WriteImpReq( CBone* srcbone );

/**
 * @fn
 * WriteImp
 * @breaf ���ꂼ��̃{�[���ɂ��ăC���p���X�f�[�^�������o���B
 * @param (CBone* srcbone) IN �C���p���X�p�����[�^�����{�[���B
 * @return ����������O�B
 */
	int WriteImp( CBone* srcbone );


/**
 * @fn
 * ReadBone
 * @breaf �{�[�����Ƃ̓ǂݍ��݊֐��B
 * @param (XMLIOBUF* xmliobuf) IN XML�̂���{�[���P���̃o�b�t�@�B
 * @return ����������O�B
 */
	int ReadBone(XMLIOBUF* xmliobuf, CModel* srcmodel);

/**
 * @fn
 * ReadRE
 * @breaf ���̂��Ƃ̓ǂݍ��݊֐��B
 * @param (XMLIOBUF* xmliobuf) IN XML�̂��鍄�̂ɂP���̃o�b�t�@�B
 * @param (CBone* srcbone) IN �C���p���X�p�����[�^�����{�[���B
 * @return ����������O�B
 */
	int ReadRE(XMLIOBUF* xmliobuf, CModel* srcmodel, CBone* srcbone);


private:
	std::string m_strimp;//�C���p���X�t�@�C�����B
	CModel* m_model;

};

#endif