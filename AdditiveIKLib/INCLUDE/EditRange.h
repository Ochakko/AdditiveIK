#ifndef EDITRANGEH
#define EDITRANGEH

#include <list>
#include <orgwindow.h>

class CEditRange
{
public:

/**
 * @fn
 * CEditRange
 * @breaf �R���X�g���N�^
 * @return �Ȃ��B
 */
	CEditRange();

/**
 * @fn
 * ~CEditRange
 * @breaf �f�X�g���N�^
 * @return �Ȃ��B
 */
	~CEditRange();

/**
 * @fn
 * Clear
 * @breaf �����o�����Z�b�g����B
 * @return ����������O�B
 */
	int Clear();

/**
 * @fn
 * SetRange
 * @breaf �ҏW�͈͂��Z�b�g����B
 * @param (std::list<KeyInfo> srcki) IN �ҏW�����L�[���L�^���ꂽ���X�g�B
 * @param (double srcframe) IN �P�t���[���݂̂̕ҏW���ɂ͂��̃t���[���B
 * @return ����������O�B
 */
	int SetRange( std::list<KeyInfo> srcki, double srcframe );


	int SetRangeOne(double srcframe);

/**
 * @fn
 * GetRange
 * @breaf �ҏW�͈͏����擾����B
 * @param (int* numptr) OUT �ҏW�͈͂̃L�[�̐��B
 * @param (double* startptr) OUT�@�ҏW�͈͂̊J�n�t���[���B
 * @param (double* endptr) OUT �ҏW�͈͂̏I���t���[���B
 * @return ����������O�B
 */
	int GetRange( int* numptr, double* startptr, double* endptr );

/**
 * @fn
 * GetRange
 * @breaf �ҏW�͈͏����擾����B
 * @param (int* numptr) OUT �ҏW�͈͂̃L�[�̐��B
 * @param (double* startptr) OUT�@�ҏW�͈͂̊J�n�t���[���B
 * @param (double* endptr) OUT �ҏW�͈͂̏I���t���[���B
 * @param (double* applyptr) OUT �p���K�p�t���[���B
 * @return ����������O�B
 * @detail �I��͈͂̊J�n�ʒu���牽���̈ʒu�Ɏp����K�p���邩�̏�񂩂�A�p���K�p�t���[���̌v�Z���s���B
 */
	int GetRange( int* numptr, double* startptr, double* endptr, double* applyptr );


	CEditRange operator= (CEditRange srcrange);
	bool operator== (const CEditRange &er) const { return ((m_startframe == er.m_startframe) && (m_endframe == er.m_endframe) && (m_applyframe == er.m_applyframe)); };
	bool operator!= (const CEditRange &er) const { return !(*this == er); };


	int GetSetFlag(){ return m_setflag; };
	int GetKeyNum(){ return m_keynum; };
	KeyInfo GetKeyInfo(int srcindex){ 
		KeyInfo initki;
		if ((srcindex < 0) || (srcindex >= (int)m_ki.size())){
			return initki;
		}
		int curindex = 0;
		std::list<KeyInfo>::iterator itrki;
		std::list<KeyInfo>::iterator findki = m_ki.end();
		for (itrki = m_ki.begin(); itrki != m_ki.end(); itrki++){
			if (curindex == srcindex){
				findki = itrki;
				break;
			}
			curindex++;
		}
		if (findki != m_ki.end()){
			return *findki;
		}
		else{
			return initki;
		}
	};
	double GetStartFrame(){ return m_startframe; };
	double GetEndFrame(){ return m_endframe; };
	double GetApplyFrame(){ return m_applyframe; };
	int IsSameStartAndEnd(){ return (m_startframe == m_endframe); };

	void SetSetFlag(int srcflag){ m_setflag = srcflag; };
	void SetSetCnt(int srccnt){ m_setcnt = srccnt; };

	static void SetApplyRate(double srcrate){
		s_applyrate = srcrate; 
	};

private:

/**
* @fn
* InitParams
* @breaf �����o�̏������B
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


private:
	int m_setflag;
	int m_setcnt;
	std::list<KeyInfo> m_ki;//�ҏW�͈͂̃L�[�̏��
	int m_keynum;
	double m_startframe;
	double m_endframe;
	double m_applyframe;//�p���K�p�t���[���B

	static double s_applyrate;//�p���K�p�t���[�����I��͈͂̊J�n�ʒu���牽���̂Ƃ���ɂ��邩�B
};

#endif



