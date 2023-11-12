#ifndef LMTFILEH
#define LMTFILEH

//#include <d3dx9.h>
#include <ChaVecCalc.h>
#include <Coef.h>
#include <XMLIO.h>

#include <map>
#include <string>

class CModel;
class CMQOObject;
class CBone;
class CRigidElem;


/*
�����p�x�t�@�C���̃N���X�B
Imp��impulse��imp�B
*/


class CLmtFile : public CXMLIO
{
public:

/**
 * @fn
 * CLmtFile
 * @breaf �R���X�g���N�^�B
 * @return �Ȃ��B
 */
	CLmtFile();

/**
 * @fn
 * ~CLmtFile
 * @breaf �f�X�g���N�^�B
 * @return �Ȃ��B
 * @detail �h���N���X�̃f�X�g���N�^�Ȃ̂�virtual�B
 */
	virtual ~CLmtFile();


/**
 * @fn
 * WriteLmtFile
 * @breaf �����p�x�t�@�C���������o���B
 * @param (WCHAR* strpath) IN �����o���t�@�C���̃p�X�B
 * @param (CModel* srcmodel) IN �����o�����f���f�[�^�B
 * @return ����������O�B
 * @detail srcmodel�ɐݒ肳��Ă��鐧���p�x�������o���B
 */
	int WriteLmtFile( WCHAR* strpath, CModel* srcmodel, char* fbxcomment);


/**
 * @fn
 * LoadLmtFile
 * @breaf �����p�x�t�@�C����ǂݍ��ށB
 * @param (WCHAR* strpath) IN �ǂݍ��ރt�@�C���̃p�X�B
 * @param (CModel* srcmodel) IN �ǂݍ��񂾐����p�x�p�����[�^��ݒ肷�郂�f���B
 * @return ����������O�B
 */
	int LoadLmtFile( WCHAR* strpath, CModel* srcmodel, char* fbxcomment );


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
 * WriteLmtReq
 * @breaf �����p�x�����o���֐����{�[���Ɋւ��čċA�I�ɌĂяo���B
 * @param (CBone* srcbone) IN �����p�x�p�����[�^�����{�[���B
 * @return �Ȃ��B
 * @detail ���ꂼ��̃{�[���ɂ��Ă̏����o���֐���WriteImp�ł���B
 */
	void WriteLmtReq(bool limitdegflag, CBone* srcbone);

/**
 * @fn
 * WriteLmt
 * @breaf ���ꂼ��̃{�[���ɂ��Đ����p�x�f�[�^�������o���B
 * @param (CBone* srcbone) IN �����p�x�p�����[�^�����{�[���B
 * @return ����������O�B
 */
	int WriteLmt(bool limitdegflag, CBone* srcbone);


/**
 * @fn
 * ReadBone
 * @breaf �{�[�����Ƃ̓ǂݍ��݊֐��B
 * @param (XMLIOBUF* xmliobuf) IN XML�̂���{�[���P���̃o�b�t�@�B
 * @return ����������O�B
 */
	int ReadBone(bool limitdegflag, XMLIOBUF* xmliobuf, CModel* srcmodel);

/**
 * @fn
 * ReadRE
 * @breaf ���̂��Ƃ̓ǂݍ��݊֐��B
 * @param (XMLIOBUF* xmliobuf) IN XML�̂��鍄�̂ɂP���̃o�b�t�@�B
 * @param (CBone* srcbone) IN �����p�x�p�����[�^�����{�[���B
 * @return ����������O�B
 */
	//int ReadRE( XMLIOBUF* xmliobuf, CBone* srcbone );



private:
	CModel* m_model;
	ANGLELIMIT m_anglelimit;

};

#endif