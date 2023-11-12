#ifndef EXTLINEH
#define EXTLINEH

#include <coef.h>
//#include <D3DX9.h>
#include <ChaVecCalc.h>

class CMQOFace;

class CExtLine
{
public:

/**
 * @fn
 * CExtLine
 * @breaf �R���X�g���N�^�B
 * @return �Ȃ��B
 */
	CExtLine();

/**
 * @fn
 * ~CExtLine
 * @breaf �f�X�g���N�^�B
 * @return �Ȃ��B
 */
	~CExtLine();

/**
 * @fn
 * CreateExtLine
 * @breaf �����f�[�^���쐬����B
 * @param (int pointnum) IN ���_�f�[�^pointptr�̒��_���B
 * @param (int facenum) IN �ʃf�[�^faceptr�̖ʐ��B
 * @param (ChaVector3* pointptr) IN ���_�f�[�^�B
 * @param (CMQOFace* faceptr) IN �ʃf�[�^�B
 * @param (ChaVector4 srccol) IN �����̐F�B
 * @return ����������O�B
 */
	int CreateExtLine( int pointnum, int facenum, ChaVector3* pointptr, CMQOFace* faceptr, ChaVector4 srccol );


/**
 * @fn
 * CalcBound
 * @breaf �������͂����ރo�E���_���[�f�[�^���쐬����B
 * @return ����������O�B
 */
	int CalcBound();


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


/**
 * @fn
 * CreateBuffer
 * @breaf ���C���o�b�t�@���쐬���ăZ�b�g����B
 * @param (EXTLINEV* lineptr) IN,OUT ���̃o�b�t�@�ɐ����f�[�^���Z�b�g����B
 * @param (int arrayleng) IN lineptr�̐������B
 * @param (int* setnum) OUT �Z�b�g�����������B
 * @return ����������O�B
 * @detail �܂��K�v�ȃf�[�^�����m���߂邽�߂�lineptr�ɂO���Z�b�g���ČĂԁBsetnum�̒l���݂�lineptr���A���P�[�g���Alineptr�������Ɏw�肵�Ă�����x�Ăяo���B
 */
	int CreateBuffer( EXTLINEV* lineptr, int arrayleng, int* setnum );


public:
	int m_linenum;//�����̐��B
	EXTLINEV* m_linev;//���_�f�[�^�B
	ChaVector4 m_color;//�����̐F
	MODELBOUND	m_bound;//�������͂����ރo�E���_���[�f�[�^�B

private:
	//�ȉ��AMQOObject�ŃA���P�[�g�����O���|�C���^�ƃf�[�^�B
	int m_pointnum;//���_���B
	int m_facenum;//�ʁi�����j�̐��B
	ChaVector3* m_pointptr;//���_�f�[�^�B
	CMQOFace* m_faceptr;//��(����)�f�[�^�B
};


#endif