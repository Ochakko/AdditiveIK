#ifndef GCOLIFILEH
#define GCOLIFILEH

//#include <d3dx9.h>
#include <ChaVecCalc.h>
#include <coef.h>
#include <XMLIO.h>

#include <map>
#include <string>

class BPWorld;
class CModel;

/*
bullet�ɂ�铖���蔻��t���̒n�ʃf�[�^�̓��o�͗p�N���X�B
*/

class CGColiFile : public CXMLIO
{
public:

/**
 * @fn
 * CGColiFile
 * @breaf �R���X�g���N�^
 * @return �Ȃ��B
 */
	CGColiFile();
	
/**
 * @fn
 * ~CGColiFile
 * @breaf �f�X�g���N�^�B
 * @return �Ȃ��B
 * @detail �h���N���X�̃f�X�g���N�^�Ȃ̂�virtual�B
 */	
	virtual ~CGColiFile();


/**
 * @fn
 * WriteGColiFile
 * @breaf �n�ʃt�@�C�����o�͂���B
 * @param (WCHAR* strpath) IN �����o���t�@�C���̃p�X�B
 * @param (BPWorld* srcbpw) IN �����Ǘ��N���X�B
 * @return ����������O�B
 */
	int WriteGColiFile( WCHAR* strpath, BPWorld* srcbpw );
	
/**
 * @fn
 * LoadGColiFile
 * @breaf �n�ʃt�@�C����ǂݍ��ށB
 * @param (CModel* srcmodel) IN �n�ʂ̌`��f�[�^�B
 * @param (WCHAR* strpath) IN �����o���t�@�C���̃p�X�B
 * @param (BPWorld* srcbpw) IN �����Ǘ��N���X�B
 * @return ����������O�B
 */
	int LoadGColiFile( CModel* srcmodel, WCHAR* strpath, BPWorld* srcbpw );

private:

/**
 * @fn
 * InitParams
 * @breaf �����o�̏������B
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
 * WriteColiID
 * @breaf �n�ʂ̃p�����[�^���t�@�C���ɏ����o���B
 * @return ����������O�B
 */
	int WriteColiID();


/**
 * @fn
 * ReadColiID
 * @breaf �n�ʃt�@�C���̓��e��ǂݍ��ށB
 * @param (XMLIOBUF* xmliobuf) IN XML�ǂݍ��ݗp�o�b�t�@�B
 * @return ����������O�B
 */
	int ReadColiID( XMLIOBUF* xmliobuf );

private:
	BPWorld* m_bpw;//�����Ǘ��N���X�B
	CModel* m_model;//�n�ʌ`��f�[�^�̊O���|�C���^�B

};

#endif