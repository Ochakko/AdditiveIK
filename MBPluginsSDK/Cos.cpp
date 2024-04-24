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

#include <crtdbg.h>// <--- _ASSERTマクロ


#define PI          3.141592f


//---------------------------------------------------------------------------
//  DllMain
//---------------------------------------------------------------------------
//BOOL APIENTRY DllMain( HANDLE hModule, 
//                       DWORD  ul_reason_for_call, 
//                       LPVOID lpReserved
//					 )
//{
//	//ダイアログボックスの表示に必要なので、インスタンスを保存しておく
//	hInstance = (HINSTANCE)hModule;
//
//	//プラグインとしては特に必要な処理はないので、何もせずにTRUEを返す
//    return TRUE;
//}


//---------------------------------------------------------------------------
//  MBGetPlugInID
//    プラグインIDを返す。
//    この関数は起動時に呼び出される。
//---------------------------------------------------------------------------
MBPLUGIN_EXPORT int MBGetPlugInID(DWORD* Product, DWORD* ID)
{
	// プロダクト名(制作者名)とIDを、全部で64bitの値として返す
	// 値は他と重複しないようなランダムなもので良い

	char* cproduct;
	cproduct = (char*)Product;
	*cproduct = 'o';
	*(cproduct + 1) = 'c';
	*(cproduct + 2) = 'h';
	*(cproduct + 3) = 'a';

	*ID = 0x00000002;

	return 0;
}


//---------------------------------------------------------------------------
//  MBGetPlugInName
//    プラグイン名を返す。
//    AdditiveIKのブラシの種類のコンボボックスに、この名前が表示される。
//---------------------------------------------------------------------------
MBPLUGIN_EXPORT const WCHAR* MBGetPlugInName(void)
{
	// プラグイン名
	return L"-CosX";
}

//---------------------------------------------------------------------------
//  MBOnClose
//    AdditiveIKが終了する直前にAdditiveIKから呼ばれる。
//---------------------------------------------------------------------------
MBPLUGIN_EXPORT int MBOnClose(void)
{
	return 0;
}

//---------------------------------------------------------------------------
//  MBOnPose
//   現在未使用
//---------------------------------------------------------------------------
MBPLUGIN_EXPORT int MBOnPose(int motid)
{
	return 0;
}


//----------------------------------------------------------------------------
//  MBOnSelectPlugin
//  AdditiveIKで複数フレームを選択した時、ブラシの種類を選んだ時、ブラシのパラメータを変えたときに呼ばれます。
//  選択された複数フレーム分のブラシ値を計算して配列dstvalueにセットします。
//----------------------------------------------------------------------------
MBPLUGIN_EXPORT int MBCreateMotionBrush(double srcstartframe, double srcendframe, double srcapplyframe, double srcframeleng, int srcrepeats, int srcmirroru, int srcmirrorv, int srcdiv2, float* dstvalue)
{
	int MB2version;
	if (MBGetVersion) {
		MBGetVersion(&MB2version);
		//Write2File( L"AdditiveIK version : %d\r\n", MB2version );
	}

	if ((srcstartframe >= 0.0) && (srcstartframe < 1e5) && (srcendframe >= srcstartframe) && (srcendframe < 1e5) &&
		(srcapplyframe >= srcstartframe) && (srcapplyframe <= srcendframe) &&
		(srcframeleng > srcendframe) && (srcframeleng < 1e5) && 
		//(srcrepeats >= 1) && (srcrepeats <= 10) &&
		(srcrepeats >= 1) && (srcrepeats <= 100) && //2024/04/24 repeats max 100
		dstvalue) {


		//rounding value
		int rstartframe, rendframe, rapplyframe, rframeleng;
		rstartframe = (int)(srcstartframe + 0.0001);
		rendframe = (int)(srcendframe + 0.0001);
		rapplyframe = (int)(srcapplyframe + 0.0001);
		rframeleng = (int)(srcframeleng + 0.0001);



		int numframe = rendframe - rstartframe + 1;
		::memset(dstvalue, 0, sizeof(float) * (size_t)rframeleng);


		//１周期が３フレーム以上になるように実際の繰り返し回数と周期を調整する
		int repeats;
		repeats = srcrepeats;
		int frameT;//周期(フレーム数)
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

				startframe = (int)((size_t)rstartframe + (size_t)repeatscnt * (size_t)frameT);
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
						//	//矩形以外　両端０
						//	curscale = 0.0;
						//}
						if ((framecnt == startframe) && (framecnt != applyframe)) {
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
						else if ((framecnt == endframe) && (framecnt != applyframe)) {
							if (repeatscnt == (repeats - 1)) {
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
						else if (framecnt < applyframe) {
							if (repeatscnt == 0) {

								//repeatscnt == 0 でminusvは無い

								if (div2) {
									curscale = (float)(cos(PI + PI * (((double)framecnt - (double)startframe) / (double)halfcnt1)));
									curscale = (curscale + 1.0f) * 0.5f;
								}
								else {
									curscale = (float)((1.0 + cos(PI + PI * (((double)framecnt - (double)startframe) / (double)halfcnt1))) * 0.5);
									if (minusv) {
										curscale *= -1.0f;
									}
								}
							}
							else {
								curscale = (float)((1.0 + cos(PI + PI * (((double)framecnt - (double)startframe) / (double)halfcnt1))) * 0.5);
								if (minusv) {
									curscale *= -1.0f;
								}
								if (div2) {
									curscale = (curscale + 1.0f) * 0.5f;
								}
							}
						}
						else if ((framecnt > applyframe) && (framecnt < endframe)) {
							if (repeatscnt == (repeats - 1)) {
								if (div2) {
									if (minusv) {
										//minus div2 : applyframeのとき０　そこから　０へ戻るには０
										curscale = 0.0f;
									}
									else {
										curscale = (float)(cos(PI + PI * (((double)endframe - (double)framecnt) / (double)halfcnt2)));
										curscale = (curscale + 1.0f) * 0.5f;
									}
								}
								else {
									curscale = (float)((1.0 + cos(PI + PI * (((double)endframe - (double)framecnt) / (double)halfcnt2))) * 0.5);
									if (minusv) {
										curscale *= -1.0f;
									}
								}
							}
							else {
								curscale = (float)((1.0 + cos(PI + PI * (((double)endframe - (double)framecnt) / (double)halfcnt2))) * 0.5);
								if (minusv) {
									curscale *= -1.0f;
								}
								if (div2) {
									curscale = (curscale + 1.0f) * 0.5f;
								}
							}
						}
						else if (framecnt == applyframe) {
							curscale = 1.0f;
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
						//選択範囲以外０
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

