#include "stdafx.h"

#include <crtdbg.h>
#include <OrgWindow.h>

#include <windows.h>
#include <list>
#include <map>

#include "GlobalVar.h"
#include <Model.h>

using namespace std;

extern std::vector<void*> g_eulpool;//allocate EULPOOLBLKLEN motoinpoints at onse and pool 
extern std::vector<void*> g_keypool;//allocate KEYPOOLBLKLEN motoinpoints at onse and pool 

extern LONG g_bvh2fbxbatchflag;
//extern LONG g_motioncachebatchflag;
extern LONG g_retargetbatchflag;

namespace OrgWinGUI{

	////////////////----------------------------------------////////////////
	////////////////										////////////////
	////////////////		ウィンドウ内部品クラス			////////////////
	////////////////										////////////////
	////////////////----------------------------------------////////////////

	void OrgWindow::allPaint() {
		//static int s_paintcnt = 0;
		//s_paintcnt++;
		//if (g_previewFlag != 0) {
		//	if ((s_paintcnt % 60) != 0) {
		//		return;
		//	}
		//}

		//if (g_bvh2fbxbatchflag || g_motioncachebatchflag || g_retargetbatchflag) {
		//	return;
		//}

		beginPaint();
		paintTitleBar();

		//if (g_retargetbatchflag == 0) {
		//if ((InterlockedAdd(&g_retargetbatchflag, 0) == 0) && (InterlockedAdd(&g_bvh2fbxbatchflag, 0) == 0) && (InterlockedAdd(&g_motioncachebatchflag, 0) == 0)) {
		if ((InterlockedAdd(&g_retargetbatchflag, 0) == 0) && (InterlockedAdd(&g_bvh2fbxbatchflag, 0) == 0)) {
			std::list<OrgWindowParts*>::iterator itr;
			for (itr = partsList.begin(); itr != partsList.end(); itr++) {
				if ((*itr)->getParent() && IsWindow((*itr)->getParent()->hWnd)) {
					(*itr)->draw();
				}
			}
		}
		//{
		//	//if (g_dsmousewait == 1) {
		//	if (g_mouseherebmp) {
		//		POINT mousepoint;
		//		::GetCursorPos(&mousepoint);
		//		::ScreenToClient(hWnd, &mousepoint);

		//		//PAINTSTRUCT ps;
		//		//HDC hdc = BeginPaint(hWnd, &ps);
		//		// メモリデバイスコンテキストを作成する
		//		HDC hCompatDC = CreateCompatibleDC(hdcM.hDC);
		//		// ロードしたビットマップを選択する
		//		HBITMAP hPrevBitmap = (HBITMAP)SelectObject(hCompatDC, g_mouseherebmp);
		//		BITMAP bmp;
		//		GetObject(g_mouseherebmp, sizeof(BITMAP), &bmp);
		//		int BMP_W = (int)bmp.bmWidth;
		//		int BMP_H = (int)bmp.bmHeight;
		//		BitBlt(hdcM.hDC, mousepoint.x, mousepoint.y, BMP_W, BMP_H, hCompatDC, 0, 0, SRCCOPY);
		//		DeleteDC(hCompatDC);
		//		//EndPaint(hWnd, &ps);
		//	}
		//	//}
		//}


		//{
		//	//if (g_dsmousewait == 1) {
		//	POINT mousepoint;
		//	::GetCursorPos(&mousepoint);
		//	//if (getParent()) {
		//		::ScreenToClient(getHWnd(), &mousepoint);
		//		PAINTSTRUCT ps;
		//		HDC hdc = BeginPaint(getHWnd(), &ps);
		//		// メモリデバイスコンテキストを作成する
		//		HDC hCompatDC = CreateCompatibleDC(hdc);
		//		// ロードしたビットマップを選択する
		//		HBITMAP hPrevBitmap = (HBITMAP)SelectObject(hCompatDC, g_mouseherebmp);
		//		BITMAP bmp;
		//		GetObject(g_mouseherebmp, sizeof(BITMAP), &bmp);
		//		int BMP_W = (int)bmp.bmWidth;
		//		int BMP_H = (int)bmp.bmHeight;
		//		BitBlt(hdc, mousepoint.x, mousepoint.y, BMP_W, BMP_H, hCompatDC, 0, 0, SRCCOPY);
		//		DeleteDC(hCompatDC);
		//		//EndPaint(hWnd, &ps);
		//	//}
		//	//}
		//}

		endPaint();
	}

	void OrgWindowParts::draw()
	{
		if (!hdcM) {
			return;
		}

		drawEdge();
		{
			if (g_dsmousewait == 1) {
				POINT mousepoint;
				::GetCursorPos(&mousepoint);
				if (getParent() && getHDCMaster()) {
					::ScreenToClient(getParent()->getHWnd(), &mousepoint);
					int BMP_W = 52;
					int BMP_H = 50;
					Gdiplus::Graphics* gdipg = new Gdiplus::Graphics(hdcM->hDC);
					if (gdipg) {
						Gdiplus::ImageAttributes attr;
						Gdiplus::ColorMatrix cmat = {
							1.0f, 0.0f, 0.0f, 0.0f, 0.0f,   // Red
							0.0f, 1.0f, 0.0f, 0.0f, 0.0f,   // Green
							0.0f, 0.0f, 1.0f, 0.0f, 0.0f,   // Blue
							0.0f, 0.0f, 0.0f, g_mouseherealpha, 0.0f,   // Alpha (70%)
							0.0f, 0.0f, 0.0f, 0.0f, 1.0f    // must be 1
						};
						attr.SetColorMatrix(&cmat);
						//Gdiplus::Image* imgptr = new Gdiplus::Image(L"E:\\PG\\AdditiveIK\Media\\MameMedia\\img_l105.png");
						if(g_mousehereimage){
							gdipg->DrawImage(g_mousehereimage, Gdiplus::Rect(mousepoint.x, mousepoint.y, BMP_W, BMP_H),
								0, 0, BMP_W, BMP_H,
								Gdiplus::UnitPixel, &attr, NULL, NULL);
						}
						delete gdipg;
					}
				}
			}
		}
		
	}

	void OWP_Separator::draw() {
		if (!hdcM) {
			return;
		}


		//static int s_paintcnt = 0;
		//s_paintcnt++;
		//if (g_previewFlag != 0) {
		//	if ((s_paintcnt % 2) != 0) {
		//		return;
		//	}
		//}

		//if (g_underloading == true) {
		//	return;
		//}

		//枠を書く
		if (hdcM) {
			int centerPos = getCenterLinePos();
			hdcM->setPenAndBrush(RGB(min(baseColor.r + 20, 255), min(baseColor.g + 20, 255), min(baseColor.b + 20, 255)), NULL);
			if (divideSide) {
				MoveToEx(hdcM->hDC, pos.x + centerPos, pos.y + 1, NULL);
				LineTo(hdcM->hDC, pos.x + centerPos, pos.y + size.y - 1);
				if (canShift) {
					hdcM->setPenAndBrush(RGB(240, 240, 240), NULL);
					int centerPos2 = pos.y + size.y / 2;
					MoveToEx(hdcM->hDC, pos.x + centerPos, centerPos2 - HANDLE_MARK_SIZE / 2, NULL);
					LineTo(hdcM->hDC, pos.x + centerPos, centerPos2 + HANDLE_MARK_SIZE / 2);
				}
			}
			else {
				MoveToEx(hdcM->hDC, pos.x + 1, pos.y + centerPos, NULL);
				LineTo(hdcM->hDC, pos.x + size.x - 1, pos.y + centerPos);
				if (canShift) {
					hdcM->setPenAndBrush(RGB(240, 240, 240), NULL);
					int centerPos2 = pos.x + size.x / 2;
					MoveToEx(hdcM->hDC, centerPos2 - HANDLE_MARK_SIZE / 2, pos.y + centerPos, NULL);
					LineTo(hdcM->hDC, centerPos2 + HANDLE_MARK_SIZE / 2, pos.y + centerPos);
				}
			}



			//全ての内部パーツを描画
			int hoststartx;
			int hostendx;
			int hoststarty;
			int hostendy;
			if (getParentScrollWnd()) {
				hoststartx = getParentScrollWnd()->getPartsAreaPos().x;
				hostendx = getParentScrollWnd()->getPartsAreaPos().x + getSize().x;
				hoststarty = getParentScrollWnd()->getPartsAreaPos().y;
				hostendy = getParentScrollWnd()->getPartsAreaPos().y + getSize().y;

			}
			else {
				hoststartx = getPos().x;
				hostendx = getPos().x + getSize().x;
				hoststarty = getPos().y;
				hostendy = getPos().y + getSize().y;
			}

			std::list<OrgWindowParts*>::iterator itr;
			for (itr = partsList1.begin(); itr != partsList1.end(); itr++) {
				if(*itr){
					WindowPos clientpos = (*itr)->getPos();
					WindowSize clientsize = (*itr)->getSize();

					if ((*itr)->getIsRadioButton() || (*itr)->getIsSeparator()) {//2023/10/15
						//OWP_RadioButtonとOWP_Separatorについては　WindowPartsとしては１つ　エリアが包含していれば良い
						//パーツの下がエリアの始まりよりも下　かつ　パーツの上がエリアの終わりよりも上
						if (((clientpos.y + clientsize.y) >= hoststarty) && (clientpos.y <= hostendy)) {
							(*itr)->draw();
						}
					}
					else if ((clientpos.y >= hoststarty) && (clientpos.y <= hostendy)) {//2023/09/19
						(*itr)->draw();
					}
					else {
						//スクロールで見切れた場合は描画しない
						if (clientpos.y > hostendy) {
							break;//!!!!! 後続のpartsのYは更に大きいはずなので　高速化のために処理を終了
						}
					}					
				}
			}
			std::list<OrgWindowParts*>::iterator itr2;
			for (itr2 = partsList2.begin(); itr2 != partsList2.end(); itr2++) {
				if (*itr2) {
					WindowPos clientpos = (*itr2)->getPos();
					WindowSize clientsize = (*itr2)->getSize();

					if ((*itr2)->getIsRadioButton() || (*itr2)->getIsSeparator()) {//2023/10/15
						//OWP_RadioButtonとOWP_Separatorについては　WindowPartsとしては１つ　エリアが包含していれば良い
						//パーツの下がエリアの始まりよりも下　かつ　パーツの上がエリアの終わりよりも上
						if (((clientpos.y + clientsize.y) >= hoststarty) && (clientpos.y <= hostendy)) {
							(*itr2)->draw();
						}
					}
					else if ((clientpos.y >= hoststarty) && (clientpos.y <= hostendy)) {//2023/09/19
						(*itr2)->draw();
					}
					else {
						//スクロールで見切れた場合は描画しない
						if (clientpos.y > hostendy) {
							break;//!!!!! 後続のpartsのYは更に大きいはずなので　高速化のために処理を終了
						}
					}
				}
			}

			{
				if (g_dsmousewait == 1) {
					POINT mousepoint;
					::GetCursorPos(&mousepoint);
					if (getParent() && getHDCMaster()) {
						::ScreenToClient(getParent()->getHWnd(), &mousepoint);
						int BMP_W = 52;
						int BMP_H = 50;
						Gdiplus::Graphics* gdipg = new Gdiplus::Graphics(hdcM->hDC);
						if (gdipg) {
							Gdiplus::ImageAttributes attr;
							Gdiplus::ColorMatrix cmat = {
								1.0f, 0.0f, 0.0f, 0.0f, 0.0f,   // Red
								0.0f, 1.0f, 0.0f, 0.0f, 0.0f,   // Green
								0.0f, 0.0f, 1.0f, 0.0f, 0.0f,   // Blue
								0.0f, 0.0f, 0.0f, g_mouseherealpha, 0.0f,   // Alpha (70%)
								0.0f, 0.0f, 0.0f, 0.0f, 1.0f    // must be 1
							};
							attr.SetColorMatrix(&cmat);
							//Gdiplus::Image* imgptr = new Gdiplus::Image(L"E:\\PG\\AdditiveIK\Media\\MameMedia\\img_l105.png");
							if (g_mousehereimage){
								gdipg->DrawImage(g_mousehereimage, Gdiplus::Rect(mousepoint.x, mousepoint.y, BMP_W, BMP_H),
									0, 0, BMP_W, BMP_H,
									Gdiplus::UnitPixel, &attr, NULL, NULL);
							}

							delete gdipg;
						}
					}
				}
			}

		}


	}

	void OWP_Timeline::LineData::callRewrite()
	{
	}

	void OWP_EulerGraph::callRewrite()
	{
		if ((g_endappflag == 0) && parentWindow && IsWindow(parentWindow->getHWnd())) {
			//const int AXIS_CURSOR_SIZE = 4;
			//const int AXIS_LABEL_SIDE_MARGIN = 7;

			////目盛り線 & ラベル
			//int x0 = pos.x + MARGIN + LABEL_SIZE_X;
			//int x1 = pos.x + size.x - MARGIN - SCROLL_BAR_WIDTH;
			//int y0 = pos.y + MARGIN;
			//int y1 = y0 + AXIS_SIZE_Y + 1;

			////for (int i = (int)showPos_time; i <= (int)maxTime; i++) {
			//int minx = 0;
			//int maxx = (int)(((double)showPos_width) * timeSize) + x0 + 1;

			//RECT tmpRect;
			//tmpRect.left = 0;
			////tmpRect.left = minx;
			//tmpRect.top = 0;
			//tmpRect.right = size.x;
			////tmpRect.right = maxx;
			//tmpRect.bottom = size.y + 40;
			//InvalidateRect(parentWindow->getHWnd(), &tmpRect, false);

			InvalidateRect(parentWindow->getHWnd(), NULL, false);


			//draw();

		}
	}


	void OWP_Timeline::callRewrite()
	{
		if ((g_endappflag == 0) && parentWindow && IsWindow(parentWindow->getHWnd())) {
			//const int AXIS_CURSOR_SIZE = 4;
			//const int AXIS_LABEL_SIDE_MARGIN = 7;

			////目盛り線 & ラベル
			//int x0 = pos.x + MARGIN + LABEL_SIZE_X;
			//int x1 = pos.x + size.x - MARGIN - SCROLL_BAR_WIDTH;
			//int y0 = pos.y + MARGIN;
			//int y1 = y0 + AXIS_SIZE_Y + 1;

			//for (int i = (int)showPos_time; i <= (int)maxTime; i++) {
			//int minx = 0;
			//int maxx = (int)(((double)showPos_width) * timeSize) + x0 + 1;

			//RECT tmpRect;
			//tmpRect.left = 0;
			////tmpRect.left = minx;
			//tmpRect.top = 0;
			//tmpRect.right = size.x + 20;
			////tmpRect.right = maxx;
			//tmpRect.bottom = size.y;
			//InvalidateRect(parentWindow->getHWnd(), &tmpRect, false);

			InvalidateRect(parentWindow->getHWnd(), NULL, false);


			//draw();

		}
	}
	void OWP_Timeline::draw() {
		if (!hdcM) {
			return;
		}

		//static int s_paintcnt = 0;
		//s_paintcnt++;
		//if (g_previewFlag != 0) {
		//	if ((s_paintcnt % 2) != 0) {
		//		return;
		//	}
		//}


		drawEdge();

		//時間軸目盛り
		{
			const int AXIS_CURSOR_SIZE = 4;
			const int AXIS_LABEL_SIDE_MARGIN = 7;

			//目盛り線 & ラベル
			int x0 = pos.x + MARGIN + LABEL_SIZE_X;
			int x1 = pos.x + size.x - MARGIN - SCROLL_BAR_WIDTH;
			int y0 = pos.y + MARGIN;
			int y1 = y0 + AXIS_SIZE_Y + 1;

			int startindex;
			int endindex;
			if ((g_previewFlag == 0) || (g_previewFlag == 5)) {
				startindex = (int)showPos_time;
				endindex = (int)maxTime;

				//int endtime = min((int)(showPos_time + showPos_width), (int)maxTime);
				//int endtime = min((int)(showPos_time + 1), (int)maxTime);

				//for (int i = (int)showPos_time; i <= (int)maxTime; i++) {
				//for (int i = (int)showPos_time; i <= endtime; i++) {
				for (int i = startindex; i <= endindex; i++) {
					int xx = (int)(((double)i - showPos_time) * timeSize) + x0 + 1;

					if ((x1 + AXIS_LABEL_SIDE_MARGIN) <= xx) break;
					if ((x0 - AXIS_LABEL_SIDE_MARGIN) <= xx) {
						hdcM->setPenAndBrush(RGB(min(baseColor.r + 20, 255), min(baseColor.g + 20, 255), min(baseColor.b + 20, 255)), NULL);
						MoveToEx(hdcM->hDC, xx, y1 - 5, NULL);
						LineTo(hdcM->hDC, xx, y1);

						if (((i < 1000) && (i % 5 == 0)) || ((i >= 1000) && (i % 10 == 0))) {
							TCHAR tmpChar[20];
							_stprintf_s(tmpChar, 20, _T("%.3G"), (double)i);
							hdcM->setFont(12, _T("ＭＳ ゴシック"));
							SetTextColor(hdcM->hDC, RGB(240, 240, 240));
							TextOut(hdcM->hDC,
								xx - (int)((double)_tcslen(tmpChar) * 2.0), y0,
								tmpChar, (int)_tcslen(tmpChar));
						}
					}
				}
			}
			else {
				//モーション再生中は　目盛り部分に　カレントタイムの数値だけを表示

				startindex = max(0, (IntTime(currentTime) - KEYNUM_ONPREVIEW));
				endindex = min(IntTime(maxTime), (IntTime(currentTime) + KEYNUM_ONPREVIEW));

				int xx = (int)((currentTime - showPos_time) * timeSize) + x0 + 1;

				if ((x1 + AXIS_LABEL_SIDE_MARGIN) > xx) {
					if ((x0 - AXIS_LABEL_SIDE_MARGIN) <= xx) {
						hdcM->setPenAndBrush(RGB(min(baseColor.r + 20, 255), min(baseColor.g + 20, 255), min(baseColor.b + 20, 255)), NULL);
						//MoveToEx(hdcM->hDC, xx, y1 - 5, NULL);
						//LineTo(hdcM->hDC, xx, y1);

						TCHAR tmpChar[20];
						_stprintf_s(tmpChar, 20, _T("%d"), (int)currentTime);
						hdcM->setFont(12, _T("ＭＳ ゴシック"));
						SetTextColor(hdcM->hDC, RGB(240, 240, 240));
						TextOut(hdcM->hDC,
							xx - (int)((double)_tcslen(tmpChar) * 12.0) - 12, y0 + 18,
							tmpChar, (int)_tcslen(tmpChar));

					}
				}
			}

			//カーソル
			int xx = (int)((currentTime - showPos_time)*timeSize) + x0 + 1;
			hdcM->setPenAndBrush(RGB(240, 240, 240), NULL);
			if (((x0 - AXIS_CURSOR_SIZE) <= xx) && 
				(xx <= (x1 + AXIS_CURSOR_SIZE))) {
				for (int i = 0; i<AXIS_CURSOR_SIZE; i++) {
					MoveToEx(hdcM->hDC, xx - i, y1 - i - 2, NULL);
					LineTo(hdcM->hDC, xx + i + 1, y1 - i - 2);
				}
			}

			//枠
			hdcM->setPenAndBrush(NULL, RGB(baseColor.r, baseColor.g, baseColor.b));
			Rectangle(hdcM->hDC, pos.x, y0, x0, y1);
			Rectangle(hdcM->hDC, x1, y0, pos.x + size.x, y1);
			hdcM->setPenAndBrush(RGB(min(baseColor.r + 20, 255), min(baseColor.g + 20, 255), min(baseColor.b + 20, 255)), NULL);
			Rectangle(hdcM->hDC, x0, y0, x1, y1);
		}

		drawEdge(false);


		//行データ
		int showLineNum = (size.y - SCROLL_BAR_WIDTH - AXIS_SIZE_Y - MARGIN * 2) / (LABEL_SIZE_Y - 1);

		int linenum;
		if ((g_previewFlag == 0) || g_preciseOnPreviewToo) {
			linenum = showLineNum;
		}
		else {
			linenum = min(1, showLineNum);
		}

		//if (getDispKeyFlag() == true) {
		//for (int i = showPos_line, j = 0; i < (int)lineData.size() && j < showLineNum; i++, j++) {
		for (int i = showPos_line, j = 0; i < (int)lineData.size() && j < linenum; i++, j++) {
			bool highLight = false;
			if (i == currentLine) highLight = true;
			if (i >= 0) {
				lineData[i]->draw(hdcM,
					pos.x + MARGIN,
					pos.y + MARGIN + AXIS_SIZE_Y + j * (LABEL_SIZE_Y - 1),
					size.x - SCROLL_BAR_WIDTH - MARGIN * 2,
					timeSize, showPos_time, highLight);
			}
		}
		//ドラッグによる選択範囲
		if (dragSelect && (dragSelectTime1 != dragSelectTime2)) {
			int xx0 = pos.x + MARGIN + LABEL_SIZE_X + 1;
			int yy0 = pos.y + MARGIN + AXIS_SIZE_Y;
			int xx1 = pos.x + size.x - MARGIN - SCROLL_BAR_WIDTH - 1;
			int yy1 = pos.y + size.y - MARGIN - SCROLL_BAR_WIDTH;
			int x0 = xx0 + (int)((min(dragSelectTime1, dragSelectTime2) - showPos_time) * timeSize);
			int x1 = xx0 + (int)((max(dragSelectTime1, dragSelectTime2) - showPos_time) * timeSize);
			int y0 = yy0 + (min(dragSelectLine1, dragSelectLine2) - showPos_line) * (LABEL_SIZE_Y - 1) + 1;
			int y1 = yy0 + (max(dragSelectLine1, dragSelectLine2) - showPos_line + 1) * (LABEL_SIZE_Y - 1) - 1;

			{//枠描画
				hdcM->setPenAndBrush(RGB(min(baseColor.r + 20, 255), min(baseColor.g + 20, 255), min(baseColor.b + 20, 255)), NULL);
				if (xx0 <= x0) {		//左枠
					MoveToEx(hdcM->hDC, x0, max(yy0, y0 + 1), NULL);
					LineTo(hdcM->hDC, x0, min(y1, yy1));
				}
				if (x1 <= xx1) {		//右枠
					MoveToEx(hdcM->hDC, x1, max(yy0, y0 + 1), NULL);
					LineTo(hdcM->hDC, x1, min(y1, yy1));
				}
				if (yy0 <= y0) {		//上枠
					MoveToEx(hdcM->hDC, max(xx0, x0 + 1), y0, NULL);
					LineTo(hdcM->hDC, min(x1, xx1), y0);
				}
				if (y1 <= yy1) {		//下枠
					MoveToEx(hdcM->hDC, max(xx0, x0 + 1), y1, NULL);
					LineTo(hdcM->hDC, min(x1, xx1), y1);
				}
			}

			//}



			////時間軸スクロールバー
			//{
			//	int x0 = pos.x + MARGIN + LABEL_SIZE_X;
			//	int x1 = pos.x + size.x - MARGIN - SCROLL_BAR_WIDTH;
			//	int y0 = pos.y + size.y - MARGIN - SCROLL_BAR_WIDTH;
			//	int y1 = y0 + SCROLL_BAR_WIDTH;

			//	//枠
			//	//hdcM->setPenAndBrush(RGB(min(baseColor.r + 20, 255), min(baseColor.g + 20, 255), min(baseColor.b + 20, 255)), NULL);
			//	hdcM->setPenAndBrush(RGB(255, 255, 255), NULL);
			//	Rectangle(hdcM->hDC, x0, y0, x1, y1);

			//	//中身
			//	double showTimeLength = ((double)x1 - x0 - 3) / timeSize;
			//	double barSize = ((double)x1 - x0 - 4)*showTimeLength / maxTime;
			//	double barStart = ((double)x1 - x0 - 4)*showPos_time / maxTime;
			//	if (showTimeLength<maxTime) {
			//		//hdcM->setPenAndBrush(NULL, RGB(min(baseColor.r + 20, 255), min(baseColor.g + 20, 255), min(baseColor.b + 20, 255)));
			//		hdcM->setPenAndBrush(NULL, RGB(240, 240, 240));
			//		Rectangle(hdcM->hDC, x0 + 2 + (int)barStart, y0 + 2, x0 + 2 + (int)(barStart + barSize), y1 - 2);
			//	}
			//}
		}

		//ラベルスクロールバー
		if(g_previewFlag == 0)
		{
			int x0 = pos.x + size.x - MARGIN - SCROLL_BAR_WIDTH - 1;
			int x1 = x0 + SCROLL_BAR_WIDTH + 1;
			int y0 = pos.y + MARGIN + AXIS_SIZE_Y;
			int y1 = pos.y + size.y - MARGIN - SCROLL_BAR_WIDTH + 1;

			//枠
			//hdcM->setPenAndBrush(RGB(min(baseColor.r + 20, 255), min(baseColor.g + 20, 255), min(baseColor.b + 20, 255)), NULL);
			hdcM->setPenAndBrush(RGB(240, 240, 240), NULL);
			Rectangle(hdcM->hDC, x0, y0, x1, y1);

			//中身
			if (lineData.size() > 0) {
				int barSize = (y1 - y0 - 4) * showLineNum / (int)lineData.size();
				int barStart = (y1 - y0 - 4) * showPos_line / (int)lineData.size();
				if (showLineNum < (int)lineData.size()) {
					//hdcM->setPenAndBrush(NULL, RGB(min(baseColor.r + 20, 255), min(baseColor.g + 20, 255), min(baseColor.b + 20, 255)));
					hdcM->setPenAndBrush(NULL, RGB(255, 255, 255));
					Rectangle(hdcM->hDC, x0 + 2, y0 + 2 + barStart, x1 - 2, y0 + 2 + barStart + barSize + 1);
				}
			}
		}
		

		{
			if (g_dsmousewait == 1) {
				POINT mousepoint;
				::GetCursorPos(&mousepoint);
				if (getParent()) {
					::ScreenToClient(getParent()->getHWnd(), &mousepoint);
					int BMP_W = 52;
					int BMP_H = 50;
					Gdiplus::Graphics* gdipg = new Gdiplus::Graphics(hdcM->hDC);
					if (gdipg) {
						Gdiplus::ImageAttributes attr;
						Gdiplus::ColorMatrix cmat = {
							1.0f, 0.0f, 0.0f, 0.0f, 0.0f,   // Red
							0.0f, 1.0f, 0.0f, 0.0f, 0.0f,   // Green
							0.0f, 0.0f, 1.0f, 0.0f, 0.0f,   // Blue
							0.0f, 0.0f, 0.0f, g_mouseherealpha, 0.0f,   // Alpha (70%)
							0.0f, 0.0f, 0.0f, 0.0f, 1.0f    // must be 1
						};
						attr.SetColorMatrix(&cmat);
						//Gdiplus::Image* imgptr = new Gdiplus::Image(L"E:\\PG\\AdditiveIK\Media\\MameMedia\\img_l105.png");
						if (g_mousehereimage) {
							gdipg->DrawImage(g_mousehereimage, Gdiplus::Rect(mousepoint.x, mousepoint.y, BMP_W, BMP_H),
								0, 0, BMP_W, BMP_H,
								Gdiplus::UnitPixel, &attr, NULL, NULL);
						}
						delete gdipg;
					}
				}
			}
		}
	}

	void OWP_EulerGraph::draw() {
		if (!hdcM) {
			return;
		}

		//static int s_paintcnt = 0;
		//s_paintcnt++;
		//if (g_previewFlag != 0) {
		//	if ((s_paintcnt % 2) != 0) {
		//		return;
		//	}
		//}

		//drawEdge();

		////時間軸目盛り
		//{
		//	const int AXIS_CURSOR_SIZE = 4;
		//	const int AXIS_LABEL_SIDE_MARGIN = 7;

		//	//目盛り線 & ラベル
		//	int x0 = pos.x + MARGIN + LABEL_SIZE_X;
		//	int x1 = pos.x + size.x - MARGIN - SCROLL_BAR_WIDTH;
		//	int y0 = pos.y + MARGIN;
		//	int y1 = y0 + AXIS_SIZE_Y + 1;
		//	for (int i = (int)showPos_time; i <= (int)maxTime; i++) {
		//		int xx = (int)(((double)i - showPos_time)*timeSize) + x0 + 1;

		//		if (x1 + AXIS_LABEL_SIDE_MARGIN <= xx) break;
		//		if (x0 - AXIS_LABEL_SIDE_MARGIN <= xx) {
		//			hdcM->setPenAndBrush(RGB(min(baseColor.r + 20, 255), min(baseColor.g + 20, 255), min(baseColor.b + 20, 255)), NULL);
		//			MoveToEx(hdcM->hDC, xx, y1 - 5, NULL);
		//			LineTo(hdcM->hDC, xx, y1);

		//			if (((i < 1000) && (i % 5 == 0)) || ((i >= 1000) && (i % 10 == 0))) {
		//				TCHAR tmpChar[20];
		//				_stprintf_s(tmpChar, 20, _T("%.3G"), (double)i);
		//				hdcM->setFont(12, _T("ＭＳ ゴシック"));
		//				SetTextColor(hdcM->hDC, RGB(240, 240, 240));
		//				TextOut(hdcM->hDC,
		//					xx - (int)((double)_tcslen(tmpChar)*2.0), y0,
		//					tmpChar, (int)_tcslen(tmpChar));
		//			}
		//		}
		//	}

		//	//カーソル
		//	int xx = (int)((currentTime - showPos_time)*timeSize) + x0 + 1;
		//	hdcM->setPenAndBrush(RGB(240, 240, 240), NULL);
		//	if (x0 - AXIS_CURSOR_SIZE <= xx && xx <= x1 + AXIS_CURSOR_SIZE) {
		//		for (int i = 0; i<AXIS_CURSOR_SIZE; i++) {
		//			MoveToEx(hdcM->hDC, xx - i, y1 - i - 2, NULL);
		//			LineTo(hdcM->hDC, xx + i + 1, y1 - i - 2);
		//		}
		//	}

		//	//枠
		//	hdcM->setPenAndBrush(NULL, RGB(baseColor.r, baseColor.g, baseColor.b));
		//	Rectangle(hdcM->hDC, pos.x, y0, x0, y1);
		//	Rectangle(hdcM->hDC, x1, y0, pos.x + size.x, y1);
		//	hdcM->setPenAndBrush(RGB(min(baseColor.r + 20, 255), min(baseColor.g + 20, 255), min(baseColor.b + 20, 255)), NULL);
		//	Rectangle(hdcM->hDC, x0, y0, x1, y1);
		//}

		//drawEdge(false);

		//行データ
		//int showLineNum = (size.y - SCROLL_BAR_WIDTH - AXIS_SIZE_Y - MARGIN * 2) / (LABEL_SIZE_Y - 1);
		//int showLineNum = 3;
		int showLineNum = 4;
		//for (int i = showPos_line, j = 0; i<(int)lineData.size() && j<showLineNum; i++, j++) {
		

		showPos_time = calcShowPosTime(currentTime);//2022/10/22


		CModel* currentmodel = GetCurrentModel();
		if (currentmodel) {
			bool noboneflag = currentmodel->GetNoBoneFlag();
			bool inviewflag = currentmodel->GetInView(0);

			if ((noboneflag == false) && (inviewflag == true)) {
				if (lineData.size() >= 4) {//X, Y, Z, Brush

					int drawnum = min((int)lineData.size(), showLineNum);

					for (int i = 0; i < drawnum; i++) {
						bool highLight = false;
						////if (i == currentLine) highLight = true;
						lineData[i]->draw(hdcM,
							pos.x + MARGIN,
							//pos.y + MARGIN + AXIS_SIZE_Y + j*(LABEL_SIZE_Y - 1),
							//pos.y + MARGIN + AXIS_SIZE_Y,
							pos.y,
							size.x - SCROLL_BAR_WIDTH - MARGIN * 2,
							timeSize, showPos_time, highLight);
					}
				}
			}
			else {
				int dbgflag1 = 1;
			}


			//2023/10/13 eultip
			if (g_previewFlag == 0) {
				int fontsize, startx, starty, stepy;
				if (g_4kresolution) {
					fontsize = 24;
					startx = 100;
					starty = 160;
					stepy = 40;
				}
				else {
					fontsize = 12;
					startx = 50;
					starty = 100;
					stepy = 20;
				}

				TCHAR strtipFrame[256];
				_stprintf_s(strtipFrame, 256, _T("Frame : %.1f"), getCurrentTime());
				hdcM->setFont(fontsize, _T("ＭＳ ゴシック"));
				SetTextColor(hdcM->hDC, RGB(240, 240, 240));
				TextOut(hdcM->hDC,
					pos.x + LABEL_SIZE_X + startx, starty,
					strtipFrame, (int)_tcslen(strtipFrame));

				TCHAR strtipX[256];
				_stprintf_s(strtipX, 256, _T("X : %.2f"), eultip.x);
				//hdcM->setFont(fontsize, _T("ＭＳ ゴシック"));
				//SetTextColor(hdcM->hDC, RGB(240, 240, 240));
				TextOut(hdcM->hDC,
					pos.x + LABEL_SIZE_X + startx, starty + stepy,
					strtipX, (int)_tcslen(strtipX));

				TCHAR strtipY[256];
				_stprintf_s(strtipY, 256, _T("Y : %.2f"), eultip.y);
				//hdcM->setFont(fontsize, _T("ＭＳ ゴシック"));
				//SetTextColor(hdcM->hDC, RGB(240, 240, 240));
				TextOut(hdcM->hDC,
					pos.x + LABEL_SIZE_X + startx, starty + 2 * stepy,
					strtipY, (int)_tcslen(strtipY));

				TCHAR strtipZ[256];
				_stprintf_s(strtipZ, 256, _T("Z : %.2f"), eultip.z);
				//hdcM->setFont(fontsize, _T("ＭＳ ゴシック"));
				//SetTextColor(hdcM->hDC, RGB(240, 240, 240));
				TextOut(hdcM->hDC,
					pos.x + LABEL_SIZE_X + startx, starty + 3 * stepy,
					strtipZ, (int)_tcslen(strtipZ));


				//2024/03/26
				TCHAR strtipName[MAX_PATH];
				_stprintf_s(strtipName, MAX_PATH, _T("Brush : %s"), g_brushname);
				//hdcM->setFont(fontsize, _T("ＭＳ ゴシック"));
				//SetTextColor(hdcM->hDC, RGB(240, 240, 240));
				TextOut(hdcM->hDC,
					pos.x + LABEL_SIZE_X + startx, starty + 4 * stepy,
					strtipName, (int)_tcslen(strtipName));

			}

		}
		else {
			//2023/08/26 カレントモデルにモーションが無い場合と　視野外の場合には　オイラーグラフ更新はお休み
			int dbgflag2 = 1;
		}


		////motionbrush ０フレームから書く
		//bool highLight = false;
		//lineData[3]->draw(hdcM,
		//pos.x + MARGIN,
		////pos.y + MARGIN + AXIS_SIZE_Y + j*(LABEL_SIZE_Y - 1),
		//pos.y + MARGIN + AXIS_SIZE_Y,
		//size.x - SCROLL_BAR_WIDTH - MARGIN * 2,
		//timeSize, 0.0, highLight);


		////ドラッグによる選択範囲
		//if (dragSelect && dragSelectTime1 != dragSelectTime2) {
		//	int xx0 = pos.x + MARGIN + LABEL_SIZE_X + 1;
		//	int yy0 = pos.y + MARGIN + AXIS_SIZE_Y;
		//	int xx1 = pos.x + size.x - MARGIN - SCROLL_BAR_WIDTH - 1;
		//	int yy1 = pos.y + size.y - MARGIN - SCROLL_BAR_WIDTH;
		//	int x0 = xx0 + (int)((min(dragSelectTime1, dragSelectTime2) - showPos_time)* timeSize);
		//	int x1 = xx0 + (int)((max(dragSelectTime1, dragSelectTime2) - showPos_time)* timeSize);
		//	int y0 = yy0 + (min(dragSelectLine1, dragSelectLine2) - showPos_line)* (LABEL_SIZE_Y - 1) + 1;
		//	int y1 = yy0 + (max(dragSelectLine1, dragSelectLine2) - showPos_line + 1)* (LABEL_SIZE_Y - 1) - 1;

		//	{//枠描画
		//		hdcM->setPenAndBrush(RGB(min(baseColor.r + 20, 255), min(baseColor.g + 20, 255), min(baseColor.b + 20, 255)), NULL);
		//		if (xx0 <= x0) {		//左枠
		//			MoveToEx(hdcM->hDC, x0, max(yy0, y0 + 1), NULL);
		//			LineTo(hdcM->hDC, x0, min(y1, yy1));
		//		}
		//		if (x1 <= xx1) {		//右枠
		//			MoveToEx(hdcM->hDC, x1, max(yy0, y0 + 1), NULL);
		//			LineTo(hdcM->hDC, x1, min(y1, yy1));
		//		}
		//		if (yy0 <= y0) {		//上枠
		//			MoveToEx(hdcM->hDC, max(xx0, x0 + 1), y0, NULL);
		//			LineTo(hdcM->hDC, min(x1, xx1), y0);
		//		}
		//		if (y1 <= yy1) {		//下枠
		//			MoveToEx(hdcM->hDC, max(xx0, x0 + 1), y1, NULL);
		//			LineTo(hdcM->hDC, min(x1, xx1), y1);
		//		}
		//	}
		//}

		////ラベルスクロールバー
		//{
		//	int x0 = pos.x + MARGIN + LABEL_SIZE_X;
		//	int x1 = pos.x + size.x - MARGIN - SCROLL_BAR_WIDTH;
		//	int y0 = pos.y + size.y - MARGIN - SCROLL_BAR_WIDTH;
		//	int y1 = y0 + SCROLL_BAR_WIDTH;

		//	//枠
		//	//hdcM->setPenAndBrush(RGB(min(baseColor.r + 20, 255), min(baseColor.g + 20, 255), min(baseColor.b + 20, 255)), NULL);
		//	hdcM->setPenAndBrush(RGB(255, 255, 255), NULL);
		//	Rectangle(hdcM->hDC, x0, y0, x1, y1);

		//	//中身
		//	double showTimeLength = ((double)(x1 - x0 - 3)) / timeSize;
		//	double barSize = ((double)(x1 - x0 - 4))*showTimeLength / maxTime;
		//	double barStart = ((double)(x1 - x0 - 4))*showPos_time / maxTime;
		//	if (showTimeLength<maxTime) {
		//		//hdcM->setPenAndBrush(NULL, RGB(min(baseColor.r + 20, 255), min(baseColor.g + 20, 255), min(baseColor.b + 20, 255)));
		//		hdcM->setPenAndBrush(NULL, RGB(255, 255, 255));
		//		Rectangle(hdcM->hDC, x0 + 2 + (int)barStart, y0 + 2, x0 + 2 + (int)(barStart + barSize), y1 - 2);
		//	}
		//}

		////時間軸スクロールバー
		//{
		//	int x0 = pos.x + size.x - MARGIN - SCROLL_BAR_WIDTH - 1;
		//	int x1 = x0 + SCROLL_BAR_WIDTH + 1;
		//	int y0 = pos.y + MARGIN + AXIS_SIZE_Y;
		//	int y1 = pos.y + size.y - MARGIN - SCROLL_BAR_WIDTH + 1;

		//	//枠
		//	hdcM->setPenAndBrush(RGB(min(baseColor.r + 20, 255), min(baseColor.g + 20, 255), min(baseColor.b + 20, 255)), NULL);
		//	Rectangle(hdcM->hDC, x0, y0, x1, y1);

		//	//中身
		//	int barSize = (y1 - y0 - 4)*showLineNum / (int)lineData.size();
		//	int barStart = (y1 - y0 - 4)*showPos_line / (int)lineData.size();
		//	if (showLineNum<(int)lineData.size()) {
		//		hdcM->setPenAndBrush(NULL, RGB(min(baseColor.r + 20, 255), min(baseColor.g + 20, 255), min(baseColor.b + 20, 255)));
		//		Rectangle(hdcM->hDC, x0 + 2, y0 + 2 + barStart, x1 - 2, y0 + 2 + barStart + barSize + 1);
		//	}
		//}

		{
			if (g_dsmousewait == 1) {
				POINT mousepoint;
				::GetCursorPos(&mousepoint);
				if (getParent() && getHDCMaster()) {
					::ScreenToClient(getParent()->getHWnd(), &mousepoint);
					int BMP_W = 52;
					int BMP_H = 50;
					Gdiplus::Graphics* gdipg = new Gdiplus::Graphics(hdcM->hDC);
					if (gdipg) {
						Gdiplus::ImageAttributes attr;
						Gdiplus::ColorMatrix cmat = {
							1.0f, 0.0f, 0.0f, 0.0f, 0.0f,   // Red
							0.0f, 1.0f, 0.0f, 0.0f, 0.0f,   // Green
							0.0f, 0.0f, 1.0f, 0.0f, 0.0f,   // Blue
							0.0f, 0.0f, 0.0f, g_mouseherealpha, 0.0f,   // Alpha (70%)
							0.0f, 0.0f, 0.0f, 0.0f, 1.0f    // must be 1
						};
						attr.SetColorMatrix(&cmat);
						//Gdiplus::Image* imgptr = new Gdiplus::Image(L"E:\\PG\\AdditiveIK\Media\\MameMedia\\img_l105.png");
						if (g_mousehereimage) {
							gdipg->DrawImage(g_mousehereimage, Gdiplus::Rect(mousepoint.x, mousepoint.y, BMP_W, BMP_H),
								0, 0, BMP_W, BMP_H,
								Gdiplus::UnitPixel, &attr, NULL, NULL);
						}
						delete gdipg;
					}
					
				}
			}
		}

	}

	//namespace global func
	void* GetNewEulKey()
	{
		static int s_befheadno = -1;
		static int s_befelemno = -1;

		int curpoollen;
		curpoollen = (int)g_eulpool.size();


		//if ((s_befheadno != (g_eulpool.size() - 1)) || (s_befelemno != (EULPOOLBLKLEN - 1))) {//前回リリースしたポインタが最後尾ではない場合

		//前回リリースしたポインタの次のメンバーをチェックして未使用だったらリリース
			int chkheadno;
			chkheadno = s_befheadno;
			int chkelemno;
			chkelemno = s_befelemno + 1;
			//if ((chkheadno >= 0) && (chkheadno >= curpoollen) && (chkelemno >= EULPOOLBLKLEN)) {
			if ((chkheadno >= 0) && (chkheadno < (curpoollen - 1)) && (chkelemno >= EULPOOLBLKLEN)) {//2021/08/21
				chkelemno = 0;
				chkheadno++;
			}
			if ((chkheadno >= 0) && (chkheadno < curpoollen) && (chkelemno >= 0) && (chkelemno < EULPOOLBLKLEN)) {
				OWP_EulerGraph::EulLineData::EulKey* cureulhead = (OWP_EulerGraph::EulLineData::EulKey*)g_eulpool[chkheadno];

				if (cureulhead) {
					OWP_EulerGraph::EulLineData::EulKey* chkeul;
					chkeul = cureulhead + chkelemno;
					if (chkeul) {
						if (chkeul->GetUseFlag() == 0) {
							int saveindex = chkeul->GetIndexOfPool();
							int saveallochead = chkeul->IsAllocHead();
							chkeul->InitParams();
							chkeul->SetUseFlag(1);
							chkeul->SetIndexOfPool(saveindex);
							chkeul->SetIsAllocHead(saveallochead);

							s_befheadno = chkheadno;
							s_befelemno = chkelemno;
							return (void*)chkeul;
						}
					}
				}
			}

			//if ((chkheadno >= 0) && (chkheadno < curpoollen)) {
				//プールを先頭から検索して未使用がみつかればそれをリリース
			int eulno;
			for (eulno = 0; eulno < curpoollen; eulno++) {
				OWP_EulerGraph::EulLineData::EulKey* cureulhead = (OWP_EulerGraph::EulLineData::EulKey*)g_eulpool[eulno];
				if (cureulhead) {
					int elemno;
					for (elemno = 0; elemno < EULPOOLBLKLEN; elemno++) {
						OrgWinGUI::OWP_EulerGraph::EulLineData::EulKey* cureul;
						cureul = cureulhead + elemno;
						if (cureul->GetUseFlag() == 0) {
							int saveindex = cureul->GetIndexOfPool();
							int saveallochead = cureul->IsAllocHead();
							cureul->InitParams();
							cureul->SetUseFlag(1);
							cureul->SetIndexOfPool(saveindex);
							cureul->SetIsAllocHead(saveallochead);

							s_befheadno = eulno;
							s_befelemno = elemno;
							return (void*)cureul;
						}
					}
				}
			}
			//}
		//}

		//未使用eulがpoolに無かった場合、アロケートしてアロケートした先頭のポインタをリリース
		OWP_EulerGraph::EulLineData::EulKey* alloceul;
		alloceul = new OWP_EulerGraph::EulLineData::EulKey[EULPOOLBLKLEN];
		if (!alloceul) {
			_ASSERT(0);

			s_befheadno = -1;
			s_befelemno = -1;

			return 0;
		}
		int allocno;
		for (allocno = 0; allocno < EULPOOLBLKLEN; allocno++) {
			OWP_EulerGraph::EulLineData::EulKey* curalloceul = alloceul + allocno;
			if (curalloceul) {
				//int indexofpool = curpoollen + allocno;
				int indexofpool = curpoollen;//pool[indexofpool] 2021/08/19
				curalloceul->InitParams();
				curalloceul->SetUseFlag(0);
				curalloceul->SetIndexOfPool(indexofpool);

				if (allocno == 0) {
					curalloceul->SetIsAllocHead(1);
				}
				else {
					curalloceul->SetIsAllocHead(0);
				}
			}
			else {
				_ASSERT(0);

				s_befheadno = -1;
				s_befelemno = -1;

				return 0;
			}
		}
		g_eulpool.push_back((void*)alloceul);//allocate block(アロケート時の先頭ポインタ)を格納

		alloceul->SetUseFlag(1);


		s_befheadno = (int)g_eulpool.size() - 1;
		if (s_befheadno < 0) {
			s_befheadno = 0;
		}
		s_befelemno = 0;

		return (void*)alloceul;
	}

	//member func
	void OWP_EulerGraph::EulLineData::EulKey::InvalidateEulKeys()
	{
		if (!this) {
			_ASSERT(0);
			return;
		}

		int saveindex = GetIndexOfPool();
		int saveallochead = IsAllocHead();

		InitParams();
		SetUseFlag(0);
		SetIsAllocHead(saveallochead);
		SetIndexOfPool(saveindex);
	}

	//namespace global func
	void InitEulKeys()
	{
		g_eulpool.clear();
	}

	//namespace global func
	void DestroyEulKeys() {
		int eulallocnum = (int)g_eulpool.size();
		int eulno;
		for (eulno = 0; eulno < eulallocnum; eulno++) {
			//class OWP_EulerGraph::EulLineData::EulKey;
			OWP_EulerGraph::EulLineData::EulKey* deleul;
			deleul = (OWP_EulerGraph::EulLineData::EulKey*)g_eulpool[eulno];
			//if (deleul && (deleul->IsAllocHead() == 1)) {
			if (deleul) {
				delete[] deleul;
			}
		}
		g_eulpool.clear();
	}


/// LineData::Key ここから


	//namespace global func
	void* GetNewKey()
	{
		static int s_befheadno = -1;
		static int s_befelemno = -1;

		int curpoollen;
		curpoollen = (int)g_keypool.size();


		//if ((s_befheadno != (g_keypool.size() - 1)) || (s_befelemno != (KEYPOOLBLKLEN - 1))) {//前回リリースしたポインタが最後尾ではない場合

		//前回リリースしたポインタの次のメンバーをチェックして未使用だったらリリース
		int chkheadno;
		chkheadno = s_befheadno;
		int chkelemno;
		chkelemno = s_befelemno + 1;
		//if ((chkheadno >= 0) && (chkheadno >= curpoollen) && (chkelemno >= KEYPOOLBLKLEN)) {
		if ((chkheadno >= 0) && (chkheadno < (curpoollen - 1)) && (chkelemno >= KEYPOOLBLKLEN)) {//2021/08/21
			chkelemno = 0;
			chkheadno++;
		}
		if ((chkheadno >= 0) && (chkheadno < curpoollen) && (chkelemno >= 0) && (chkelemno < KEYPOOLBLKLEN)) {
			OWP_Timeline::LineData::Key* curkeyhead = (OWP_Timeline::LineData::Key*)g_keypool[chkheadno];

			if (curkeyhead) {
				OWP_Timeline::LineData::Key* chkkey;
				chkkey = curkeyhead + chkelemno;
				if (chkkey) {
					if (chkkey->GetUseFlag() == 0) {
						int saveindex = chkkey->GetIndexOfPool();
						int saveallochead = chkkey->IsAllocHead();
						chkkey->InitParams();
						chkkey->SetUseFlag(1);
						chkkey->SetIndexOfPool(saveindex);
						chkkey->SetIsAllocHead(saveallochead);

						s_befheadno = chkheadno;
						s_befelemno = chkelemno;
						return (void*)chkkey;
					}
				}
			}
		}

		//if ((chkheadno >= 0) && (chkheadno < curpoollen)) {
			//プールを先頭から検索して未使用がみつかればそれをリリース
		int keyno;
		for (keyno = 0; keyno < curpoollen; keyno++) {
			OWP_Timeline::LineData::Key* curkeyhead = (OWP_Timeline::LineData::Key*)g_keypool[keyno];
			if (curkeyhead) {
				int elemno;
				for (elemno = 0; elemno < KEYPOOLBLKLEN; elemno++) {
					OrgWinGUI::OWP_Timeline::LineData::Key* curkey;
					curkey = curkeyhead + elemno;
					if (curkey->GetUseFlag() == 0) {
						int saveindex = curkey->GetIndexOfPool();
						int saveallochead = curkey->IsAllocHead();
						curkey->InitParams();
						curkey->SetUseFlag(1);
						curkey->SetIndexOfPool(saveindex);
						curkey->SetIsAllocHead(saveallochead);

						s_befheadno = keyno;
						s_befelemno = elemno;
						return (void*)curkey;
					}
				}
			}
		}
		//}
	//}

	//未使用keyがpoolに無かった場合、アロケートしてアロケートした先頭のポインタをリリース
		OWP_Timeline::LineData::Key* allockey;
		allockey = new OWP_Timeline::LineData::Key[KEYPOOLBLKLEN];
		if (!allockey) {
			_ASSERT(0);

			s_befheadno = -1;
			s_befelemno = -1;

			return 0;
		}
		int allocno;
		for (allocno = 0; allocno < KEYPOOLBLKLEN; allocno++) {
			OWP_Timeline::LineData::Key* curallockey = allockey + allocno;
			if (curallockey) {
				//int indexofpool = curpoollen + allocno;
				int indexofpool = curpoollen;//pool[indexofpool] 2021/08/19
				curallockey->InitParams();
				curallockey->SetUseFlag(0);
				curallockey->SetIndexOfPool(indexofpool);

				if (allocno == 0) {
					curallockey->SetIsAllocHead(1);
				}
				else {
					curallockey->SetIsAllocHead(0);
				}
			}
			else {
				_ASSERT(0);

				s_befheadno = -1;
				s_befelemno = -1;

				return 0;
			}
		}
		g_keypool.push_back((void*)allockey);//allocate block(アロケート時の先頭ポインタ)を格納

		allockey->SetUseFlag(1);


		s_befheadno = (int)g_keypool.size() - 1;
		if (s_befheadno < 0) {
			s_befheadno = 0;
		}
		s_befelemno = 0;

		return (void*)allockey;
	}

	//member func
	void OWP_Timeline::LineData::Key::InvalidateKeys()
	{
		if (!this) {
			_ASSERT(0);
			return;
		}

		int saveindex = GetIndexOfPool();
		int saveallochead = IsAllocHead();

		InitParams();
		SetUseFlag(0);
		SetIsAllocHead(saveallochead);
		SetIndexOfPool(saveindex);
	}

	//namespace global func
	void InitKeys()
	{
		g_keypool.clear();
	}

	//namespace global func
	void DestroyKeys() {
		int keyallocnum = (int)g_keypool.size();
		int keyno;
		for (keyno = 0; keyno < keyallocnum; keyno++) {
			//class OWP_Timeline::LineData::Key;
			OWP_Timeline::LineData::Key* delkey;
			delkey = (OWP_Timeline::LineData::Key*)g_keypool[keyno];
			//if (delkey && (delkey->IsAllocHead() == 1)) {
			if (delkey) {
				delete[] delkey;
			}
		}
		g_keypool.clear();
	}


/// LineData::Key ここまで


	void OWP_CheckBoxA::draw() {
		if (!hdcM) {
			return;
		}

		if (!parentWindow) {
			return;
		}
		HWND parenthwnd = parentWindow->getHWnd();
		if (!IsWindow(parenthwnd)) {
			return;
		}


		if (hdcM) {
			drawEdge();

			//チェックボックス
			int pos1x = pos.x + BOX_POS_X;
			int pos1y = pos.y + size.y / 2 - BOX_WIDTH / 2;
			int pos2x = pos.x + BOX_POS_X + BOX_WIDTH - 1;
			int pos2y = pos.y + size.y / 2 + BOX_WIDTH / 2 - 1;
			hdcM->setPenAndBrush(RGB(240, 240, 240), NULL);
			Rectangle(hdcM->hDC, pos1x, pos1y, pos2x + 1, pos2y + 1);
			if (value) {
				MoveToEx(hdcM->hDC, pos1x + 2, pos1y + 2, NULL);
				LineTo(hdcM->hDC, pos2x - 1, pos2y - 1);
				MoveToEx(hdcM->hDC, pos2x - 2, pos1y + 2, NULL);
				LineTo(hdcM->hDC, pos1x + 1, pos2y - 1);
			}
			hdcM->setPenAndBrush(RGB(min(baseColor.r + 20, 255), min(baseColor.g + 20, 255), min(baseColor.b + 20, 255)), NULL);
			MoveToEx(hdcM->hDC, pos1x, pos1y + 2, NULL);
			LineTo(hdcM->hDC, pos1x, pos2y - 1);
			MoveToEx(hdcM->hDC, pos2x, pos1y + 2, NULL);
			LineTo(hdcM->hDC, pos2x, pos2y - 1);
			MoveToEx(hdcM->hDC, pos1x + 2, pos1y, NULL);
			LineTo(hdcM->hDC, pos2x - 1, pos1y);
			MoveToEx(hdcM->hDC, pos1x + 2, pos2y, NULL);
			LineTo(hdcM->hDC, pos2x - 1, pos2y);

			//名前
			pos1x = pos.x + BOX_POS_X + BOX_WIDTH + 3;
			pos1y = pos.y + size.y / 2 - 5;
			hdcM->setFont(12, _T("ＭＳ ゴシック"));
			//SetTextColor(hdcM->hDC, RGB(240, 240, 240));
			SetTextColor(hdcM->hDC, OrgWindowParts::getTextColor());
			TextOut(hdcM->hDC,
				pos1x, pos1y,
				name, (int)_tcslen(name));
		}
		{
			if (g_dsmousewait == 1) {
				POINT mousepoint;
				::GetCursorPos(&mousepoint);
				if (getParent() && getHDCMaster()) {
					::ScreenToClient(getParent()->getHWnd(), &mousepoint);
					int BMP_W = 52;
					int BMP_H = 50;
					Gdiplus::Graphics* gdipg = new Gdiplus::Graphics(hdcM->hDC);
					if (gdipg) {
						Gdiplus::ImageAttributes attr;
						Gdiplus::ColorMatrix cmat = {
							1.0f, 0.0f, 0.0f, 0.0f, 0.0f,   // Red
							0.0f, 1.0f, 0.0f, 0.0f, 0.0f,   // Green
							0.0f, 0.0f, 1.0f, 0.0f, 0.0f,   // Blue
							0.0f, 0.0f, 0.0f, g_mouseherealpha, 0.0f,   // Alpha (70%)
							0.0f, 0.0f, 0.0f, 0.0f, 1.0f    // must be 1
						};
						attr.SetColorMatrix(&cmat);
						//Gdiplus::Image* imgptr = new Gdiplus::Image(L"E:\\PG\\AdditiveIK\Media\\MameMedia\\img_l105.png");
						if (g_mousehereimage) {
							gdipg->DrawImage(g_mousehereimage, Gdiplus::Rect(mousepoint.x, mousepoint.y, BMP_W, BMP_H),
								0, 0, BMP_W, BMP_H,
								Gdiplus::UnitPixel, &attr, NULL, NULL);
						}
						delete gdipg;
					}
				}
			}
		}

	}

	double OWP_Timeline::calcShowPosTime(double srctime)
	{
		double newshowpostime = showPos_time;

		if ((g_underselectingframe != 0) || (g_previewFlag != 0)) {

			int x1 = MARGIN + LABEL_SIZE_X;
			int x2 = size.x - MARGIN - SCROLL_BAR_WIDTH;

			if (srctime <= showPos_time) {
				newshowpostime = srctime;
			}
			if ((newshowpostime + ((double)x2 - 3 - x1) / timeSize) <= srctime) {
				newshowpostime = srctime - ((double)x2 - 3 - x1) / timeSize;
			}
		}
		else {
			//カレントフレームがタイムラインのセンターに来るように計算

			int x0 = MARGIN + LABEL_SIZE_X;
			int x1 = size.x - MARGIN - SCROLL_BAR_WIDTH;

			double curshowTimeLength = ((double)(x1 - x0 - 3)) / timeSize;
			newshowpostime = srctime - curshowTimeLength / 2.0;

			if (curshowTimeLength < maxTime) {
				//showPos_time= max(0,min( _showPosTime, maxTime-showTimeLength));
				newshowpostime = max(0, min(newshowpostime, maxTime - curshowTimeLength / 2.0));
			}
			else {
				newshowpostime = 0;
			}
		}

		//2022/10/22 グラフ描画開始位置が中途半端な位置にならないようにintキャストしてから返す
		//intキャストしない場合　例えば　オイラーグラフの選択範囲のカレントライン描画位置が　タイムラインのRectangleと微妙に位置ずれする
		return (double)((int)newshowpostime);
	}


	void OWP_Timeline::setCurrentTime(double _currentTime, bool CallListener, bool needRewrite) {
		//default : CallListener = false, needRewrite = true (OWP_EulerGraphのneedRewriteは falseがdefault)
		static int s_paintcnt = 0;
		s_paintcnt++;


		int x1 = MARGIN + LABEL_SIZE_X;
		int x2 = size.x - MARGIN - SCROLL_BAR_WIDTH;

		if (timeSnapSize != 0) {
			_currentTime = floor(_currentTime / timeSnapSize + 0.5)*timeSnapSize;
		}

		currentTime = min(max(_currentTime, 0), maxTime);
		showPos_time = calcShowPosTime(currentTime);

		//リスナーコール
		if (CallListener && (this->cursorListener != NULL)) {
			(this->cursorListener)();
		}

		//再描画要求
		if (needRewrite && rewriteOnChange) {
			callRewrite();
		}
	}

	double OWP_EulerGraph::calcShowPosTime(double srctime)
	{
		double newshowpostime = showPos_time;

		if ((g_underselectingframe != 0) || (g_previewFlag != 0)) {

			int x1 = MARGIN + LABEL_SIZE_X;
			int x2 = size.x - MARGIN - SCROLL_BAR_WIDTH;

			if (srctime <= showPos_time) {
				newshowpostime = srctime;
			}
			if ((newshowpostime + ((double)x2 - 3 - x1) / timeSize) <= srctime) {
				newshowpostime = srctime - ((double)x2 - 3 - x1) / timeSize;
			}
		}
		else {
			//カレントフレームがタイムラインのセンターに来るように計算

			int x0 = MARGIN + LABEL_SIZE_X;
			int x1 = size.x - MARGIN - SCROLL_BAR_WIDTH;

			double curshowTimeLength = ((double)(x1 - x0 - 3)) / timeSize;
			newshowpostime = srctime - curshowTimeLength / 2.0;

			if (curshowTimeLength < maxTime) {
				//showPos_time= max(0,min( _showPosTime, maxTime-showTimeLength));
				newshowpostime = max(0, min(newshowpostime, maxTime - curshowTimeLength / 2.0));
			}
			else {
				newshowpostime = 0;
			}
		}

		//2022/10/22 グラフ描画開始位置が中途半端な位置にならないようにintキャストしてから返す
		//intキャストしない場合　例えば　オイラーグラフの選択範囲のカレントライン描画位置が　タイムラインのRectangleと微妙に位置ずれする
		return (double)((int)newshowpostime);
	}
	void OWP_EulerGraph::setCurrentTime(double _currentTime, bool CallListener, bool needRewrite) {
		//default : CallListener = false, needRewrite = false(OWP_timelineのneedRewriteは trueがdefault)
		static int s_paintcnt = 0;
		s_paintcnt++;

		int x1 = MARGIN + LABEL_SIZE_X;
		int x2 = size.x - MARGIN - SCROLL_BAR_WIDTH;

		if (timeSnapSize != 0) {
			_currentTime = floor(_currentTime / timeSnapSize + 0.5)*timeSnapSize;
		}

		currentTime = min(max(_currentTime, 0), maxTime);
		showPos_time = calcShowPosTime(currentTime);

		//リスナーコール
		if (CallListener && (this->cursorListener != NULL)) {
			(this->cursorListener)();
		}

		//再描画要求
		//if (rewriteOnChange) {
		if (needRewrite && rewriteOnChange) {
			callRewrite();
		}
	}


	int OWP_EulerGraph::getEuler(double srcframe, ChaVector3* dsteul)
	{
		if (!dsteul) {
			return 1;
		}
		if (isseteulminmax == false) {
			return 1;
		}
		if ((int)lineData.size() != 3) {
			return 1;
		}

		double valx, valy, valz;
		int ret;
		ret = lineData[0]->getValue(srcframe, &valx);
		if (ret) {
			return 1;
		}
		ret = lineData[1]->getValue(srcframe, &valy);
		if (ret) {
			return 1;
		}
		ret = lineData[2]->getValue(srcframe, &valz);
		if (ret) {
			return 1;
		}

		dsteul->x = (float)valx;
		dsteul->y = (float)valy;
		dsteul->z = (float)valz;
	
		return 0;

	}

	int OWP_EulerGraph::EulLineData::getValue(double srcframe, double* dstvalue)
	{
		if (!dstvalue) {
			return 1;
		}
		if (parent->isseteulminmax == false) {
			return 1;
		}

		int srcindex = getKeyIndex(srcframe);
		if (srcindex >= 0) {
			*dstvalue = key[srcindex]->value;
			return 0;
		}
		else {
			return 1;
		}
	}


	/// Method : 再描画要求を送る
	void OrgWindowParts::callRewrite(){
		//if( parentWindow==NULL ) return;
		if ((g_endappflag == 0) && parentWindow && IsWindow(parentWindow->getHWnd())) {

			//再描画領域
			RECT tmpRect;
			tmpRect.left = pos.x + 1;
			tmpRect.top = pos.y + 1;
			tmpRect.right = pos.x + size.x - 1;
			tmpRect.bottom = pos.y + size.y - 1;
			//InvalidateRect( parentWindow->getHWnd(), &tmpRect, false );
			InvalidateRect(parentWindow->getHWnd(), &tmpRect, false);

			//draw();
		}
	}

	///<summary>
	///	ウィンドウ内部品"タイムラインテーブル"クラス
	///</summary>
	//const double OWP_Timeline::LineData::TIME_ERROR_WIDTH= 0.0001;

	////////////////----------------------------------------////////////////
	////////////////										////////////////
	////////////////		オリジナルウィンドウクラス		////////////////
	////////////////										////////////////
	////////////////----------------------------------------////////////////

	////////////////////////// MemberVar /////////////////////////////

	//	MemberVar : ウィンドウハンドルとOrgWindowインスタンスポインタの対応マップ
	map<HWND,OrgWindow*> OrgWindow::hWndAndClassMap;

	//////////////////////////// Method //////////////////////////////

	//
	//	Method : ウィンドウプロシージャ
	//
	LRESULT CALLBACK OrgWindow::wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
		map<HWND,OrgWindow*>::iterator owItr;
		owItr= hWndAndClassMap.find(hwnd);
		if( owItr==hWndAndClassMap.end() ){
			//if (message != WM_SETCURSOR) {
				return (DefWindowProc(hwnd, message, wParam, lParam));
			//}
			//else {
			//	return 0;
			//}
		}
		OrgWindow *owner= owItr->second;	//ウィンドウ作成元のクラスのインスタンスポインタ


		static MouseEvent mouseEvent;
		static KeyboardEvent keyboardEvent;
		static POINT tmpPoint;
		mouseEvent.Init();
		keyboardEvent.Init();
		tmpPoint.x = 0;
		tmpPoint.y = 0;

		switch (message){				//イベント作成
			case WM_LBUTTONDBLCLK://2023/10/04
			case WM_RBUTTONDBLCLK://2023/10/04
			case WM_LBUTTONDOWN:
			case WM_RBUTTONDOWN:
			case WM_MBUTTONDOWN:
			case WM_LBUTTONUP:
			case WM_RBUTTONUP:
			case WM_MBUTTONUP:
			case WM_MOUSEMOVE:
			case WM_MOUSEWHEEL:
			case WM_MOUSEHOVER:
			case WM_MOUSELEAVE:
				GetCursorPos(&tmpPoint);

				mouseEvent.globalX= (int)tmpPoint.x;
				mouseEvent.globalY= (int)tmpPoint.y;
				mouseEvent.localX= mouseEvent.globalX - owner->pos.x;
				mouseEvent.localY= mouseEvent.globalY - owner->pos.y;
				mouseEvent.shiftKey= GetKeyState(VK_SHIFT)<0;
				mouseEvent.ctrlKey= GetKeyState(VK_CONTROL)<0;
				mouseEvent.altKey= GetKeyState(VK_MENU)<0;
				mouseEvent.wheeldelta = GET_WHEEL_DELTA_WPARAM(wParam);
				break;
			case WM_KEYDOWN:
			case WM_KEYUP:
				keyboardEvent.keyCode= (int)(unsigned int)wParam;
				keyboardEvent.shiftKey= GetKeyState(VK_SHIFT)<0;
				keyboardEvent.ctrlKey= GetKeyState(VK_CONTROL)<0;
				keyboardEvent.altKey= GetKeyState(VK_MENU)<0;
				keyboardEvent.repeat= (lParam&0x40000000)!=0;
				if( message==WM_KEYUP ){
					keyboardEvent.repeat=false;
					keyboardEvent.onDown=false;
				}else{
					keyboardEvent.onDown=true;
				}
				break;
		}

		switch (message){
			case WM_LBUTTONDBLCLK://2023/10/04
				owner->onLButtonDBLCLK(mouseEvent);//左ボタンダブルクリック
				return 0;
			case WM_RBUTTONDBLCLK://2023/10/04
				owner->onRButtonDBLCLK(mouseEvent);//左ボタンダブルクリック
				return 0;
			case WM_LBUTTONDOWN:		//左マウスボタンダウン
				owner->onLButtonDown(mouseEvent);
				return 0;
			case WM_LBUTTONUP:			//左マウスボタンアップ
				owner->onLButtonUp(mouseEvent);
				return 0;
			case WM_RBUTTONDOWN:		//右マウスボタンダウン
				owner->onRButtonDown(mouseEvent);
				return 0;
			case WM_RBUTTONUP:			//右マウスボタンアップ
				owner->onRButtonUp(mouseEvent);
				return 0;
			case WM_MOUSEMOVE:			//マウス移動
				owner->onMouseMove(mouseEvent);
				return 0;
			case WM_MBUTTONDOWN:
				owner->onMButtonDown(mouseEvent);
				return 0;
			case WM_MBUTTONUP:
				owner->onMButtonUp(mouseEvent);
				return 0;
			case WM_MOUSEWHEEL:
				owner->onMouseWheel(mouseEvent);
				return 0;
			case WM_MOUSEHOVER:
				owner->onMouseHover(mouseEvent);
				return 0;
			case WM_MOUSELEAVE:
				owner->onMouseLeave(mouseEvent);
				return 0;
			case WM_KEYDOWN:			//キーボードイベント
			case WM_KEYUP:
				owner->onKeyboard(keyboardEvent);
				return 0;
			case WM_MOVE:				//位置変更
				owner->refreshPosAndSize();
				return 0;
			case WM_SIZE:				//サイズ変更
				owner->refreshPosAndSize();
				owner->callRewrite();
				owner->nowChangingSize= true;
				return 0;
			case WM_PAINT:				//描画
				owner->allPaint();
				return 0;
			case WM_CLOSE:
				DestroyWindow(hwnd);
				return 0;
			case WM_DESTROY:
				//PostQuitMessage(0);
				return 0;
			default:
				if( (g_endappflag == 0 )&& owner->nowChangingSize ){
					owner->nowChangingSize= false;
					owner->autoResizeAllParts();
				}
				//if (message != WM_SETCURSOR) {
					return (DefWindowProc(hwnd, message, wParam, lParam));
				//}
				break;
		}

		return 0;
	}





}