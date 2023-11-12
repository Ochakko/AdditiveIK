#include "pch.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <math.h>
#include <Commdlg.h>

#include "MBPlugin.h"

#include <crtdbg.h>// <--- _ASSERT�}�N��


#define PI          3.141592f


//---------------------------------------------------------------------------
//  DllMain
//---------------------------------------------------------------------------
//BOOL APIENTRY DllMain( HANDLE hModule, 
//                       DWORD  ul_reason_for_call, 
//                       LPVOID lpReserved
//					 )
//{
//	//�_�C�A���O�{�b�N�X�̕\���ɕK�v�Ȃ̂ŁA�C���X�^���X��ۑ����Ă���
//	hInstance = (HINSTANCE)hModule;
//
//	//�v���O�C���Ƃ��Ă͓��ɕK�v�ȏ����͂Ȃ��̂ŁA����������TRUE��Ԃ�
//    return TRUE;
//}


//---------------------------------------------------------------------------
//  MBGetPlugInID
//    �v���O�C��ID��Ԃ��B
//    ���̊֐��͋N�����ɌĂяo�����B
//---------------------------------------------------------------------------
MBPLUGIN_EXPORT int MBGetPlugInID(DWORD *Product, DWORD *ID)
{
	// �v���_�N�g��(����Җ�)��ID���A�S����64bit�̒l�Ƃ��ĕԂ�
	// �l�͑��Əd�����Ȃ��悤�ȃ����_���Ȃ��̂ŗǂ�

	char* cproduct;
	cproduct = (char*)Product;
	*cproduct = 'o';
	*( cproduct + 1 ) = 'c';
	*( cproduct + 2 ) = 'h';
	*( cproduct + 3 ) = 'a';

	*ID      = 0x00000005;

	return 0;
}


//---------------------------------------------------------------------------
//  MBGetPlugInName
//    �v���O�C������Ԃ��B
//    EditMot�̃u���V�̎�ނ̃R���{�{�b�N�X�ɁA���̖��O���\�������B
//---------------------------------------------------------------------------
MBPLUGIN_EXPORT const WCHAR* MBGetPlugInName(void)
{
	// �v���O�C����
	return L"Up_Linear";
}

//---------------------------------------------------------------------------
//  MBOnClose
//    EditMot���I�����钼�O��EditMot����Ă΂��B
//---------------------------------------------------------------------------
MBPLUGIN_EXPORT int MBOnClose(void)
{
	return 0;
}

//---------------------------------------------------------------------------
//  MBOnPose
//   ���ݖ��g�p
//---------------------------------------------------------------------------
MBPLUGIN_EXPORT int MBOnPose( int motid )
{
	return 0;
}


//----------------------------------------------------------------------------
//  MBOnSelectPlugin
//  EditMot�ŕ����t���[����I���������A�u���V�̎�ނ�I�񂾎��A�u���V�̃p�����[�^��ς����Ƃ��ɌĂ΂�܂��B
//  �I�����ꂽ�����t���[�����̃u���V�l���v�Z���Ĕz��dstvalue�ɃZ�b�g���܂��B
//----------------------------------------------------------------------------

MBPLUGIN_EXPORT int MBCreateMotionBrush(double srcstartframe, double srcendframe, double srcapplyframe, double srcframeleng, int srcrepeats, int srcmirroru, int srcmirrorv, int srcdiv2, float* dstvalue)
{
	int MB2version;
	if (MBGetVersion) {
		MBGetVersion(&MB2version);
		//Write2File( L"EditMot version : %d\r\n", MB2version );
	}

	if ((srcstartframe >= 0.0) && (srcstartframe < 1e5) && (srcendframe >= srcstartframe) && (srcendframe < 1e5) &&
		(srcapplyframe >= srcstartframe) && (srcapplyframe <= srcendframe) &&
		(srcframeleng > srcendframe) && (srcframeleng < 1e5) && 
		(srcrepeats >= 1) && (srcrepeats <= 10) &&
		dstvalue) {

		//rounding value
		int rstartframe, rendframe, rapplyframe, rframeleng;
		rstartframe = (int)(srcstartframe + 0.0001);
		rendframe = (int)(srcendframe + 0.0001);
		rapplyframe = (int)(srcapplyframe + 0.0001);
		rframeleng = (int)(srcframeleng + 0.0001);

		int numframe = rendframe - rstartframe + 1;
		::memset(dstvalue, 0, sizeof(float) * (size_t)rframeleng);


		//�P�������R�t���[���ȏ�ɂȂ�悤�Ɏ��ۂ̌J��Ԃ��񐔂Ǝ����𒲐�����
		int repeats;
		repeats = srcrepeats;
		int frameT;//����(�t���[����)
		frameT = (int)(numframe / srcrepeats);
		while ((frameT >= (rapplyframe + 1)) && (frameT < 3) && (repeats >= 2)) {
			repeats--;
			frameT = (int)(numframe / srcrepeats);
		}

		if (frameT >= 3) {
			int halfcnt1, halfcnt2;


			int repeatscnt;
			for (repeatscnt = 0; repeatscnt < repeats; repeatscnt++) {
				int startframe;
				int endframe;
				int applyframe;
				bool invu;
				bool minusv;
				bool div2;

				startframe = rstartframe + repeatscnt * frameT;
				endframe = startframe + frameT;
				applyframe = startframe + (rapplyframe - rstartframe);
				if (srcmirroru) {
					if ((repeatscnt % 2) == 0) {
						invu = false;
					}
					else {
						invu = true;
					}
				}
				else {
					invu = false;
				}
				if (srcmirrorv) {
					div2 = srcdiv2;
					if ((repeatscnt % 2) == 0) {
						minusv = false;
					}
					else {
						minusv = true;
					}
				}
				else {
					div2 = false;
					minusv = false;
				}



				if (invu) {
					applyframe = startframe + (endframe - applyframe);
				}

				int framecnt;
				halfcnt1 = (applyframe - startframe);
				halfcnt2 = (endframe - applyframe);

				for (framecnt = startframe; framecnt <= endframe; framecnt++) {
					float curscale;
					if ((framecnt >= startframe) && (framecnt <= endframe)) {
						//if ((framecnt == startframe) || (framecnt == endframe)) {
						//	//��`�ȊO�@���[�O
						//	curscale = 0.0;
						//}
						if ((framecnt == startframe) && (framecnt != applyframe)) {
							if (invu) {
								if ((repeatscnt % 2) == 0) {
									curscale = 0.0f;
								}
								else {
									if (minusv) {
										if (div2) {
											curscale = 0.0f;
										}
										else {
											curscale = -1.0f;
										}
									}
									else {
										curscale = 1.0f;
									}
								}
							}
							else {
								if (repeatscnt == 0) {
									curscale = 0.0f;
								}
								else {
									if (div2) {
										curscale = 0.5f;
									}
									else {
										curscale = 0.0f;
									}
								}
							}
						}
						else if ((framecnt == endframe) && (framecnt != applyframe)) {
							if (invu) {
								if (minusv) {
									if (repeatscnt == (repeats - 1)) {
										curscale = 0.0f;
									}
									else {
										if ((repeatscnt % 2) == 0) {
											curscale = 1.0f;
										}
										else {
											if (div2) {
												curscale = 0.5f;
											}
											else {
												curscale = 1.0f;
											}
										}
									}
								}
								else {
									curscale = 0.0f;
								}
							}
							else {
								if (minusv) {
									if (div2) {
										curscale = 0.0f;
									}
									else {
										curscale = -1.0f;
									}
								}
								else {
									curscale = 1.0f;
								}
							}
						}
						else if (framecnt < applyframe) {
							if (repeatscnt == 0) {
								
								//repeatscnt == 0 ��minusv�͖���
								curscale = (float)(((double)framecnt - (double)startframe) / (double)halfcnt1);
							}
							else {
								if (invu) {
									if ((repeatscnt % 2) == 0) {
										curscale = (float)(((double)framecnt - (double)startframe) / (double)halfcnt1);
									}
									else {
										curscale = 1.0f;
									}
									if (minusv) {
										curscale *= -1.0f;
									}
									if (div2) {
										curscale = (curscale + 1.0f) * 0.5f;
									}
								}
								else {
									curscale = (float)(((double)framecnt - (double)startframe) / (double)halfcnt1);
									if (minusv) {
										curscale *= -1.0f;
									}
									if (div2) {
										curscale = (curscale + 1.0f) * 0.5f;
									}
								}
							}
						}
						else if ((framecnt > applyframe) && (framecnt < endframe)) {
							if (repeatscnt == (repeats - 1)) {
								if (invu) {
									if ((repeatscnt % 2) == 0) {
										if (minusv) {
											if (div2) {
												curscale = 0.0f;
											}
											else {
												curscale = -1.0f;
											}
										}
										else {
											curscale = 1.0f;
										}
									}
									else {
										curscale = 1.0f - (float)(((double)endframe - (double)framecnt) / (double)halfcnt2);
										curscale = curscale * -1.0f + 1.0f;
										if (minusv) {
											curscale *= -1.0f;
										}
										if (div2) {
											curscale = (curscale + 1.0f) * 0.5f;
										}
									}
								}
								else {
									if (minusv) {
										if (div2) {
											curscale = 0.0f;
										}
										else {
											curscale = -1.0f;
										}
									}
									else {
											curscale = 1.0f;
									}
								}
							}
							else {
								if (invu) {
									if ((repeatscnt % 2) == 0) {
										if (minusv) {
											if (div2) {
												curscale = 0.0f;
											}
											else {
												curscale = -1.0f;
											}
										}
										else {
											curscale = 1.0f;
										}
									}
									else {
										curscale = 1.0f - (float)(((double)endframe - (double)framecnt) / (double)halfcnt2);
										curscale = curscale * -1.0f + 1.0f;
										if (minusv) {
											curscale *= -1.0f;
										}
										if (div2) {
											curscale = (curscale + 1.0f) * 0.5f;
										}
									}
								}
								else {
									if (minusv) {
										if (div2) {
											curscale = 0.0f;
										}
										else {
											curscale = -1.0f;
										}
									}
									else {
										curscale = 1.0f;
									}
								}
							}
						}
						else if (framecnt == applyframe) {
							curscale = 1.0;
							if (minusv) {
								curscale *= -1.0f;
							}
							if (div2) {
								curscale = (curscale + 1.0f) * 0.5f;
							}
						}
						else {
							_ASSERT(0);
							curscale = 0.0f;
						}
					}
					else {
						//�I��͈͈ȊO�O
						curscale = 0.0f;
					}
					*(dstvalue + framecnt) = curscale;
				}
			}
		}
		else {
			int framecnt;
			for (framecnt = 0; framecnt < rframeleng; framecnt++) {
				float curscale;
				if ((framecnt >= rstartframe) && (framecnt <= rendframe)) {
					curscale = 1.0f;
				}
				else {
					curscale = 0.0f;
				}
				*(dstvalue + framecnt) = curscale;
			}
		}
	}
	else {
		_ASSERT(0);
		return 1;
	}


	return 0;
}


