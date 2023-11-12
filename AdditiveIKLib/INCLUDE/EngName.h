#ifndef ENGNAMEH
#define ENGNAMEH

enum {
	ENGNAME_DISP,
	ENGNAME_BONE,
	ENGNAME_MOTION,
	ENGNAME_MAX
};

#ifdef ENGNAMECPP

/**
 * @fn
 * ConvEngName
 * @breaf �p�����A���_�[�o�[�ȊO�̕��������O�ɂ���ꍇ�A�����u�������ĉp��\���o���閼�O�����B
 * @param (int type) IN �\���I�u�W�F�N�g��ENGNAME_DISP, �{�[����ENGNAME_BONE, ���[�V��������ENGNAME_MOTION���w�肷��B
 * @param (char* srcname) IN ���̖��O�B�Q�T�U�o�C�g���̃o�b�t�@�B
 * @param (int srcleng) IN 256���w�肷��B�o�b�t�@�̒������Q�T�U���Œ�ł��邱�Ƃ̊m�F�̂��߂̈����ł���B
 * @param (char* dstname) OUT �ϊ���̖��O�B�Q�T�U�o�C�g���̃o�b�t�@�B
 * @param (int dstleng) IN 256���w�肷��B�o�b�t�@�̒������Q�T�U���Œ�ł��邱�Ƃ̊m�F�̂��߂̈����ł���B
 * @return ����������O�B
 * @detail [L], [R]��_L_, _R_�ɂ���B�{�[���ɂ�_Joint��t����B���[�V�����ɂ�_Motion��t����B�p�����ȊO������ꍇ�́uReplacedName�v�Ɛ����̑g�ݍ��킹�̖��O���쐬����B
 */
	int ConvEngName( int type, char* srcname, int srcleng, char* dstname, int dstleng );


	int TermJointRepeats(char* dstname);


#else
	extern int ConvEngName( int type, char* srcname, int srcleng, char* dstname, int dstleng );
	extern int TermJointRepeats(char* dstname);
#endif

#endif