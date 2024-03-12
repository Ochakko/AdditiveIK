#ifndef ORGWINDOWH
#define ORGWINDOWH

#include <crtdbg.h>
#include <windows.h>
#include <tchar.h>
#include <list>
#include <map>
#include <string>
#include <process.h>
#include <functional>
#include <vector>
#include <gdiplus.h>


//#include <d3dx9.h>
#include <ChaVecCalc.h>


//#define EULPOOLBLKLEN	2048
//#define EULPOOLBLKLEN	65536
//#define KEYPOOLBLKLEN	65536
#define EULPOOLBLKLEN	4096
#define KEYPOOLBLKLEN	4096

#define KEYNUM_ONPREVIEW	5

struct KeyInfo{
	const TCHAR *label;
	double time;
	int timeIndex,lineIndex;
	//ChaVector3 eul, tra;
	ChaMatrix wmat;
	void *object;

	KeyInfo() {
		label = 0;
		time = 0.0;
		timeIndex = 0;
		lineIndex = 0;
		wmat.SetIdentity();
		object = 0;
	};
};

extern int g_endappflag;
extern bool g_4kresolution;//Main.cpp
extern bool g_selecttolastFlag;//Main.cpp
extern bool g_underselecttolast;//Main.cpp
extern bool g_undereditrange;//Main.cpp
//extern HBITMAP g_mouseherebmp;
extern int g_dsmousewait;
//extern COLORREF g_tranbmp;
extern float g_mouseherealpha;
extern Gdiplus::Image* g_mousehereimage;
extern Gdiplus::Image* g_menuaimbarimage;
extern int g_currentsubmenuid;
extern POINT g_currentsubmenupos;
extern bool g_enableDS;
extern int g_submenuwidth;
extern bool g_4kresolution;

extern double g_motionbrush_startframe;
extern double g_motionbrush_endframe;
extern int g_previewFlag;

extern double g_playingstart;
extern double g_playingend;
extern int g_underselectingframe;

extern bool g_preciseOnPreviewToo;

static double TIME_ERROR_WIDTH = 0.0001;


class CModel;


namespace OrgWinGUI{

	//global
void InitEulKeys();
void DestroyEulKeys();
void* GetNewEulKey();
void InitKeys();
void DestroyKeys();
void* GetNewKey();


static void s_dummyfunc();
void s_dummyfunc()
{
	static int dummycnt = 0;
	dummycnt++;
}

/*	//	function : WCHAR → TCHAR
	int wchar2tchar(const WCHAR *src, TCHAR *dst, size_t size){
#ifdef UNICODE
//		return wcscpy_s(dst,size,src);
		return swprintf_s(dst,size,src);
#else
		size_t returnCharCount;
		return wcstombs_s(&returnCharCount,dst,size,src,size-1);
#endif
	}
	//	function : char → TCHAR
	int char2tchar(const char *src, TCHAR *dst, size_t size){
#ifdef UNICODE
		size_t returnCharCount;
		return mbstowcs_s(&returnCharCount,dst,size,src,size-1);
#else
//		return strcpy_s(dst,size,src);
		return sprintf_s(dst,size,src);
#endif
	}//*/
	;

	//--------------------------------------------------//
	//													//
	//	オリジナルツールウィンドウクラス群				//
	//													//
	//--------------------------------------------------//

	class OrgWindow;

	////////////////----------------------------------------////////////////
	////////////////										////////////////
	////////////////	ウィンドウサイズ・位置クラス		////////////////
	////////////////										////////////////
	////////////////----------------------------------------////////////////

	class WindowPos{
	public:
		//////////////////// Constructor/Destructor //////////////////////
		WindowPos(){
			useDefault();
		}
		WindowPos(int _x, int _y){
			x=_x;
			y=_y;
		}


		////////////////////////// MemberVar /////////////////////////////
		int x,y;

		/////////////////////////// Operator /////////////////////////////
		void operator+=(const WindowPos& a){
			x+=a.x;
			y+=a.y;
		}
		void operator-=(const WindowPos& a){
			x-=a.x;
			y-=a.y;
		}

		friend WindowPos operator+(const WindowPos& a, const WindowPos& b){
			return WindowPos(a.x+b.x, a.y+b.y);
		}
		friend WindowPos operator-(const WindowPos& a, const WindowPos& b){
			return WindowPos(a.x-b.x, a.y-b.y);
		}

		//////////////////////////// Method //////////////////////////////
		//	Method : デフォルト位置を適応
		void useDefault(){
			x=CW_USEDEFAULT;
			y=CW_USEDEFAULT;
		}
	};

	class WindowSize{
	public:
		//////////////////// Constructor/Destructor //////////////////////
		WindowSize(){
			useDefault();
		}
		WindowSize(int _x, int _y){
			x=_x;
			y=_y;
		}

		////////////////////////// MemberVar /////////////////////////////
		int x,y;

		/////////////////////////// Operator /////////////////////////////
		void operator+=(const WindowSize& a){
			x+=a.x;
			y+=a.y;
		}
		void operator-=(const WindowSize& a){
			x-=a.x;
			y-=a.y;
		}

		friend WindowSize operator+(const WindowSize& a, const WindowSize& b){
			return WindowSize(a.x+b.x, a.y+b.y);
		}
		friend WindowSize operator-(const WindowSize& a, const WindowSize& b){
			return WindowSize(a.x-b.x, a.y-b.y);
		}

		//////////////////////////// Method //////////////////////////////
		//	Method : デフォルト位置を適応
		void useDefault(){
			x=CW_USEDEFAULT;
			y=CW_USEDEFAULT;
		}
	};

	////////////////----------------------------------------////////////////
	////////////////										////////////////
	////////////////			イベントクラス				////////////////
	////////////////										////////////////
	////////////////----------------------------------------////////////////

	class MouseEvent{
	public:
		MouseEvent() {
			Init();
		}
		~MouseEvent() {
		}
		void Init() {
			localX = 0;
			localY = 0;
			globalX = 0;
			globalY = 0;
			shiftKey = false;
			ctrlKey = false;
			altKey = false;
			wheeldelta = 0;
		}

		int localX,localY;
		int globalX,globalY;
		bool shiftKey,ctrlKey,altKey;
		int wheeldelta;
	};

	class KeyboardEvent{
	public:
		KeyboardEvent()
		{
			Init();
		}
		~KeyboardEvent() {};

		void Init() {
			keyCode = 0;
			shiftKey = false;
			ctrlKey = false;
			altKey = false;
			repeat = false;
			onDown = false;
		}
		int keyCode;
		bool shiftKey,ctrlKey,altKey;
		bool repeat;
		bool onDown;
	};

	//static void s_dummyfuncKey( KeyboardEvent& keye );


	////////////////----------------------------------------////////////////
	////////////////										////////////////
	////////////////			HDC類管理クラス				////////////////
	////////////////										////////////////
	////////////////----------------------------------------////////////////

	class HDCMaster{
	public:
		//////////////////// Constructor/Destructor //////////////////////
		HDCMaster(){
			hWnd=  NULL;
			hDC=   NULL;
			hFont= NULL;
			hPen=  NULL;
			hBrush=NULL;

			backBM=NULL;
			backBM_first=NULL;
			hDC_window=NULL;

			height = 0;
			width = 0;
			ZeroMemory(&paint, sizeof(PAINTSTRUCT));
		}
		HDCMaster( const HDCMaster& a ){
			_ASSERT_EXPR( 0, L"コピーコンストラクタは使えません" );

			hWnd = NULL;
			hDC = NULL;
			hFont = NULL;
			hPen = NULL;
			hBrush = NULL;

			backBM = NULL;
			backBM_first = NULL;
			hDC_window = NULL;

			height = 0;
			width = 0;
			ZeroMemory(&paint, sizeof(PAINTSTRUCT));
		}
		~HDCMaster(){
			if( hFont ){
				DeleteObject(hFont);
				hFont= NULL;
			}
			if( hPen ){
				DeleteObject(hPen);
				hPen= NULL;
			}
			if( hBrush ){
				DeleteObject(hBrush);
				hBrush= NULL;
			}
		}

		////////////////////////// MemberVar /////////////////////////////
		HDC hDC;
		PAINTSTRUCT paint;

		HFONT hFont;
		HPEN hPen;
		HBRUSH hBrush;

		//////////////////////////// Method //////////////////////////////
/*		//	Method : バックバッファサイズ変更
		void refreshBackBaffer(){
			RECT tmpRect;
			GetWindowRect(hWnd, &tmpRect);

			hDC= BeginPaint(hWnd, &paint);
			backDC= CreateCompatibleDC(hDC);
			backBM= CreateCompatibleBitmap(hDC, tmpRect.right-tmpRect.left, tmpRect.bottom-tmpRect.top);
			backBM_old= (HBITMAP)SelectObject(backDC,backBM);
			EndPaint(hWnd, &paint);
		}*/
		//	Method : 描画準備
		void beginPaint(){
			if( hDC==NULL ){
				hDC_window= BeginPaint(hWnd, &paint);

				RECT tmpRect;
				GetWindowRect(hWnd, &tmpRect);
				width= tmpRect.right-tmpRect.left;
				height= tmpRect.bottom-tmpRect.top;

				hDC= CreateCompatibleDC(hDC_window);
				backBM= CreateCompatibleBitmap( hDC_window, width, height );
				backBM_first= (HBITMAP)SelectObject(hDC,backBM);

				SetBkMode(hDC, TRANSPARENT);
			}
		}
		//	Method : 描画終了
		void endPaint(){
			if(hWnd!=NULL){
				//SetBkColor(hDC, g_tranbmp);
				BitBlt(hDC_window, 0,0,width,height, hDC,0,0,SRCCOPY);

				EndPaint(hWnd, &paint);

				SelectObject(hDC,backBM_first);
				DeleteDC(hDC);
				DeleteObject(backBM);

				backBM=NULL;
				backBM_first=NULL;
				hDC=NULL;
			}


			hDC_window= NULL;
			if( hFont ){
				DeleteObject(hFont);
				hFont= NULL;
			}
			if( hPen ){
				DeleteObject(hPen);
				hPen= NULL;
			}
			if( hBrush ){
				DeleteObject(hBrush);
				hBrush= NULL;
			}
		}
		//	Method : フォントを指定
		void setFont(int h, LPCTSTR face){
//			if( hFont ) DeleteObject(hFont);
			hFont = CreateFont( h,					//フォント高さ
								0,					//文字幅
								0,					//テキストの角度
								0,					//ベースラインとｘ軸との角度
								FW_NORMAL,			//フォントの重さ（太さ）
								FALSE,				//イタリック体
								FALSE,				//アンダーライン
								FALSE,				//打ち消し線
								SHIFTJIS_CHARSET,	//文字セット
								OUT_DEFAULT_PRECIS,	//出力精度
								CLIP_DEFAULT_PRECIS,//クリッピング精度
								PROOF_QUALITY,		//出力品質
								FIXED_PITCH | FF_MODERN,//ピッチとファミリー
								face);				//書体名
			DeleteObject( SelectObject(hDC, hFont) );
		}
		//	Method : ペン・ブラシを指定
		//			 penColorをNULLにすると、ブラシと同じカラーが適応されます
		//			 brushColorをNULLにすると、透明なブラシが適応されます
		void setPenAndBrush(COLORREF penColor, COLORREF brushColor, int penStyle=PS_SOLID, int penWidth=1){
//			if( hPen )   DeleteObject(hPen);
//			if( hBrush ) DeleteObject(hBrush);
		
			if( penColor==NULL ){
				hPen = CreatePen(penStyle, penWidth, brushColor);
			}else{
				hPen = CreatePen(penStyle, penWidth, penColor);
			}
			DeleteObject( SelectObject(hDC, hPen) );

			if( brushColor==NULL ){
				DeleteObject( SelectObject(hDC, GetStockObject(NULL_BRUSH)) );
			}else{
				hBrush = CreateSolidBrush(brushColor);
				DeleteObject( SelectObject(hDC, hBrush) );
			}
		}

		/////////////////////////// Accessor /////////////////////////////
		//	Accessor : hWnd
		void setHWnd(HWND _hWnd){
			if( hWnd==NULL ){
				hWnd= _hWnd;

				return;
			}
			_ASSERT_EXPR( 0, L"hWndの多重設定" );
		}

	private:
		////////////////////////// MemberVar /////////////////////////////
		HWND hWnd;
		HDC hDC_window;
		HBITMAP backBM, backBM_first;
		int width,height;
	};

	////////////////----------------------------------------////////////////
	////////////////										////////////////
	////////////////	ウィンドウ内部品用スーパークラス	////////////////
	////////////////										////////////////
	////////////////----------------------------------------////////////////

	class OrgWindowParts{
	public:
		//////////////////// Constructor/Destructor //////////////////////
		OrgWindowParts(){
			parentWindow = NULL;
			isregistered = false;
			isactive = false;
			isslider = false;
			isradiobutton = false;
			isseparator = false;

			//baseColor.r = 70;
			//baseColor.g = 50;
			//baseColor.b = 70;
			baseColor.r = 0;
			baseColor.g = 0;
			baseColor.b = 0;

			textColor = RGB(255, 255, 255);

			pos = WindowPos(0, 0);
			size = WindowSize(0, 0);
			hdcM = 0;

			isplayerbutton = false;
			lrbuttondone = false;
		}
		OrgWindowParts( const OrgWindowParts& a ){
			operator=(a);
		}

		bool getActive()
		{
			return isactive;
		}
		void setActive(bool srcactive)
		{
			isactive = srcactive;
		}
		bool getIsResigtered()
		{
			return isregistered;
		}
		void setIsSlider(bool srcisslider)
		{
			isslider = srcisslider;
		}
		bool getIsSlider()
		{
			return isslider;
		}
		void setIsPlayerButton(bool srcflag) {
			isplayerbutton = srcflag;
		}
		bool getIsPlayerButton()
		{
			return isplayerbutton;
		}
		void setIsRadioButton(bool srcflag)
		{
			isradiobutton = srcflag;
		}
		bool getIsRadioButton()
		{
			return isradiobutton;
		}
		void setIsSeparator(bool srcflag)
		{
			isseparator = srcflag;
		}
		bool getIsSeparator()
		{
			return isseparator;
		}

		OrgWindow* getParentWindow()
		{
			return parentWindow;
		}
		void setDoneFlag(bool srcflag)
		{
			lrbuttondone = srcflag;
		}
		bool getDoneFlag()
		{
			return lrbuttondone;
		}
		

		virtual ~OrgWindowParts(){
			parentWindow = NULL;
		}

		////////////////////////// MemberVar /////////////////////////////
		struct color_tag{
			unsigned char r,g,b;
		};

		//////////////////////////// Method //////////////////////////////
		//	Method : 親ウィンドウに登録
		virtual void registmember( OrgWindow *_parentWindow,
							 WindowPos _pos, WindowSize _size,
							 HDCMaster* _hdcM,
							 //unsigned char _baseR=50, unsigned char _baseG=70, unsigned char _baseB=70 ){
							unsigned char _baseR = 0, unsigned char _baseG = 0, unsigned char _baseB = 0) {
			_registmember(_parentWindow,_pos,_size,_hdcM,_baseR,_baseG,_baseB);
			autoResize();
		}
		/// Method : 自動サイズ設定
		virtual void autoResize(){

		}
		//	Method : 描画
		virtual void draw();
		//virtual void draw(){
		//	drawEdge();
		//}
		/// Method : 再描画要求を送る
		virtual void callRewrite();
		//	Method : 左マウスボタンダウンイベント受信
		virtual void onLButtonDown(const MouseEvent& e){
		}
		//	Method : 左マウスボタン ダブルクリックイベント受信
		virtual void onLButtonDBLCLK(const MouseEvent& e) {//2023/10/04
			int dbgflag1 = 1;
		}
		//	Method : 右マウスボタン ダブルクリックイベント受信
		virtual void onRButtonDBLCLK(const MouseEvent& e) {//2023/10/04
			int dbgflag1 = 1;
		}
		//	Method : 左マウスボタンアップイベント受信
		virtual void onLButtonUp(const MouseEvent& e){
		}
		//	Method : 右マウスボタンダウンイベント受信
		virtual void onRButtonDown(const MouseEvent& e){
		}
		//	Method : 右マウスボタンアップイベント受信
		virtual void onRButtonUp(const MouseEvent& e){
		}
		//	Method : 中マウスボタンダウンイベント受信
		virtual void onMButtonDown(const MouseEvent& e){
		}
		//	Method : 中マウスボタンアップイベント受信
		virtual void onMButtonUp(const MouseEvent& e){
		}
		//	Method : マウス移動イベント受信
		virtual void onMouseMove(const MouseEvent& e){
		}
		virtual void onMouseHover(const MouseEvent& e) {
		}
		virtual void onMouseLeave(const MouseEvent& e) {
		}
		//	Method : キーダウンイベント受信
		virtual void onKeyDown(const KeyboardEvent& e){
		}
		//	Method : キーアップイベント受信
		virtual void onKeyUp(const KeyboardEvent& e){
		}
		virtual void onMouseWheel(const MouseEvent & e){
		}

		/////////////////////////// Operator /////////////////////////////
		void operator=(const OrgWindow& a){
			_ASSERT_EXPR( 0, L"コピーコンストラクタは使えません" );
		}

		/////////////////////////// Accessor /////////////////////////////
		//	Accessor : pos
		HDCMaster* getHDCMaster()
		{
			return hdcM;
		}
		virtual WindowPos getPos() const{
			return WindowPos(pos.x,pos.y);
		}
		virtual void setPos(const WindowPos& _pos){
			//if ((_pos.x >= -4000) && (_pos.x <= 4000) && (_pos.y >= -4000) && (_pos.y <= 4000)) {
				pos = _pos;
			//}
	//		draw()
		}
		//	Accessor : size
		virtual WindowSize getSize() const{
			return WindowSize(size.x,size.y);
		}
		virtual OrgWindow* getParent() const {
			return parentWindow;
		}

		virtual void setSize(const WindowSize& _size){
			//if ((_size.x >= -4000) && (_size.x <= 4000) && (_size.y >= -4000) && (_size.y <= 4000)) {
				size = _size;
			//}
	//		draw();
		}
		//	Accessor : color
		color_tag getBaseColor() const{
			return baseColor;
		}
		void setColor(unsigned char srcr, unsigned char srcg, unsigned char srcb) {
			baseColor.r = srcr;
			baseColor.g = srcg;
			baseColor.b = srcb;
		}
		void setColor(color_tag _color){
			baseColor= _color;
	//		draw();
		}

		COLORREF getTextColor() const {
			return textColor;
		}
		void setTextColor(unsigned char srcr, unsigned char srcg, unsigned char srcb) {
			textColor = RGB(srcr, srcg, srcb);
		}
		void setTextColor(COLORREF srccol) {
			textColor = srccol;
		}

	protected:
		////////////////////////// MemberVar /////////////////////////////
		OrgWindow *parentWindow;
		WindowPos  pos;
		WindowSize size;
		color_tag baseColor;
		COLORREF textColor;

		HDCMaster *hdcM;
		bool isregistered;
		bool isactive;
		bool isslider;
		bool isplayerbutton;
		bool isradiobutton;
		bool isseparator;
		bool lrbuttondone;
		//////////////////////////// Method //////////////////////////////
		//	Method : 親ウィンドウに登録
		void _registmember( OrgWindow *_parentWindow,
					  WindowPos _pos, WindowSize _size,
					  HDCMaster* _hdcM,
					  unsigned char _baseR, unsigned char _baseG, unsigned char _baseB ){
			parentWindow= _parentWindow;
			pos=  _pos;
			size= _size;
			baseColor.r= _baseR;
			baseColor.g= _baseG;
			baseColor.b= _baseB;
			hdcM= _hdcM;
			isregistered = true;
		}
		//	Method : 枠を描画
		void drawEdge(bool fill=true){
			if (isregistered && hdcM) {
				if (getActive()) {
					if (fill) {
						hdcM->setPenAndBrush(RGB(min(baseColor.r + 20 + 30, 255), min(baseColor.g + 20 + 30, 255), min(baseColor.b + 20 + 30, 255)), RGB(baseColor.r + 30, baseColor.g + 30, baseColor.b + 30));
					}
					else {
						hdcM->setPenAndBrush(RGB(min(baseColor.r + 20 + 30, 255), min(baseColor.g + 20 + 30, 255), min(baseColor.b + 20 + 30, 255)), NULL);
					}
					Rectangle(hdcM->hDC, pos.x, pos.y, pos.x + size.x, pos.y + size.y);
				}
				else {
					if (fill) {
						hdcM->setPenAndBrush(RGB(min(baseColor.r + 20, 255), min(baseColor.g + 20, 255), min(baseColor.b + 20, 255)), RGB(baseColor.r, baseColor.g, baseColor.b));
					}
					else {
						hdcM->setPenAndBrush(RGB(min(baseColor.r + 20, 255), min(baseColor.g + 20, 255), min(baseColor.b + 20, 255)), NULL);
					}
					Rectangle(hdcM->hDC, pos.x, pos.y, pos.x + size.x, pos.y + size.y);
				}
			}
		}
	};

	////////////////----------------------------------------////////////////
	////////////////										////////////////
	////////////////		オリジナルウィンドウクラス		////////////////
	////////////////										////////////////
	////////////////----------------------------------------////////////////

	class OrgWindow{
	public:
		//////////////////// Constructor/Destructor //////////////////////
		OrgWindow( int srcistopmost, const TCHAR *_szclassName,HINSTANCE _hInstance,
				   WindowPos _pos,WindowSize _size,
				   const TCHAR *_title,
				   HWND _hWndParent=NULL, bool _visible=true,
				   //unsigned char _baseR=50, unsigned char _baseG=70, unsigned char _baseB=70,
					unsigned char _baseR = 0, unsigned char _baseG = 0, unsigned char _baseB = 0,
				   bool _canQuit=true, bool _canChangeSize=true ){

			parentwindow = 0;

			isactive = false;
			listenmouse = false;
			istopmost = srcistopmost;


			//##########################################################################
			// For Handleing Mouse Event, It is necessary to call setListenMouse(true).
			// マウスイベントを処理するためには、setListenMouse(true)をあらかじめ呼んでおく必要有り。
			//##########################################################################


			//イベントリスナー
			//closeListener = [](){s_dummyfunc();};
			closeListener = NULL;
			keyboardListener = [](const KeyboardEvent &e){s_dummyfunc();};
			ldownListener = []() {s_dummyfunc(); };
			//lupListener = [](){s_dummyfunc();};
			//rupListener = [](){s_dummyfunc();};
			//hoverListener = []() {s_dummyfunc(); };
			//leaveListener = []() {s_dummyfunc(); };
			lupListener = NULL;
			rupListener = NULL;
			hoverListener = NULL;
			leaveListener = NULL;
			
			//マウスキャプチャ用のフラグ
			mouseCaptureFlagL=mouseCaptureFlagR=false;

			pos=  _pos;
			size= _size;
			sizeMin= _size;
			hInstance= _hInstance;
			szclassName= new TCHAR[256];
			_tcscpy_s(szclassName,256,_szclassName);
			hWndParent= _hWndParent;
			title= new TCHAR[256];
			_tcscpy_s(title,256,_title);
			baseR= _baseR;
			baseG= _baseG;
			baseB= _baseB;
			canQuit= _canQuit;
			canChangeSize= _canChangeSize;

			partsAreaPos=  WindowPos( 3, 15 );
			partsAreaSize= WindowSize( size.x-partsAreaPos.x-3, size.y-partsAreaPos.y-3 );
			currentPartsSizeY= 0;

			isblacktheme = false;
			//isblacktheme = true;

			lrbuttondone = false;

			partsList.clear();

			//ウィンドウクラスを登録
			registerWindowClass();

			//ウィンドウ作成
			create();

			//ウィンドウ表示
			if (hWnd) {
				ShowWindow(hWnd, SW_SHOW);
			}

			beginPaint();
			paintTitleBar();
			endPaint();

			if (hWnd) {
				UpdateWindow(hWnd);
			}

			//表示非表示
			setVisible(_visible);
			
			//サイズ変更中フラグ
			nowChangingSize= false;

		}
		OrgWindow( const OrgWindow& a ){
			operator=(a);
		}
		~OrgWindow(){
			if(hWnd!=NULL){
				DestroyWindow(hWnd);
				hWndAndClassMap.erase(hWnd);	//hWndとこのクラスのインスタンスポインタの対応表更新
			}

			hWnd = NULL;
			hWndParent = NULL;
			parentwindow = 0;

			delete[] szclassName;
			delete[] title;
		}

		bool getActive()
		{
			return isactive;
		}

		void setBlackTheme()
		{
			isblacktheme = true;
		}

		void setListenMouse(bool srcflag)
		{
			listenmouse = srcflag;
		}
		bool getListenMouse()
		{
			return listenmouse;
		}

		void setDoneFlag(bool srcflag)
		{
			lrbuttondone = srcflag;
		}
		bool getDoneFlag()
		{
			return lrbuttondone;
		}

		//////////////////////////// Method //////////////////////////////
		//	Method : ウィンドウ内部品を追加
		void setBackGroundColor(bool srcisactive) {
			if (hWnd == NULL) return;

			isactive = srcisactive;

			if (isactive) {
				if (isblacktheme) {
					baseR = 0;
					baseG = 0;
					baseB = 0;
				}
				else {
					//baseR = 255;
					//baseG = 128;
					//baseB = 64;
					baseR = 100;
					baseG = 100;
					baseB = 100;
				}
			}
			else {
				//baseR = 70;
				//baseG = 50;
				//baseB = 70;
				baseR = 0;
				baseG = 0;
				baseB = 0;
			}

			HBRUSH brush = CreateSolidBrush(RGB(baseR, baseG, baseB));
			SetClassLongPtr(hWnd, GCLP_HBRBACKGROUND, (LONG_PTR)brush);

			//UpdateWindow(hWnd);

			//std::list<OrgWindowParts*> tmpPartsList = partsList;
			//partsList.clear();
			//currentPartsSizeY = 0;
			//for (std::list<OrgWindowParts*>::iterator itr = tmpPartsList.begin();
			//	itr != tmpPartsList.end(); itr++) {
			//	(*itr)->setColor(baseR, baseG, baseB);
			//}


			callRewrite();

			//allPaint();

			//std::list<OrgWindowParts*> tmpPartsList = partsList;
			//partsList.clear();
			//currentPartsSizeY = 0;
			//for (std::list<OrgWindowParts*>::iterator itr = tmpPartsList.begin();
			//	itr != tmpPartsList.end(); itr++) {
			//	(*itr)->setActive(srcisactive);
			//	(*itr)->setColor(baseR, baseG, baseB);
			//	(*itr)->callRewrite();
			//	//(*itr)->draw();
			//}

			//callRewrite();
			//UpdateWindow(hWnd);

			//if (isactive) {
			//	HBRUSH brush = CreateSolidBrush(RGB(min(baseR + 30, 255), min(baseG + 30, 255), min(baseB + 30, 255)));
			//	SetClassLongPtr(hWnd, GCLP_HBRBACKGROUND, (LONG_PTR)brush);
			//	DeleteObject(brush);
			//	InvalidateRect(hWnd, NULL, true);
			//	UpdateWindow(hWnd);
			//}
			//else {
			//	HBRUSH brush = CreateSolidBrush(RGB(baseR, baseG, baseB));
			//	SetClassLongPtr(hWnd, GCLP_HBRBACKGROUND, (LONG_PTR)brush);
			//	DeleteObject(brush);
			//	InvalidateRect(hWnd, NULL, true);
			//	UpdateWindow(hWnd);
			//}
		};

		void addParts(OrgWindowParts& a){
			if (hWnd == NULL) return;


			////整合性チェック
			//if (!partsList.empty()) {
			//	std::list<OrgWindowParts*>::iterator itr = partsList.begin();
			//	if (*itr) {
			//		OrgWindow* parent = (*itr)->getParent();
			//		if (parent) {
			//			if (parent != this) {
			//				return;
			//			}
			//		}
			//		else {
			//			return;
			//		}
			//	}
			//	else {
			//		return;
			//	}
			//}

			a.registmember( this,
					  WindowPos(partsAreaPos.x,partsAreaPos.y+currentPartsSizeY),
					  WindowSize(partsAreaSize.x,partsAreaSize.y-currentPartsSizeY),
					  &hdcM,
					  baseR,baseG,baseB);
			currentPartsSizeY+= a.getSize().y+1;
			partsList.push_back(&a);


			//2023/02/17追加
			//modelpanelにScrollWndをaddしている最中に
			//指定と違う場所に大きい白い領域が表示されるのを防ぐ
			autoResizeAllParts();


			//2023/02/17コメントアウト
			//setPos(0, 30)したWindowPartsをaddPartsすると
			//フレーム組み込みOrgWindowのサイズが変わってしまう不具合があったため　コメントアウト
			//if( (partsAreaPos.y + currentPartsSizeY + 2) >= size.y ){		//ウィンドウからはみ出る場合はサイズを調整
			//	setSize( WindowSize(size.x, partsAreaPos.y+currentPartsSizeY+2) );
			//}

			//refreshPosAndSize();//2022/09/20


			RECT tmpRect;
			tmpRect.left=   a.getPos().x+1;
			tmpRect.top=    a.getPos().y+1;
			tmpRect.right=  a.getPos().x+ a.getSize().x-1;
			tmpRect.bottom= a.getPos().y+ a.getSize().y-1;
			InvalidateRect( hWnd, NULL, false );
		}
		/// Method : 再描画要求を送る
		void callRewrite(){
			if( hWnd==NULL ) return;

			////再描画領域
			RECT tmpRect;
			tmpRect.left=   0;
			tmpRect.top=    0;
			tmpRect.right=  size.x;
			tmpRect.bottom= size.y;
			InvalidateRect( hWnd, &tmpRect, false );
			
			std::list<OrgWindowParts*>::iterator itr;
			for (itr = partsList.begin(); itr != partsList.end(); itr++) {
				if (*itr) {
					//(*itr)->draw();
					(*itr)->callRewrite();
				}
			}
		}

		/// Method : ウィンドウ内部品を全て追加しなおす
		void reAddAllParts(){
			std::list<OrgWindowParts*> tmpPartsList= partsList;
			partsList.clear();
			currentPartsSizeY= 0;
			std::list<OrgWindowParts*>::iterator itr;
			for(itr = tmpPartsList.begin(); itr != tmpPartsList.end(); itr++){
				addParts(**itr);
			}
		}
		/// Method : ウィンドウ内部品の位置・サイズを全て自動設定する
		void autoResizeAllParts(){
			currentPartsSizeY= 0;

			//全ての内部要素に対して位置・サイズを自動設定
			std::list<OrgWindowParts*>::iterator itr;
			for (itr = partsList.begin(); itr != partsList.end(); itr++) {
				if (*itr) {
					(*itr)->setPos(WindowPos(partsAreaPos.x, partsAreaPos.y + currentPartsSizeY));
					(*itr)->setSize(WindowSize(partsAreaSize.x, partsAreaSize.y - currentPartsSizeY));
					(*itr)->autoResize();
					currentPartsSizeY += (*itr)->getSize().y + 1;
				}
			}

			//再描画要求を送る
			callRewrite();
		}
		//	Method : ウィンドウ位置とサイズの更新
		void refreshPosAndSize() {
			if (hWnd == NULL) return;

			RECT tmpRect;
			GetWindowRect(hWnd, &tmpRect);
			pos.x = tmpRect.left;
			pos.y = tmpRect.top;
			size.x = tmpRect.right - tmpRect.left;
			size.y = tmpRect.bottom - tmpRect.top;

			//最小ウィンドウサイズ未満になって居ないかどうか確認
			if ((size.x < sizeMin.x) || (size.y < sizeMin.y)) {
				if (size.x < sizeMin.x) size.x = sizeMin.x;
				if (size.y < sizeMin.y) size.y = sizeMin.y;
				setSize(size);
			}

			partsAreaSize = WindowSize(size.x - partsAreaPos.x - 3, size.y - partsAreaPos.y - 3);
		}

		/////////////////////////// Operator /////////////////////////////
		void operator=(const OrgWindow& a){
			_ASSERT_EXPR( 0, L"コピーコンストラクタは使えません" );
		}

		/////////////////////////// Accessor /////////////////////////////
		//	Accessor : pos
		WindowPos getPos(){
			if( hWnd==NULL ) _ASSERT_EXPR( 0, L"hWnd = NULL" );
			if (hWnd != 0) {
				refreshPosAndSize();
				return WindowPos(pos.x, pos.y);
			}
			else {
				_ASSERT(0);
				return WindowPos(0, 0);
			}
		}
		void setPos(const WindowPos& _pos){
			if( hWnd==NULL ) _ASSERT_EXPR( 0, L"hWnd = NULL" );
			if (hWnd) {
				//if ((_pos.x >= -4000) && (_pos.x <= 4000) && (_pos.y >= -4000) && (_pos.y <= 4000)) {
				pos = _pos;
				//}
				MoveWindow(hWnd, pos.x, pos.y, size.x, size.y, true);
			}
		}
		//	Accessor : size
		WindowSize getSizeSimple() {
			if (hWnd == NULL) _ASSERT_EXPR(0, L"hWnd = NULL");
			if (hWnd) {
				return size;
			}
			else {
				_ASSERT(0);
				return WindowSize(0, 0);
			}
		}
		WindowSize getSize(){
			if( hWnd==NULL ) _ASSERT_EXPR( 0, L"hWnd = NULL" );
			if (hWnd) {
				refreshPosAndSize();
				return size;
			}
			else {
				_ASSERT(0);
				return WindowSize(0, 0);
			}
		}
		WindowSize getClientSize() {
			if (hWnd == NULL) _ASSERT_EXPR(0, L"hWnd = NULL");
			if (hWnd) {
				refreshPosAndSize();
				return WindowSize(partsAreaSize.x, partsAreaSize.y);
			}
			else {
				_ASSERT(0);
				return WindowSize(0, 0);
			}
		}
		void setSize(const WindowSize& _size){
			if( hWnd==NULL ) _ASSERT_EXPR( 0, L"hWnd = NULL" );
			if (hWnd) {
				//if ((_size.x >= -4000) && (_size.x <= 4000) && (_size.y >= -4000) && (_size.y <= 4000)) {
				if (_size.x < sizeMin.x) size.x = sizeMin.x;
				else					size.x = _size.x;
				if (_size.y < sizeMin.y) size.y = sizeMin.y;
				else					size.y = _size.y;
				//}

				MoveWindow(hWnd, pos.x, pos.y, size.x, size.y, true);
			}
			else {
				_ASSERT(0);
			}
		}
		//	Accessor : sizeMin
		WindowSize getSizeMin(){
			if( hWnd==NULL ) _ASSERT_EXPR( 0, L"hWnd = NULL" );

			return sizeMin;
		}
		void setSizeMin(const WindowSize& _sizeMin){
			if( hWnd==NULL ) _ASSERT_EXPR( 0, L"hWnd = NULL" );
			//if ((_sizeMin.x >= -4000) && (_sizeMin.x <= 4000) && (_sizeMin.y >= -4000) && (_sizeMin.y <= 4000)) {
				sizeMin = _sizeMin;
			//}
			if ((size.x < sizeMin.x) || (size.y < sizeMin.y)) {
				if (size.x < sizeMin.x) size.x = sizeMin.x;
				if (size.y < sizeMin.y) size.y = sizeMin.y;
			}
			if (hWnd) {
				MoveWindow(hWnd, pos.x, pos.y, size.x, size.y, true);
			}
			else {
				_ASSERT(0);
			}
		}
		//	Accessor : hWnd
		HWND getHWnd(){
			return hWnd;
		}
		//	Accessor : closeListener
		void setCloseListener(std::function<void()> listener){
			this->closeListener= listener;
		}
		void setLDownListener(std::function<void()> listener) {
			this->ldownListener = listener;
		}
		void setLUpListener(std::function<void()> listener){
			this->lupListener= listener;
		}
		void setRUpListener(std::function<void()> listener){
			this->rupListener= listener;
		}
		void setHoverListener(std::function<void()> listener) {
			this->hoverListener = listener;
		}
		void setLeaveListener(std::function<void()> listener) {
			this->leaveListener = listener;
		}
		/// Accessor : keyboardListener
		void setKeyboardEventListener(std::function<void(const KeyboardEvent&)> listener){
			this->keyboardListener= listener;
		}
		//	Accessor : visible
		bool getVisible(){
			return visible;
		}
		void setVisible(bool value){
			visible = value;
			if (hWnd) {
				if (value) {
					ShowWindow(hWnd, SW_SHOWNA);
				}
				else {
					ShowWindow(hWnd, SW_HIDE);
				}
			}
		}



	private:
		////////////////////////// MemberVar /////////////////////////////
		HWND hWnd,hWndParent;
		HINSTANCE hInstance;
		int istopmost;
		TCHAR *szclassName;
		WindowPos  pos;
		WindowSize size,sizeMin;
		WNDCLASSEX wcex;
		WindowPos  partsAreaPos;
		WindowSize partsAreaSize;
		int currentPartsSizeY;
		bool visible;
		bool nowChangingSize;
		bool canChangeSize;
		bool isactive;

		HDCMaster hdcM;

		TCHAR *title;
		unsigned char baseR,baseG,baseB;
		bool canQuit;

		std::list<OrgWindowParts*> partsList;

		static std::map<HWND,OrgWindow*> hWndAndClassMap;

		//ユーザーイベントリスナー
		std::function<void()> closeListener;
		std::function<void()> ldownListener;
		std::function<void()> lupListener;
		std::function<void()> rupListener;
		std::function<void()> hoverListener;
		std::function<void()> leaveListener;
		std::function<void(const KeyboardEvent&)> keyboardListener;

		//マウスキャプチャ用のフラグ
		bool mouseCaptureFlagL,mouseCaptureFlagR;

		bool isblacktheme;
		bool listenmouse;

		OrgWindow* parentwindow;
		bool lrbuttondone;

		//////////////////////////// Method //////////////////////////////
		//	Method : ウィンドウクラスを登録

/*
	//ウィンドウクラスを登録
	WNDCLASSEX wcex;
	ZeroMemory((LPVOID)&wcex, sizeof(WNDCLASSEX));
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = 0;
	wcex.lpfnWndProc = InfoWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = (HINSTANCE)GetModuleHandle(NULL);
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"InfoWindow_";
	wcex.hIconSm = NULL;
	RegisterClassEx(&wcex);



	//ウィンドウ作成
	int cxframe = GetSystemMetrics(SM_CXFRAME);
	int cyframe = GetSystemMetrics(SM_CYFRAME);
	//if (istopmost) {
	//	hWnd = CreateWindowEx(//WS_EX_TOOLWINDOW|WS_EX_TOPMOST,szclassName,title,WS_POPUP,
	//		WS_EX_LEFT | WS_EX_TOPMOST, szclassName, title, WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE,
	//		pos.x, pos.y,
	//		size.x, size.y,
	//		hWndParent, NULL, hInstance, NULL);
	//}
	//else {
	m_hWnd = CreateWindowEx(
		WS_EX_LEFT, L"InfoWindow_", L"InfoWindow", WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE | WS_THICKFRAME,
		srcposx, srcposy,
		srcwidth, srcheight - 2 * cyframe,
		srcparentwnd, NULL, (HINSTANCE)GetModuleHandle(NULL), NULL);

*/


		void registerWindowClass(){
			HBRUSH blkbrush = CreateSolidBrush(RGB(0, 0, 0));//自分で削除しない　DestroyWindow時に解放される

			ZeroMemory((LPVOID)&wcex, sizeof(WNDCLASSEX));

			wcex.cbSize			= sizeof(WNDCLASSEX);
			//wcex.style			= 0;
			wcex.style = CS_DBLCLKS;//2023/10/04 ダブルクリックを受け取るために必要
			wcex.lpfnWndProc	= wndProc;
			wcex.cbClsExtra		= 0;
			wcex.cbWndExtra		= 0;
			//wcex.hInstance		= hInstance;
			wcex.hInstance = (HINSTANCE)GetModuleHandle(NULL);
			wcex.hIcon			= NULL;
			wcex.hCursor		= LoadCursor(NULL,IDC_ARROW);
			//wcex.hCursor		= NULL;
			//wcex.hbrBackground	= ( HBRUSH)( COLOR_WINDOW+1);
			wcex.hbrBackground = blkbrush;// (HBRUSH)COLOR_BACKGROUND + 1;
			wcex.lpszMenuName	= NULL;
			wcex.lpszClassName	= szclassName;
			wcex.hIconSm		= NULL;
			RegisterClassExW(&wcex);
		}
		//	Method : ウィンドウ作成
		void create(){
			if (hWndParent != NULL) {
				if (istopmost) {
					hWnd = CreateWindowExW(//WS_EX_TOOLWINDOW|WS_EX_TOPMOST,szclassName,title,WS_POPUP,
						//WS_EX_TOOLWINDOW | WS_EX_TOPMOST, szclassName, title, WS_POPUP,// | WS_THICKFRAME,
						//WS_EX_LEFT | WS_EX_TOPMOST, szclassName, title, WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE,// | WS_THICKFRAME,
						WS_EX_LEFT | WS_EX_TOPMOST, szclassName, title, WS_POPUP,// | WS_THICKFRAME,
						pos.x, pos.y,
						size.x, size.y,
						hWndParent, NULL, hInstance, NULL);
				}
				else {
					hWnd = CreateWindowExW(
						WS_EX_LEFT, szclassName, title, WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE,// | WS_THICKFRAME,
						pos.x, pos.y,
						size.x, size.y,
						hWndParent, NULL, hInstance, NULL);
				}
				if (hWnd) {
					SetParent(hWnd, hWndParent);
				}
			}
			else {
				if (istopmost) {
					hWnd = CreateWindowExW(WS_EX_LEFT | WS_EX_TOPMOST,szclassName,title, WS_POPUP,// | WS_THICKFRAME,
						pos.x, pos.y,
						size.x, size.y,
						hWndParent, NULL, hInstance, NULL);
				}
				else {
					hWnd = CreateWindowExW(WS_EX_LEFT,szclassName,title, WS_POPUP,// | WS_THICKFRAME,
						pos.x, pos.y,
						size.x, size.y,
						hWndParent, NULL, hInstance, NULL);
				}
			}
			if (hWnd) {
				hdcM.setHWnd(hWnd);

				hWndAndClassMap[hWnd] = this;	//hWndとこのクラスのインスタンスポインタの対応表更新
			}
			else {
				_ASSERT(0);
			}
		}
		//	Method : ウィンドウプロシージャ
		static LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
		//	Method : 全描画
		void allPaint();
		//void allPaint(){
		//	static int s_paintcnt = 0;
		//	s_paintcnt++;
		//	if (g_previewFlag != 0) {
		//		if ((s_paintcnt % 10) != 0) {
		//			return;
		//		}
		//	}


		//	beginPaint();
		//		paintTitleBar();
		//		for( std::list<OrgWindowParts*>::iterator itr=partsList.begin();
		//				itr!= partsList.end();
		//				itr++ ){
		//			(*itr)->draw();
		//		}
		//	endPaint();
		//}
		///	Method : 左右マウスボタンダウンイベント受信
		virtual void onLButtonDown(const MouseEvent& e){
			if (!listenmouse) {
				return;//!!!!!!!!!!!!!!!!
			}
			if (this->ldownListener != NULL) {
				(this->ldownListener)();
			}
			onLRButtonDown(e,true);
		}
		virtual void onLButtonDBLCLK(const MouseEvent& e) {
			if (!listenmouse) {
				return;//!!!!!!!!!!!!!!!!
			}

			int xButtonX1 = size.x - 1 - 2 - 9;
			int xButtonY1 = 1 + 2;
			int xButtonX2 = xButtonX1 + 9;
			int xButtonY2 = xButtonY1 + 9;
			refreshPosAndSize();

			//マウスキャプチャ
			//if (!mouseCaptureFlagL && !mouseCaptureFlagR) SetCapture(hWnd);
			//if (lButton) mouseCaptureFlagL = true;
			//else		  mouseCaptureFlagR = true;

			//内部パーツ
			std::list<OrgWindowParts*>::iterator plItr;
			for (plItr = partsList.begin(); plItr != partsList.end(); plItr++) {
				if (*plItr) {

					WindowPos chkpos = (*plItr)->getPos();
					WindowSize chksize = (*plItr)->getSize();
					//############################################################
					//2023/10/03 マウス位置が子供ウインドウ内部にある場合だけ処理
					//############################################################
					if ((e.localX >= chkpos.x) && (e.localX <= (chkpos.x + chksize.x)) &&
						(e.localY >= chkpos.y) && (e.localY <= (chkpos.y + chksize.y))) {

						MouseEvent mouseEvent;
						mouseEvent.globalX = e.globalX;
						mouseEvent.globalY = e.globalY;
						mouseEvent.localX = e.localX - chkpos.x;//!!!!
						mouseEvent.localY = e.localY - chkpos.y;//!!!!
						mouseEvent.altKey = e.altKey;
						mouseEvent.shiftKey = e.shiftKey;
						mouseEvent.ctrlKey = e.ctrlKey;
						mouseEvent.wheeldelta = e.wheeldelta;

						(*plItr)->onLButtonDBLCLK(mouseEvent);
						//setDoneFlag(1);
						//return;
					}
				}
			}
		}
		//	Method : 右マウスボタン ダブルクリックイベント受信
		virtual void onRButtonDBLCLK(const MouseEvent& e) {//2023/10/04
			if (!listenmouse) {
				return;//!!!!!!!!!!!!!!!!
			}

			int xButtonX1 = size.x - 1 - 2 - 9;
			int xButtonY1 = 1 + 2;
			int xButtonX2 = xButtonX1 + 9;
			int xButtonY2 = xButtonY1 + 9;
			refreshPosAndSize();

			//マウスキャプチャ
			//if (!mouseCaptureFlagL && !mouseCaptureFlagR) SetCapture(hWnd);
			//if (lButton) mouseCaptureFlagL = true;
			//else		  mouseCaptureFlagR = true;

			//内部パーツ
			std::list<OrgWindowParts*>::iterator plItr;
			for (plItr = partsList.begin(); plItr != partsList.end(); plItr++) {
				if (*plItr) {

					WindowPos chkpos = (*plItr)->getPos();
					WindowSize chksize = (*plItr)->getSize();
					//############################################################
					//2023/10/03 マウス位置が子供ウインドウ内部にある場合だけ処理
					//############################################################
					if ((e.localX >= chkpos.x) && (e.localX <= (chkpos.x + chksize.x)) &&
						(e.localY >= chkpos.y) && (e.localY <= (chkpos.y + chksize.y))) {

						MouseEvent mouseEvent;
						mouseEvent.globalX = e.globalX;
						mouseEvent.globalY = e.globalY;
						mouseEvent.localX = e.localX - chkpos.x;//!!!!
						mouseEvent.localY = e.localY - chkpos.y;//!!!!
						mouseEvent.altKey = e.altKey;
						mouseEvent.shiftKey = e.shiftKey;
						mouseEvent.ctrlKey = e.ctrlKey;
						mouseEvent.wheeldelta = e.wheeldelta;

						(*plItr)->onRButtonDBLCLK(mouseEvent);
						//setDoneFlag(1);
						//return;
					}
				}
			}
		}

		virtual void onRButtonDown(const MouseEvent& e){
			if (!listenmouse) {
				return;//!!!!!!!!!!!!!!!!
			}
			onLRButtonDown(e,false);
		}
		virtual void onLRButtonDown(const MouseEvent& e, bool lButton){
			if (!listenmouse) {
				return;//!!!!!!!!!!!!!!!!
			}

			//2023/10/15 再び下方へソースの位置を戻す
			// ここで呼ぶと　タイトルバードラッグなどが効かない
			// 
			////2023/09/28 上方へ移動
			////マウスキャプチャ
			//if (!mouseCaptureFlagL && !mouseCaptureFlagR) SetCapture(hWnd);
			//if (lButton) mouseCaptureFlagL = true;
			//else		  mouseCaptureFlagR = true;



			int xButtonX1=size.x-1-2-9;
			int xButtonY1=1+2;
			int xButtonX2=xButtonX1+9;
			int xButtonY2=xButtonY1+9;
			refreshPosAndSize();

			if( lButton ){
				//タイトルバーのXボタン
				if( (xButtonX1 <= e.localX) && (e.localX <= xButtonX2 - 1)
				 && (xButtonY1 <= e.localY) && (e.localY <= xButtonY2 - 1)
				 && canQuit ){
					if(this->closeListener!=NULL){
						(this->closeListener)();
					}
					return;
				}


				
				//////タイトルバー
				//if( (1 <= e.localX) && (e.localX <= size.x - 2)
				if ((1 <= e.localX) && (e.localX <= (size.x - 20 - 8)) //2023/10/15 スクロールバーの幅 20 : スクロールバーのSetCaptureを呼ぶため除外
				 && (1 <= e.localY) && (e.localY <= 1 + 2 + 9 + 1) ){
					if (istopmost) {
						SendMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
					}
					return;
				}

				//右下の隅
				if( canChangeSize &&
					((size.x - 4) <= e.localX) && ((size.y - 4) <= e.localY) ){
					if (istopmost) {
						SendMessage(hWnd, WM_NCLBUTTONDOWN, HTBOTTOMRIGHT, 0);
					}
					return;
				}
			}


			//マウスキャプチャ
			if( !mouseCaptureFlagL && !mouseCaptureFlagR ) SetCapture(hWnd);
			if( lButton ) mouseCaptureFlagL=true;
			else		  mouseCaptureFlagR=true;


			//内部パーツ
			setDoneFlag(false);
			std::list<OrgWindowParts*>::iterator plItr;
			int loopcount;
			for (loopcount = 0; loopcount < 2; loopcount++) {
				for (plItr = partsList.begin(); plItr != partsList.end(); plItr++) {
					if (((loopcount == 0) && *plItr && ((*plItr)->getIsPlayerButton())) ||
						((loopcount == 1) && *plItr && !((*plItr)->getIsPlayerButton())) && !getDoneFlag()) {
						WindowSize partsSize = (*plItr)->getSize();
						int tmpPosX = e.localX - (*plItr)->getPos().x;
						int tmpPosY = e.localY - (*plItr)->getPos().y;
						if ((0 <= tmpPosX) && (tmpPosX < partsSize.x) &&
							(0 <= tmpPosY) && (tmpPosY < partsSize.y)) {


							MouseEvent mouseEvent;
							mouseEvent.globalX = e.globalX;
							mouseEvent.globalY = e.globalY;
							mouseEvent.localX = tmpPosX;
							mouseEvent.localY = tmpPosY;
							mouseEvent.altKey = e.altKey;
							mouseEvent.shiftKey = e.shiftKey;
							mouseEvent.ctrlKey = e.ctrlKey;

							if (lButton) {
								(*plItr)->onLButtonDown(mouseEvent);
							}
							else {
								(*plItr)->onRButtonDown(mouseEvent);
							}
							//setDoneFlag(1);
							//return;

							//if ((*plItr)->getIsPlayerButton()) {
							//	return;
							//}

							(*plItr)->setDoneFlag(true);
							setDoneFlag(true);
							return;

						}
					}
				}
			}
		}
		//	Method : 左右マウスボタンアップイベント受信
		virtual void onLButtonUp(const MouseEvent& e){
			if (!listenmouse) {
				return;//!!!!!!!!!!!!!!!!
			}

			if( this->lupListener!=NULL ){
				(this->lupListener)();
			}
			onLRButtonUp(e,true);
		}
		virtual void onRButtonUp(const MouseEvent& e){
			if (!listenmouse) {
				return;//!!!!!!!!!!!!!!!!
			}
			onLRButtonUp(e,false);
			if (this->rupListener != NULL){
				(this->rupListener)();
			}
		}
		virtual void onLRButtonUp(const MouseEvent& e, bool lButton){
			if (!listenmouse) {
				return;//!!!!!!!!!!!!!!!!
			}

			//マウスキャプチャリリース
			if( lButton ) mouseCaptureFlagL=false;
			else		  mouseCaptureFlagR=false;
			if( !mouseCaptureFlagL && !mouseCaptureFlagR ) ReleaseCapture();
		


			//内部パーツ
			//setDoneFlag(0);
			std::list<OrgWindowParts*>::iterator plItr;
			for( plItr = partsList.begin(); plItr!=partsList.end(); plItr++ ){
				//if (*plItr && (*plItr)->getParentWindow() && ((*plItr)->getParentWindow()->getDoneFlag())) {
				if (*plItr && (*plItr)->getDoneFlag()) {
					MouseEvent mouseEvent;
					mouseEvent.globalX = e.globalX;
					mouseEvent.globalY = e.globalY;
					mouseEvent.localX = e.localX - (*plItr)->getPos().x;
					mouseEvent.localY = e.localY - (*plItr)->getPos().y;
					mouseEvent.altKey = e.altKey;
					mouseEvent.shiftKey = e.shiftKey;
					mouseEvent.ctrlKey = e.ctrlKey;

					if (lButton) {
						(*plItr)->onLButtonUp(mouseEvent);
					}
					else {
						(*plItr)->onRButtonUp(mouseEvent);
					}
					//setDoneFlag(0);
					//return;

					(*plItr)->setDoneFlag(0);
					return;

				}
			}
			setDoneFlag(false);
		}
		virtual void onMButtonDown(const MouseEvent& e){
			if (!listenmouse) {
				return;//!!!!!!!!!!!!!!!!
			}

			int xButtonX1 = size.x - 1 - 2 - 9;
			int xButtonY1 = 1 + 2;
			int xButtonX2 = xButtonX1 + 9;
			int xButtonY2 = xButtonY1 + 9;
			refreshPosAndSize();

			//マウスキャプチャ
			//if (!mouseCaptureFlagL && !mouseCaptureFlagR) SetCapture(hWnd);
			//if (lButton) mouseCaptureFlagL = true;
			//else		  mouseCaptureFlagR = true;

			//内部パーツ
			//setDoneFlag(0);
			std::list<OrgWindowParts*>::iterator plItr;
			for (plItr = partsList.begin(); plItr != partsList.end(); plItr++){
				//if (*plItr && (*plItr)->getParentWindow() && !((*plItr)->getParentWindow()->getDoneFlag())) {
				if (*plItr) {
					WindowSize partsSize = (*plItr)->getSize();
					int tmpPosX = e.localX - (*plItr)->getPos().x;
					int tmpPosY = e.localY - (*plItr)->getPos().y;
					if ((0 <= tmpPosX) && (tmpPosX < partsSize.x) &&
						(0 <= tmpPosY) && (tmpPosY < partsSize.y)) {

						MouseEvent mouseEvent;
						mouseEvent.globalX = e.globalX;
						mouseEvent.globalY = e.globalY;
						mouseEvent.localX = tmpPosX;
						mouseEvent.localY = tmpPosY;
						mouseEvent.altKey = e.altKey;
						mouseEvent.shiftKey = e.shiftKey;
						mouseEvent.ctrlKey = e.ctrlKey;

						(*plItr)->onMButtonDown(mouseEvent);
						//setDoneFlag(1);

						//return;
					}
				}
			}

		}
		virtual void onMButtonUp(const MouseEvent& e){
			if (!listenmouse) {
				return;//!!!!!!!!!!!!!!!!
			}

			//マウスキャプチャリリース
			//if (lButton) mouseCaptureFlagL = false;
			//else		  mouseCaptureFlagR = false;
			//if (!mouseCaptureFlagL && !mouseCaptureFlagR) ReleaseCapture();

			//内部パーツ
			//setDoneFlag(0);
			std::list<OrgWindowParts*>::iterator plItr;
			for (plItr = partsList.begin(); plItr != partsList.end(); plItr++){
				//if (*plItr && (*plItr)->getParentWindow() && ((*plItr)->getParentWindow()->getDoneFlag())) {
				if (*plItr) {
					MouseEvent mouseEvent;
					mouseEvent.globalX = e.globalX;
					mouseEvent.globalY = e.globalY;
					mouseEvent.localX = e.localX - (*plItr)->getPos().x;
					mouseEvent.localY = e.localY - (*plItr)->getPos().y;
					mouseEvent.altKey = e.altKey;
					mouseEvent.shiftKey = e.shiftKey;
					mouseEvent.ctrlKey = e.ctrlKey;

					(*plItr)->onMButtonUp(mouseEvent);
					//setDoneFlag(0);
					//return;
				}
			}
		}
		virtual void onMouseWheel(const MouseEvent& e){
			if (!listenmouse) {
				return;//!!!!!!!!!!!!!!!!
			}

			int xButtonX1 = size.x - 1 - 2 - 9;
			int xButtonY1 = 1 + 2;
			int xButtonX2 = xButtonX1 + 9;
			int xButtonY2 = xButtonY1 + 9;
			refreshPosAndSize();

			//マウスキャプチャ
			//if (!mouseCaptureFlagL && !mouseCaptureFlagR) SetCapture(hWnd);
			//if (lButton) mouseCaptureFlagL = true;
			//else		  mouseCaptureFlagR = true;

			//内部パーツ
			//setDoneFlag(0);
			std::list<OrgWindowParts*>::iterator plItr;
			for (plItr = partsList.begin(); plItr != partsList.end(); plItr++){
				//if (*plItr && (*plItr)->getParentWindow() && !((*plItr)->getParentWindow()->getDoneFlag())) {
				if (*plItr) {

					WindowPos chkpos = (*plItr)->getPos();
					WindowSize chksize = (*plItr)->getSize();
					//############################################################
					//2023/10/03 マウス位置が子供ウインドウ内部にある場合だけ処理
					//############################################################
					if ((e.localX >= chkpos.x) && (e.localX <= (chkpos.x + chksize.x)) &&
						(e.localY >= chkpos.y) && (e.localY <= (chkpos.y + chksize.y))) {

						//WindowSize partsSize = (*plItr)->getSize();
						//int tmpPosX = e.localX - (*plItr)->getPos().x;
						//int tmpPosY = e.localY - (*plItr)->getPos().y;
						////if (0 <= tmpPosX && tmpPosX<partsSize.x &&
						////	0 <= tmpPosY && tmpPosY<partsSize.y){

						MouseEvent mouseEvent;
						mouseEvent.globalX = e.globalX;
						mouseEvent.globalY = e.globalY;
						mouseEvent.localX = e.localX - chkpos.x;//!!!!
						mouseEvent.localY = e.localY - chkpos.y;//!!!!
						mouseEvent.altKey = e.altKey;
						mouseEvent.shiftKey = e.shiftKey;
						mouseEvent.ctrlKey = e.ctrlKey;
						mouseEvent.wheeldelta = e.wheeldelta;

						(*plItr)->onMouseWheel(mouseEvent);
						//setDoneFlag(1);
						//return;
					}
				}
			}


		}
		//	Method : マウス移動イベント受信
		virtual void onMouseMove(const MouseEvent& e){
			if (!listenmouse) {
				return;//!!!!!!!!!!!!!!!!
			}

			//右下の隅
			if( canChangeSize && 
				((size.x - 4) <= e.localX) && ((size.y - 4) <= e.localY) ){
				SetCursor((HCURSOR)LoadImage(NULL, IDC_SIZENWSE,IMAGE_CURSOR,
											 NULL, NULL,LR_DEFAULTCOLOR | LR_SHARED));
			}

			//内部パーツ
			std::list<OrgWindowParts*>::iterator plItr;
			for (plItr = partsList.begin(); plItr != partsList.end(); plItr++) {
				//if (*plItr) {
				if (*plItr && (*plItr)->getDoneFlag()) {
					MouseEvent mouseEvent;
					mouseEvent.globalX = e.globalX;
					mouseEvent.globalY = e.globalY;
					mouseEvent.localX = e.localX - (*plItr)->getPos().x;
					mouseEvent.localY = e.localY - (*plItr)->getPos().y;
					mouseEvent.altKey = e.altKey;
					mouseEvent.shiftKey = e.shiftKey;
					mouseEvent.ctrlKey = e.ctrlKey;

					(*plItr)->onMouseMove(mouseEvent);
				}
			}
		}

		virtual void onMouseHover(const MouseEvent& e) {
			if (!listenmouse) {
				return;//!!!!!!!!!!!!!!!!
			}

			if (this->hoverListener != NULL) {
				(this->hoverListener)();
			}

			//内部パーツ
			std::list<OrgWindowParts*>::iterator plItr;
			for ( plItr = partsList.begin(); plItr != partsList.end(); plItr++) {
				if (*plItr) {

					MouseEvent mouseEvent;
					mouseEvent.globalX = e.globalX;
					mouseEvent.globalY = e.globalY;
					mouseEvent.localX = e.localX - (*plItr)->getPos().x;
					mouseEvent.localY = e.localY - (*plItr)->getPos().y;
					mouseEvent.altKey = e.altKey;
					mouseEvent.shiftKey = e.shiftKey;
					mouseEvent.ctrlKey = e.ctrlKey;

					(*plItr)->onMouseHover(mouseEvent);
				}
			}
		}
		virtual void onMouseLeave(const MouseEvent& e) {
			if (!listenmouse) {
				return;//!!!!!!!!!!!!!!!!
			}

			if (this->leaveListener != NULL) {
				(this->leaveListener)();
			}

			//内部パーツ
			std::list<OrgWindowParts*>::iterator plItr;
			for (plItr = partsList.begin(); plItr != partsList.end(); plItr++) {
				if (*plItr) {
					MouseEvent mouseEvent;
					mouseEvent.globalX = e.globalX;
					mouseEvent.globalY = e.globalY;
					mouseEvent.localX = e.localX - (*plItr)->getPos().x;
					mouseEvent.localY = e.localY - (*plItr)->getPos().y;
					mouseEvent.altKey = e.altKey;
					mouseEvent.shiftKey = e.shiftKey;
					mouseEvent.ctrlKey = e.ctrlKey;

					(*plItr)->onMouseLeave(mouseEvent);
				}
			}
		}

		//	Method : キーボードイベント受信
		virtual void onKeyboard(const KeyboardEvent& e){
			if( this->keyboardListener!=NULL ){
				(this->keyboardListener)(e);
			}
		}
		//	Method : 描画準備
		void beginPaint(){
			hdcM.beginPaint();
		}
		//	Method : 描画終了
		void endPaint(){
			hdcM.endPaint();
		}
		//	Method : タイトルバーを描画
		void paintTitleBar(){
			if( hdcM.hDC==NULL ){
				_ASSERT_EXPR( 0, L"HDC = NULL" );
			}else{

				//枠を描画
				hdcM.setPenAndBrush(RGB(baseR,baseG,baseB),NULL);
				Rectangle(hdcM.hDC,0,0,size.x,size.y);					//ウィンドウ枠1
				hdcM.setPenAndBrush(RGB(240,240,240),RGB(baseR,baseG,baseB));
				Rectangle(hdcM.hDC,1,1,size.x-1,size.y-1);				//ウィンドウ枠2
				hdcM.setPenAndBrush(RGB(240,240,240),NULL);
				Rectangle(hdcM.hDC,1,1,size.x-1,1+13);				//タイトル枠

				//右下の隅のサイズ変更マークを描画
				if( canChangeSize ){
					MoveToEx(hdcM.hDC, size.x-3, size.y-1, NULL);
					LineTo(hdcM.hDC,   size.x-1, size.y-3);
					LineTo(hdcM.hDC,   size.x-1, size.y-1);
					LineTo(hdcM.hDC,   size.x-3, size.y-1);
					hdcM.setPenAndBrush(RGB(baseR+20,baseG+20,baseB+20),RGB(baseR+20,baseG+20,baseB+20));
					Rectangle(hdcM.hDC, size.x-2,size.y-2,size.x,size.y);
				}

				//タイトルボタンを描画
				hdcM.setPenAndBrush(RGB(240,240,240),NULL);
				int xButtonX1=size.x-1-2-9;
				int xButtonY1=1+2;
				int xButtonX2=xButtonX1+9;
				int xButtonY2=xButtonY1+9;
				Rectangle(hdcM.hDC,xButtonX1,xButtonY1,xButtonX2,xButtonY2);	//X枠
				MoveToEx(hdcM.hDC, xButtonX1+2, xButtonY1+2, NULL);			//X印
				LineTo(hdcM.hDC, xButtonX2-2, xButtonY2-2);
				MoveToEx(hdcM.hDC, xButtonX2-3, xButtonY1+2, NULL);
				LineTo(hdcM.hDC, xButtonX1+1, xButtonY2-2);

				//タイトル名を描画
				hdcM.setFont(12,_T("ＭＳ ゴシック"));
				SetTextColor(hdcM.hDC,RGB(240,240,240));
				TextOut(hdcM.hDC, 10, 2, title, (int)_tcslen(title));			//タイトル名
			}
		}
	};

	////////////////----------------------------------------////////////////
	////////////////										////////////////
	////////////////		ウィンドウ内部品クラス			////////////////
	////////////////										////////////////
	////////////////----------------------------------------////////////////

	class OWP_ScrollWnd;

	///<summary>
	///	ウィンドウ内部品"セパレータ"クラス
	///</summary>
	class OWP_Separator : public OrgWindowParts{
	public:
		//////////////////// Constructor/Destructor //////////////////////
		OWP_Separator(OrgWindow* _parentWindow, bool _only1line, double _centerRate = 0.5, bool _divideSide = true, OWP_ScrollWnd* _parentScrollWnd = 0) : OrgWindowParts() {
			parentWindow = _parentWindow;
			currentPartsSizeY1= 0;
			currentPartsSizeY2= 0;

			canShift= true;
			//centerRate= 0.5;
			centerRate = _centerRate;
			divideSide= _divideSide;
			shiftDrag= false;

			partsList1.clear();
			partsList2.clear();

			only1line = _only1line;
			parentScrollWnd = _parentScrollWnd;//2023/09/19

			setIsSeparator(true);
		}
		~OWP_Separator(){
		}

		//////////////////////////// Method //////////////////////////////
		///	Method : 親ウィンドウに登録
		//void regist( OrgWindow *_parentWindow,
		//					 WindowPos _pos, WindowSize _size,
		//					 HDCMaster* _hdcM,
		//					 unsigned char _baseR=50, unsigned char _baseG=70, unsigned char _baseB=70 ){
		//	_regist(_parentWindow,_pos,_size,_hdcM,_baseR,_baseG,_baseB);


		//	//addParts1, addParts2で個別にregistが呼ばれるのでコメントアウト。
		//	////全てのグループ内部品を同じウィンドウに登録
		//	//for(std::list<OrgWindowParts*>::iterator itr=partsList1.begin();
		//	//	itr!=partsList1.end(); itr++){
		//	//	(*itr)->regist( parentWindow,
		//	//					_pos,_size,
		//	//					hdcM,
		//	//					baseColor.r,baseColor.g,baseColor.b);
		//	//}
		//	//for(std::list<OrgWindowParts*>::iterator itr=partsList2.begin();
		//	//	itr!=partsList2.end(); itr++){
		//	//	(*itr)->regist( parentWindow,
		//	//					_pos,_size,
		//	//					hdcM,
		//	//					baseColor.r,baseColor.g,baseColor.b);
		//	//}

		//	//グループボックスと内部要素の位置とサイズを自動設定
		//	autoResize();
		//}
		///	Method : グループ内部品を追加
		void addParts1(OrgWindowParts& a){
			partsList1.push_back(&a);

			// グループボックスがウィンドウに登録されている場合は
			// グループ内部品も同じウィンドウに登録する
			if( parentWindow!=NULL ){
				a.registmember( parentWindow,
						  pos, size,
						  hdcM,
						  baseColor.r,baseColor.g,baseColor.b);

				//ウィンドウ内の全パーツの位置・サイズを自動調整
				parentWindow->autoResizeAllParts();
			}
		}
		void addParts2(OrgWindowParts& a){
			partsList2.push_back(&a);

			// グループボックスがウィンドウに登録されている場合は
			// グループ内部品も同じウィンドウに登録する
			if( parentWindow!=NULL ){
				a.registmember( parentWindow,
						  pos, size,
						  hdcM,
						  baseColor.r,baseColor.g,baseColor.b);

				//ウィンドウ内の全パーツの位置・サイズを自動調整
				parentWindow->autoResizeAllParts();
			}
		}


		/// Method : 自動サイズ設定
		virtual void autoResize(){
			//パーツエリアの位置とサイズを設定
			int onelineheight = 16;
			int centerPos= getCenterLinePos();


			//only1line == trueの際にはセパレータ自体の最小サイズを決める
			if (only1line == true) {
				int sizey1 = 0;
				int sizey2 = 0;
				std::list<OrgWindowParts*>::iterator itr;
				for ( itr = partsList1.begin(); itr != partsList1.end(); itr++) {
					if (*itr) {
						(*itr)->autoResize();
						WindowSize befsize = (*itr)->getSize();
						sizey1 += (*itr)->getSize().y;
					}
				}
				std::list<OrgWindowParts*>::iterator itr2;
				for (itr2 = partsList2.begin(); itr2 != partsList2.end(); itr2++) {
					if (*itr2) {
						(*itr2)->autoResize();
						WindowSize befsize = (*itr2)->getSize();
						sizey2 += (*itr2)->getSize().y;
					}
				}
				size.y = max(sizey1, sizey2);
				if (size.y == 0) {
					size.y = 15;
				}
			}


			partsAreaPos1=  pos;
			int scrollbarwidth = 20;
			if( divideSide ){
				partsAreaPos2=  pos+ WindowPos(centerPos+1+LINE_MARGIN, 0);
				partsAreaSize1= WindowSize( centerPos-LINE_MARGIN, size.y);
				partsAreaSize2= WindowSize( size.x-centerPos-LINE_MARGIN-1, size.y);
			}else{
				partsAreaPos2=  pos+ WindowPos(0, centerPos+1+LINE_MARGIN);
				partsAreaSize1= WindowSize( size.x, centerPos-LINE_MARGIN);
				partsAreaSize2= WindowSize( size.x, size.y-centerPos-LINE_MARGIN-1);
			}
			currentPartsSizeY1= 0;
			currentPartsSizeY2= 0;

			//全ての内部パーツの位置とサイズを自動設定
			std::list<OrgWindowParts*>::iterator itr;
			for (itr = partsList1.begin(); itr != partsList1.end(); itr++) {
				if (*itr) {
					(*itr)->autoResize();//!!!!!!!!!!!!
					WindowSize befsize = (*itr)->getSize();
					(*itr)->setPos(WindowPos(partsAreaPos1.x, partsAreaPos1.y + currentPartsSizeY1));
					//(*itr)->setSize( WindowSize( partsAreaSize1.x, partsAreaSize1.y-currentPartsSizeY1 ) );
					(*itr)->setSize(WindowSize(partsAreaSize1.x, befsize.y));
					//(*itr)->autoResize();//befsizeよりも前に移動

					//currentPartsSizeY1+= (*itr)->getSize().y+1;
					currentPartsSizeY1 += (*itr)->getSize().y;
				}
			}
			std::list<OrgWindowParts*>::iterator itr2;
			for (itr2 = partsList2.begin(); itr2 != partsList2.end(); itr2++) {
				if (*itr2) {
					WindowSize befsize = (*itr2)->getSize();
					(*itr2)->setPos(WindowPos(partsAreaPos2.x, partsAreaPos2.y + currentPartsSizeY2));
					//(*itr2)->setSize( WindowSize( partsAreaSize2.x, partsAreaSize2.y-currentPartsSizeY2 ) );
					(*itr2)->setSize(WindowSize(partsAreaSize2.x, befsize.y));
					//(*itr2)->autoResize();//befsizeよりも前に移動

					//currentPartsSizeY2+= (*itr2)->getSize().y+1;
					currentPartsSizeY2 += (*itr2)->getSize().y;
				}
			}
		}
		///	Method : 描画
		virtual void draw();
		//void draw(){
		//	//枠を書く
		//	int centerPos= getCenterLinePos();
		//	hdcM->setPenAndBrush(RGB(min(baseColor.r+20,255),min(baseColor.g+20,255),min(baseColor.b+20,255)),NULL);
		//	if( divideSide ){
		//		MoveToEx(hdcM->hDC, pos.x+centerPos,pos.y+1, NULL);
		//		LineTo(hdcM->hDC,   pos.x+centerPos,pos.y+size.y-1);
		//		if( canShift ){
		//			hdcM->setPenAndBrush(RGB(240,240,240),NULL);
		//			int centerPos2= pos.y+size.y/2;
		//			MoveToEx(hdcM->hDC, pos.x+centerPos,centerPos2-HANDLE_MARK_SIZE/2, NULL);
		//			LineTo(hdcM->hDC,   pos.x+centerPos,centerPos2+HANDLE_MARK_SIZE/2);
		//		}
		//	}else{
		//		MoveToEx(hdcM->hDC, pos.x+1,pos.y+centerPos, NULL);
		//		LineTo(hdcM->hDC,   pos.x+size.x-1,pos.y+centerPos);
		//		if( canShift ){
		//			hdcM->setPenAndBrush(RGB(240,240,240),NULL);
		//			int centerPos2= pos.x+size.x/2;
		//			MoveToEx(hdcM->hDC, centerPos2-HANDLE_MARK_SIZE/2,pos.y+centerPos, NULL);
		//			LineTo(hdcM->hDC,   centerPos2+HANDLE_MARK_SIZE/2,pos.y+centerPos);
		//		}
		//	}

		//	//全ての内部パーツを描画
		//	for(std::list<OrgWindowParts*>::iterator itr=partsList1.begin();
		//		itr!=partsList1.end(); itr++){
		//			(*itr)->draw();
		//	}
		//	for(std::list<OrgWindowParts*>::iterator itr=partsList2.begin();
		//		itr!=partsList2.end(); itr++){
		//			(*itr)->draw();
		//	}
		//}
		///	Method : 左右マウスボタンダウンイベント受信
		virtual void onLButtonDown(const MouseEvent& e){
			onLRButtonDown(e,true);
		}
		//	Method : 左マウスボタン ダブルクリックイベント受信
		virtual void onLButtonDBLCLK(const MouseEvent& e) {//2023/10/04
			//内部パーツ
			std::list<OrgWindowParts*>::iterator plItr;
			for (plItr = partsList1.begin(); plItr != partsList1.end(); plItr++) {
				if (*plItr) {
					WindowPos chkpos = (*plItr)->getPos();
					WindowSize chksize = (*plItr)->getSize();
					//############################################################
					//2023/10/04 マウス位置が子供ウインドウ内部にある場合だけ処理
					//############################################################
					if ((e.localX >= chkpos.x) && (e.localX <= (chkpos.x + chksize.x)) &&
						(e.localY >= chkpos.y) && (e.localY <= (chkpos.y + chksize.y))) {

						MouseEvent mouseEvent;
						mouseEvent.globalX = e.globalX;
						mouseEvent.globalY = e.globalY;
						mouseEvent.localX = e.localX + pos.x - (*plItr)->getPos().x;
						mouseEvent.localY = e.localY + pos.y - (*plItr)->getPos().y;
						mouseEvent.altKey = e.altKey;
						mouseEvent.shiftKey = e.shiftKey;
						mouseEvent.ctrlKey = e.ctrlKey;
						mouseEvent.wheeldelta = e.wheeldelta;

						(*plItr)->onLButtonDBLCLK(mouseEvent);
					}
				}
			}
			std::list<OrgWindowParts*>::iterator plItr2;
			for (plItr2 = partsList2.begin(); plItr2 != partsList2.end(); plItr2++) {
				if (*plItr2) {
					WindowPos chkpos = (*plItr2)->getPos();
					WindowSize chksize = (*plItr2)->getSize();
					//############################################################
					//2023/10/04 マウス位置が子供ウインドウ内部にある場合だけ処理
					//############################################################
					if ((e.localX >= chkpos.x) && (e.localX <= (chkpos.x + chksize.x)) &&
						(e.localY >= chkpos.y) && (e.localY <= (chkpos.y + chksize.y))) {
						MouseEvent mouseEvent;
						mouseEvent.globalX = e.globalX;
						mouseEvent.globalY = e.globalY;
						mouseEvent.localX = e.localX + pos.x - (*plItr2)->getPos().x;
						mouseEvent.localY = e.localY + pos.y - (*plItr2)->getPos().y;
						mouseEvent.altKey = e.altKey;
						mouseEvent.shiftKey = e.shiftKey;
						mouseEvent.ctrlKey = e.ctrlKey;
						mouseEvent.wheeldelta = e.wheeldelta;

						(*plItr2)->onLButtonDBLCLK(mouseEvent);
					}
				}
			}
		}
		//	Method : 右マウスボタン ダブルクリックイベント受信
		virtual void onRButtonDBLCLK(const MouseEvent& e) {//2023/10/04
			//内部パーツ
			std::list<OrgWindowParts*>::iterator plItr;
			for (plItr = partsList1.begin(); plItr != partsList1.end(); plItr++) {
				if (*plItr) {
					WindowPos chkpos = (*plItr)->getPos();
					WindowSize chksize = (*plItr)->getSize();
					//############################################################
					//2023/10/04 マウス位置が子供ウインドウ内部にある場合だけ処理
					//############################################################
					if ((e.localX >= chkpos.x) && (e.localX <= (chkpos.x + chksize.x)) &&
						(e.localY >= chkpos.y) && (e.localY <= (chkpos.y + chksize.y))) {

						MouseEvent mouseEvent;
						mouseEvent.globalX = e.globalX;
						mouseEvent.globalY = e.globalY;
						mouseEvent.localX = e.localX + pos.x - (*plItr)->getPos().x;
						mouseEvent.localY = e.localY + pos.y - (*plItr)->getPos().y;
						mouseEvent.altKey = e.altKey;
						mouseEvent.shiftKey = e.shiftKey;
						mouseEvent.ctrlKey = e.ctrlKey;
						mouseEvent.wheeldelta = e.wheeldelta;

						(*plItr)->onRButtonDBLCLK(mouseEvent);
					}
				}
			}
			std::list<OrgWindowParts*>::iterator plItr2;
			for (plItr2 = partsList2.begin(); plItr2 != partsList2.end(); plItr2++) {
				if (*plItr2) {
					WindowPos chkpos = (*plItr2)->getPos();
					WindowSize chksize = (*plItr2)->getSize();
					//############################################################
					//2023/10/04 マウス位置が子供ウインドウ内部にある場合だけ処理
					//############################################################
					if ((e.localX >= chkpos.x) && (e.localX <= (chkpos.x + chksize.x)) &&
						(e.localY >= chkpos.y) && (e.localY <= (chkpos.y + chksize.y))) {
						MouseEvent mouseEvent;
						mouseEvent.globalX = e.globalX;
						mouseEvent.globalY = e.globalY;
						mouseEvent.localX = e.localX + pos.x - (*plItr2)->getPos().x;
						mouseEvent.localY = e.localY + pos.y - (*plItr2)->getPos().y;
						mouseEvent.altKey = e.altKey;
						mouseEvent.shiftKey = e.shiftKey;
						mouseEvent.ctrlKey = e.ctrlKey;
						mouseEvent.wheeldelta = e.wheeldelta;

						(*plItr2)->onRButtonDBLCLK(mouseEvent);
					}
				}
			}
		}
		virtual void onRButtonDown(const MouseEvent& e){
			onLRButtonDown(e,false);
		}
		///	Method : 左右マウスボタンアップイベント受信
		virtual void onLButtonUp(const MouseEvent& e){
			onLRButtonUp(e,true);
		}
		virtual void onRButtonUp(const MouseEvent& e){
			onLRButtonUp(e,false);
		}
		///	Method : マウス移動イベント受信
		virtual void onMouseMove(const MouseEvent& e){

			//カーソル変更
			if( canShift && isMouseOnHandle(e) ){
				if( divideSide ){
					SetCursor((HCURSOR)LoadImage(NULL, IDC_SIZEWE,IMAGE_CURSOR,
												 NULL, NULL,LR_DEFAULTCOLOR | LR_SHARED));
				}else{
					SetCursor((HCURSOR)LoadImage(NULL, IDC_SIZENS,IMAGE_CURSOR,
												 NULL, NULL,LR_DEFAULTCOLOR | LR_SHARED));
				}
			}

			//ドラッグによる移動
			if( shiftDrag ){
				shiftLine(e);
			}

			//内部パーツ
			std::list<OrgWindowParts*>::iterator plItr;
			for (plItr = partsList1.begin(); plItr != partsList1.end(); plItr++) {
				if (*plItr) {
					MouseEvent mouseEvent;
					mouseEvent.globalX = e.globalX;
					mouseEvent.globalY = e.globalY;
					mouseEvent.localX = e.localX + pos.x - (*plItr)->getPos().x;
					mouseEvent.localY = e.localY + pos.y - (*plItr)->getPos().y;
					mouseEvent.altKey = e.altKey;
					mouseEvent.shiftKey = e.shiftKey;
					mouseEvent.ctrlKey = e.ctrlKey;

					(*plItr)->onMouseMove(mouseEvent);
				}
			}
			std::list<OrgWindowParts*>::iterator plItr2;
			for (plItr2 = partsList2.begin(); plItr2 != partsList2.end(); plItr2++) {
				if (*plItr2) {
					MouseEvent mouseEvent;
					mouseEvent.globalX = e.globalX;
					mouseEvent.globalY = e.globalY;
					mouseEvent.localX = e.localX + pos.x - (*plItr2)->getPos().x;
					mouseEvent.localY = e.localY + pos.y - (*plItr2)->getPos().y;
					mouseEvent.altKey = e.altKey;
					mouseEvent.shiftKey = e.shiftKey;
					mouseEvent.ctrlKey = e.ctrlKey;

					(*plItr2)->onMouseMove(mouseEvent);
				}
			}
		}
		virtual void onMouseWheel(const MouseEvent& e) {
			//内部パーツ
			std::list<OrgWindowParts*>::iterator plItr;
			for (plItr = partsList1.begin(); plItr != partsList1.end(); plItr++) {
				if (*plItr) {
					MouseEvent mouseEvent;
					mouseEvent.globalX = e.globalX;
					mouseEvent.globalY = e.globalY;
					mouseEvent.localX = e.localX + pos.x - (*plItr)->getPos().x;
					mouseEvent.localY = e.localY + pos.y - (*plItr)->getPos().y;
					mouseEvent.altKey = e.altKey;
					mouseEvent.shiftKey = e.shiftKey;
					mouseEvent.ctrlKey = e.ctrlKey;
					mouseEvent.wheeldelta = e.wheeldelta;

					(*plItr)->onMouseWheel(mouseEvent);
				}
			}
			std::list<OrgWindowParts*>::iterator plItr2;
			for (plItr2 = partsList2.begin(); plItr2 != partsList2.end(); plItr2++) {
				if (*plItr2) {
					MouseEvent mouseEvent;
					mouseEvent.globalX = e.globalX;
					mouseEvent.globalY = e.globalY;
					mouseEvent.localX = e.localX + pos.x - (*plItr2)->getPos().x;
					mouseEvent.localY = e.localY + pos.y - (*plItr2)->getPos().y;
					mouseEvent.altKey = e.altKey;
					mouseEvent.shiftKey = e.shiftKey;
					mouseEvent.ctrlKey = e.ctrlKey;
					mouseEvent.wheeldelta = e.wheeldelta;

					(*plItr2)->onMouseWheel(mouseEvent);
				}
			}
		}

		virtual void onMButtonDown(const MouseEvent& e) {
			//内部パーツ
			std::list<OrgWindowParts*>::iterator plItr;
			for (plItr = partsList1.begin(); plItr != partsList1.end(); plItr++) {
				if (*plItr) {
					MouseEvent mouseEvent;
					mouseEvent.globalX = e.globalX;
					mouseEvent.globalY = e.globalY;
					mouseEvent.localX = e.localX + pos.x - (*plItr)->getPos().x;
					mouseEvent.localY = e.localY + pos.y - (*plItr)->getPos().y;
					mouseEvent.altKey = e.altKey;
					mouseEvent.shiftKey = e.shiftKey;
					mouseEvent.ctrlKey = e.ctrlKey;
					mouseEvent.wheeldelta = e.wheeldelta;

					(*plItr)->onMButtonDown(mouseEvent);
				}
			}
			std::list<OrgWindowParts*>::iterator plItr2;
			for (plItr2 = partsList2.begin(); plItr2 != partsList2.end(); plItr2++) {
				if (*plItr2) {
					MouseEvent mouseEvent;
					mouseEvent.globalX = e.globalX;
					mouseEvent.globalY = e.globalY;
					mouseEvent.localX = e.localX + pos.x - (*plItr2)->getPos().x;
					mouseEvent.localY = e.localY + pos.y - (*plItr2)->getPos().y;
					mouseEvent.altKey = e.altKey;
					mouseEvent.shiftKey = e.shiftKey;
					mouseEvent.ctrlKey = e.ctrlKey;
					mouseEvent.wheeldelta = e.wheeldelta;

					(*plItr2)->onMButtonDown(mouseEvent);
				}
			}

		}
		virtual void onMButtonUp(const MouseEvent& e) {
			//内部パーツ
			std::list<OrgWindowParts*>::iterator plItr;
			for (plItr = partsList1.begin(); plItr != partsList1.end(); plItr++) {
				if (*plItr) {
					MouseEvent mouseEvent;
					mouseEvent.globalX = e.globalX;
					mouseEvent.globalY = e.globalY;
					mouseEvent.localX = e.localX + pos.x - (*plItr)->getPos().x;
					mouseEvent.localY = e.localY + pos.y - (*plItr)->getPos().y;
					mouseEvent.altKey = e.altKey;
					mouseEvent.shiftKey = e.shiftKey;
					mouseEvent.ctrlKey = e.ctrlKey;
					mouseEvent.wheeldelta = e.wheeldelta;

					(*plItr)->onMButtonUp(mouseEvent);
				}
			}
			std::list<OrgWindowParts*>::iterator plItr2;
			for ( plItr2 = partsList2.begin(); plItr2 != partsList2.end(); plItr2++) {
				if (*plItr2) {
					MouseEvent mouseEvent;
					mouseEvent.globalX = e.globalX;
					mouseEvent.globalY = e.globalY;
					mouseEvent.localX = e.localX + pos.x - (*plItr2)->getPos().x;
					mouseEvent.localY = e.localY + pos.y - (*plItr2)->getPos().y;
					mouseEvent.altKey = e.altKey;
					mouseEvent.shiftKey = e.shiftKey;
					mouseEvent.ctrlKey = e.ctrlKey;
					mouseEvent.wheeldelta = e.wheeldelta;

					(*plItr2)->onMButtonUp(mouseEvent);
				}
			}

		}



		/////////////////////////// Accessor /////////////////////////////
		/// Accessor : canShift
		bool getCanShiftType() const{
			return canShift;
		}
		void setCanShiftType(bool value){
			canShift= value;
			callRewrite();
		}
		/// Accessor : divideSide
		bool getDivideSide() const{
			return divideSide;
		}
		void setDivideSide(bool value){
			if( divideSide!=value ){
				divideSide= value;

				if( parentWindow!=NULL ){
					parentWindow->autoResizeAllParts();
				}
			}
		}
		/// Accessor : centerRate
		double getCenterRate() const{
			return centerRate;
		}
		void setCenterRate(const double value){
			centerRate= max(0.0, min(value,1.0));
			if( parentWindow!=NULL ){
				parentWindow->autoResizeAllParts();
			}
		}

		OWP_ScrollWnd* getParentScrollWnd()//2023/09/19
		{
			return parentScrollWnd;
		}

	private:
		////////////////////////// MemberVar /////////////////////////////
		int currentPartsSizeY1;
		int currentPartsSizeY2;
		WindowPos  partsAreaPos1,partsAreaPos2;
		WindowSize partsAreaSize1,partsAreaSize2;
		std::list<OrgWindowParts*> partsList1,partsList2;

		bool canShift;
		double centerRate;
		bool divideSide;
		bool shiftDrag;
			
		bool only1line;
		OWP_ScrollWnd* parentScrollWnd;//2023/09/19

		static const int LINE_MARGIN= 1;
		static const int HANDLE_SIZE= 12;
		static const int HANDLE_MARK_SIZE= 6;

		/// Method : 仕切り線の位置を取得
		int getCenterLinePos() const{
			int divval = divideSide ? size.x : size.y;
			int divpos = divideSide ? pos.x : pos.y;

			int centerPos= (int)(centerRate*(double)divval);
			centerPos= max(1, min(centerPos, divval-2));
			return centerPos;
		}
		/// Method : 仕切り線に触れているかどうか取得
		bool isMouseOnHandle(const MouseEvent& e) const{
			int centerPos= getCenterLinePos();
			if( ((centerPos - LINE_MARGIN) <= (divideSide ? e.localX : e.localY))
			 && ((divideSide ? e.localX : e.localY) <= (centerPos + LINE_MARGIN)) ){

				int centerPos2= (divideSide?size.y:size.x)/2;
				if( ((centerPos2 - HANDLE_SIZE / 2) <= (divideSide ? e.localY : e.localX))
				 && ((divideSide ? e.localY : e.localX) <= (centerPos2 + HANDLE_SIZE / 2)) ){
					return true;
				}
			}
			return false;
		}
		/// Method : マウス位置に仕切り線を移動
		void shiftLine(const MouseEvent& e){
			if( divideSide ){
				setCenterRate( (double)e.localX/(double)size.x );
			}else{
				setCenterRate( (double)e.localY/(double)size.y );
			}
		}
		///	Method : 左右マウスボタンダウンイベント受信
		void onLRButtonDown(const MouseEvent& e, bool lButton){

			//仕切り線ドラッグで移動
			if( isMouseOnHandle(e) ){

				if( canShift && lButton ){
					shiftDrag= true;
					//カーソル変更
					if( divideSide ){
						SetCursor((HCURSOR)LoadImage(NULL, IDC_SIZEWE,IMAGE_CURSOR,
													 NULL, NULL,LR_DEFAULTCOLOR | LR_SHARED));
					}else{
						SetCursor((HCURSOR)LoadImage(NULL, IDC_SIZENS,IMAGE_CURSOR,
													 NULL, NULL,LR_DEFAULTCOLOR | LR_SHARED));
					}
				}

			//内部パーツ1
			}else if( (divideSide ? e.localX : e.localY) <= getCenterLinePos() ){

				std::list<OrgWindowParts*>::iterator plItr;
				for(plItr = partsList1.begin(); plItr != partsList1.end(); plItr++ ){
					if (*plItr) {
						WindowSize partsSize = (*plItr)->getSize();
						int tmpPosX = e.localX + pos.x - (*plItr)->getPos().x;
						int tmpPosY = e.localY + pos.y - (*plItr)->getPos().y;
						if ((0 <= tmpPosX) && (tmpPosX < partsSize.x) &&
							(0 <= tmpPosY) && (tmpPosY < partsSize.y)) {

							MouseEvent mouseEvent;
							mouseEvent.globalX = e.globalX;
							mouseEvent.globalY = e.globalY;
							mouseEvent.localX = tmpPosX;
							mouseEvent.localY = tmpPosY;
							mouseEvent.altKey = e.altKey;
							mouseEvent.shiftKey = e.shiftKey;
							mouseEvent.ctrlKey = e.ctrlKey;

							if (lButton) {
								(*plItr)->onLButtonDown(mouseEvent);
							}
							else {
								(*plItr)->onRButtonDown(mouseEvent);
							}
							return;
						}
					}
				}

			//内部パーツ2
			}else{

				std::list<OrgWindowParts*>::iterator plItr2;
				for (plItr2 = partsList2.begin(); plItr2 != partsList2.end(); plItr2++) {
					if (*plItr2) {
						WindowSize partsSize = (*plItr2)->getSize();
						int tmpPosX = e.localX + pos.x - (*plItr2)->getPos().x;
						int tmpPosY = e.localY + pos.y - (*plItr2)->getPos().y;
						if ((0 <= tmpPosX) && (tmpPosX < partsSize.x) &&
							(0 <= tmpPosY) && (tmpPosY < partsSize.y)) {

							MouseEvent mouseEvent;
							mouseEvent.globalX = e.globalX;
							mouseEvent.globalY = e.globalY;
							mouseEvent.localX = tmpPosX;
							mouseEvent.localY = tmpPosY;
							mouseEvent.altKey = e.altKey;
							mouseEvent.shiftKey = e.shiftKey;
							mouseEvent.ctrlKey = e.ctrlKey;

							if (lButton) {
								(*plItr2)->onLButtonDown(mouseEvent);
							}
							else {
								(*plItr2)->onRButtonDown(mouseEvent);
							}
							return;
						}
					}
				}

			}

		}
		///	Method : 左右マウスボタンアップイベント受信
		void onLRButtonUp(const MouseEvent& e, bool lButton){

			//ドラッグ移動解除
			if( shiftDrag ){
				shiftDrag= false;
				shiftLine(e);
			}

			//内部パーツ
			std::list<OrgWindowParts*>::iterator plItr;
			for (plItr = partsList1.begin(); plItr != partsList1.end(); plItr++) {

				if (*plItr) {
					MouseEvent mouseEvent;
					mouseEvent.globalX = e.globalX;
					mouseEvent.globalY = e.globalY;
					mouseEvent.localX = e.localX + pos.x - (*plItr)->getPos().x;
					mouseEvent.localY = e.localY + pos.y - (*plItr)->getPos().y;
					mouseEvent.altKey = e.altKey;
					mouseEvent.shiftKey = e.shiftKey;
					mouseEvent.ctrlKey = e.ctrlKey;

					if (lButton) {
						(*plItr)->onLButtonUp(mouseEvent);
					}
					else {
						(*plItr)->onRButtonUp(mouseEvent);
					}
				}
			}
			std::list<OrgWindowParts*>::iterator plItr2;
			for (plItr2 = partsList2.begin(); plItr2 != partsList2.end(); plItr2++) {
				if (*plItr2) {
					MouseEvent mouseEvent;
					mouseEvent.globalX = e.globalX;
					mouseEvent.globalY = e.globalY;
					mouseEvent.localX = e.localX + pos.x - (*plItr2)->getPos().x;
					mouseEvent.localY = e.localY + pos.y - (*plItr2)->getPos().y;
					mouseEvent.altKey = e.altKey;
					mouseEvent.shiftKey = e.shiftKey;
					mouseEvent.ctrlKey = e.ctrlKey;

					if (lButton) {
						(*plItr2)->onLButtonUp(mouseEvent);
					}
					else {
						(*plItr2)->onRButtonUp(mouseEvent);
					}
				}
			}
		}
	};

	///<summary>
	///	ウィンドウ内部品"グループボックス"クラス
	///</summary>
	class OWP_GroupBox : public OrgWindowParts{
	public:
		//////////////////// Constructor/Destructor //////////////////////
		OWP_GroupBox(const TCHAR *_name ) : OrgWindowParts() {
			name = new TCHAR[256];
			if (_name) {
				size_t tclen = _tcslen(_name);
				size_t cplen;
				if (tclen != 0) {
					if (tclen <= 255) {
						cplen = tclen;
					}
					else {
						cplen = 255;
					}
					_tcsncpy_s(name, 256, _name, cplen);
					name[cplen] = (TCHAR)0;
				}
				else {
					_tcscpy_s(name, 256, TEXT("NoName"));
				}
			}

			//openListener = [](){s_dummyfunc();};
			openListener = NULL;

			currentPartsSizeY= 0;

			open= true;
			canClose= true;
		}
		~OWP_GroupBox(){
			delete[] name;
		}

		//////////////////////////// Method //////////////////////////////
		///	Method : 親ウィンドウに登録
		//void regist( OrgWindow *_parentWindow,
		//					 WindowPos _pos, WindowSize _size,
		//					 HDCMaster* _hdcM,
		//					 unsigned char _baseR=50, unsigned char _baseG=70, unsigned char _baseB=70 ){
		//	_regist(_parentWindow,_pos,_size,_hdcM,_baseR,_baseG,_baseB);

		//	////全てのグループ内部品を同じウィンドウに登録
		//	//for(std::list<OrgWindowParts*>::iterator itr=partsList.begin();
		//	//	itr!=partsList.end(); itr++){
		//	//	(*itr)->regist( parentWindow,
		//	//					_pos,_size,
		//	//					hdcM,
		//	//					baseColor.r,baseColor.g,baseColor.b);
		//	//}

		//	//グループボックスと内部要素の位置とサイズを自動設定
		//	autoResize();
		//}
		///	Method : グループ内部品を追加
		void addParts(OrgWindowParts& a){
			partsList.push_back(&a);

			// グループボックスがウィンドウに登録されている場合は
			// グループ内部品も同じウィンドウに登録する
			if( parentWindow!=NULL ){
				a.registmember( parentWindow,
						  pos, size,
						  hdcM,
						  baseColor.r,baseColor.g,baseColor.b);

				// グループボックスがオープン状態の時は
				// ウィンドウ内の全パーツの位置・サイズを自動調整
				if( open ){
					parentWindow->autoResizeAllParts();
				}
			}
		}
		/// Method : 自動サイズ設定
		virtual void autoResize(){
			if( open ){
				//パーツエリアの位置とサイズを設定
				partsAreaPos=  pos + WindowPos( 3, SIZE_CLOSE_Y+2 );
				partsAreaSize= WindowSize( size.x-3-3, size.y-(SIZE_CLOSE_Y+3)-3 );
				currentPartsSizeY= 0;

				//全ての内部パーツの位置とサイズを自動設定
				std::list<OrgWindowParts*>::iterator itr;
				for(itr = partsList.begin(); itr != partsList.end(); itr++){
					if (*itr) {
						//(*itr)->setPos(WindowPos(partsAreaPos.x, partsAreaPos.y + currentPartsSizeY));
						//(*itr)->setSize(WindowSize(partsAreaSize.x, partsAreaSize.y - currentPartsSizeY));
						//(*itr)->autoResize();
						//currentPartsSizeY += (*itr)->getSize().y + 1;

						(*itr)->autoResize();//!!!!!!!!!!!!
						WindowSize befsize = (*itr)->getSize();
						(*itr)->setPos(WindowPos(partsAreaPos.x, partsAreaPos.y + currentPartsSizeY));
						(*itr)->setSize(WindowSize(partsAreaSize.x, befsize.y));
						currentPartsSizeY += (*itr)->getSize().y;
					}
				}

				//グループボックスのサイズを内部要素に合わせてトリミング
				partsAreaSize.y= currentPartsSizeY;
				size.y= partsAreaPos.y-pos.y+partsAreaSize.y+3;

			}else{
				size.y= SIZE_CLOSE_Y;
			}
		}
		///	Method : 描画
		virtual void draw(){
			if (!hdcM) {
				return;
			}

			//枠を書く
			if( open ){
				hdcM->setPenAndBrush(RGB(min(baseColor.r+20,255),min(baseColor.g+20,255),min(baseColor.b+20,255)),RGB(baseColor.r,baseColor.g,baseColor.b));
				Rectangle(hdcM->hDC,pos.x,pos.y+SIZE_CLOSE_Y/2,pos.x+size.x,pos.y+size.y);
			}

			//開閉ボタン
			if( canClose ){
				int pos0x= pos.x+ BOX_POS_X;
				int pos0y= pos.y+ SIZE_CLOSE_Y/2- BOX_WIDTH/2;
				int pos1x= pos.x+ BOX_POS_X+ BOX_WIDTH;
				int pos1y= pos.y+ SIZE_CLOSE_Y/2+ BOX_WIDTH/2;

				//背景
				hdcM->setPenAndBrush(NULL,RGB(baseColor.r,baseColor.g,baseColor.b));
				Rectangle(hdcM->hDC,
					pos0x-1, pos0y,
					pos1x+3, pos1y);

				//枠
				hdcM->setPenAndBrush(RGB(min(baseColor.r+20,255),min(baseColor.g+20,255),min(baseColor.b+20,255)),NULL);
				Rectangle(hdcM->hDC,
					pos0x, pos0y,
					pos1x, pos1y);

				//中身
				hdcM->setPenAndBrush(RGB(240,240,240),NULL);
				if(open){
					MoveToEx(hdcM->hDC, (pos0x+pos1x)/2,pos0y+2, NULL);
					LineTo(hdcM->hDC,   (pos0x+pos1x)/2,pos1y-2);
					MoveToEx(hdcM->hDC, (pos0x+pos1x)/2-1,pos0y+2, NULL);
					LineTo(hdcM->hDC,   (pos0x+pos1x)/2-1,pos1y-2);
				}
				MoveToEx(hdcM->hDC, pos0x+2,(pos0y+pos1y)/2, NULL);
				LineTo(hdcM->hDC,   pos1x-2,(pos0y+pos1y)/2);
				MoveToEx(hdcM->hDC, pos0x+2,(pos0y+pos1y)/2-1, NULL);
				LineTo(hdcM->hDC,   pos1x-2,(pos0y+pos1y)/2-1);
/*				hdcM->setPenAndBrush(RGB(min(baseColor.r+20,255),min(baseColor.g+20,255),min(baseColor.b+20,255)),NULL);
				if(open){
					MoveToEx(hdcM->hDC, (pos0x+pos1x)/2,(pos0y+pos1y)/2-1, NULL);
					LineTo(hdcM->hDC,   (pos0x+pos1x)/2,(pos0y+pos1y)/2+1);
					MoveToEx(hdcM->hDC, (pos0x+pos1x)/2-1,(pos0y+pos1y)/2-1, NULL);
					LineTo(hdcM->hDC,   (pos0x+pos1x)/2-1,(pos0y+pos1y)/2+1);
				}//*/
			}

			{//名前
				int pos1x,pos1y;
				if( canClose ){
					pos1x= pos.x+ BOX_POS_X+ BOX_WIDTH+ NAME_POS_X2;
					pos1y= pos.y+ SIZE_CLOSE_Y/2- 5;
				}else{
					pos1x= pos.x+NAME_POS_X1;
					pos1y= pos.y+SIZE_CLOSE_Y/2-5;
				}
				hdcM->setFont(12,_T("ＭＳ ゴシック"));
				SetTextColor(hdcM->hDC,RGB(240,240,240));
				SetBkColor(hdcM->hDC,RGB(baseColor.r,baseColor.g,baseColor.b));
				SetBkMode(hdcM->hDC,OPAQUE);
				ExtTextOut( hdcM->hDC,
							pos1x, pos1y,
							ETO_OPAQUE, NULL,
							name, (int)_tcslen(name), NULL);
				SetBkMode(hdcM->hDC,TRANSPARENT);
			}

			//全ての内部パーツを描画
			if( open ){
				std::list<OrgWindowParts*>::iterator itr;
				for(itr = partsList.begin(); itr != partsList.end(); itr++){
					if (*itr) {
						(*itr)->draw();
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
		///	Method : 左右マウスボタンダウンイベント受信
		virtual void onLButtonDown(const MouseEvent& e){
			onLRButtonDown(e,true);
		}
		//	Method : 左マウスボタン ダブルクリックイベント受信
		virtual void onLButtonDBLCLK(const MouseEvent& e) {//2023/10/04
			//内部パーツ
			std::list<OrgWindowParts*>::iterator plItr;
			for (plItr = partsList.begin(); plItr != partsList.end(); plItr++) {
				if (*plItr) {
					WindowPos chkpos = (*plItr)->getPos();
					WindowSize chksize = (*plItr)->getSize();
					//############################################################
					//2023/10/04 マウス位置が子供ウインドウ内部にある場合だけ処理
					//############################################################
					if ((e.localX >= chkpos.x) && (e.localX <= (chkpos.x + chksize.x)) &&
						(e.localY >= chkpos.y) && (e.localY <= (chkpos.y + chksize.y))) {

						MouseEvent mouseEvent;
						mouseEvent.globalX = e.globalX;
						mouseEvent.globalY = e.globalY;
						mouseEvent.localX = e.localX + pos.x - (*plItr)->getPos().x;
						mouseEvent.localY = e.localY + pos.y - (*plItr)->getPos().y;
						mouseEvent.altKey = e.altKey;
						mouseEvent.shiftKey = e.shiftKey;
						mouseEvent.ctrlKey = e.ctrlKey;

						(*plItr)->onLButtonDBLCLK(mouseEvent);
					}
				}
			}
		}
		//	Method : 右マウスボタン ダブルクリックイベント受信
		virtual void onRButtonDBLCLK(const MouseEvent& e) {//2023/10/04
			//内部パーツ
			std::list<OrgWindowParts*>::iterator plItr;
			for (plItr = partsList.begin(); plItr != partsList.end(); plItr++) {
				if (*plItr) {
					WindowPos chkpos = (*plItr)->getPos();
					WindowSize chksize = (*plItr)->getSize();
					//############################################################
					//2023/10/04 マウス位置が子供ウインドウ内部にある場合だけ処理
					//############################################################
					if ((e.localX >= chkpos.x) && (e.localX <= (chkpos.x + chksize.x)) &&
						(e.localY >= chkpos.y) && (e.localY <= (chkpos.y + chksize.y))) {

						MouseEvent mouseEvent;
						mouseEvent.globalX = e.globalX;
						mouseEvent.globalY = e.globalY;
						mouseEvent.localX = e.localX + pos.x - (*plItr)->getPos().x;
						mouseEvent.localY = e.localY + pos.y - (*plItr)->getPos().y;
						mouseEvent.altKey = e.altKey;
						mouseEvent.shiftKey = e.shiftKey;
						mouseEvent.ctrlKey = e.ctrlKey;

						(*plItr)->onRButtonDBLCLK(mouseEvent);
					}
				}
			}
		}

		virtual void onRButtonDown(const MouseEvent& e){
			onLRButtonDown(e,false);
		}
		///	Method : 左右マウスボタンアップイベント受信
		virtual void onLButtonUp(const MouseEvent& e){
			onLRButtonUp(e,true);
		}
		virtual void onRButtonUp(const MouseEvent& e){
			onLRButtonUp(e,false);
		}
		///	Method : マウス移動イベント受信
		virtual void onMouseMove(const MouseEvent& e){
			//内部パーツ
			std::list<OrgWindowParts*>::iterator plItr;
			for (plItr = partsList.begin(); plItr != partsList.end(); plItr++) {

				if (*plItr) {
					MouseEvent mouseEvent;
					mouseEvent.globalX = e.globalX;
					mouseEvent.globalY = e.globalY;
					mouseEvent.localX = e.localX + pos.x - (*plItr)->getPos().x;
					mouseEvent.localY = e.localY + pos.y - (*plItr)->getPos().y;
					mouseEvent.altKey = e.altKey;
					mouseEvent.shiftKey = e.shiftKey;
					mouseEvent.ctrlKey = e.ctrlKey;

					(*plItr)->onMouseMove(mouseEvent);
				}
			}
		}

		/////////////////////////// Accessor /////////////////////////////
		/// Accessor : name
		const TCHAR* getName() const{
			return name;
		}
		void setName( const TCHAR *value ){
			if (name && value) {
				size_t tclen = _tcslen(value);
				size_t cplen;
				if (tclen != 0) {
					if (tclen <= 255) {
						cplen = tclen;
					}
					else {
						cplen = 255;
					}
					_tcsncpy_s(name, 256, value, cplen);
					name[cplen] = (TCHAR)0;
				}
				else {
					_tcscpy_s(name, 256, TEXT("NoName"));
				}
			}

			callRewrite();
		}
		/// Accessor : open
		bool getOpenStatus() const{
			return open;
		}
		void setOpenStatus(bool value){

			//現在の開閉状態と異なる状態をセットした時だけ変更する
			if( open!=value ){
				open= value;

				//リスナーコール
				if( this->openListener!=NULL ){
					(this->openListener)();
				}

				//ウィンドウ内部品の全ての位置とサイズを自動設定
				if( parentWindow!=NULL ){
					parentWindow->autoResizeAllParts();
				}
			}
		}
		/// Accessor : canClose
		bool getCanCloseType() const{
			return canClose;
		}
		void setCanCloseType(bool value){
			if(canClose!=value){
				canClose= value;
				if( open ){
					callRewrite();
				}else{
					setOpenStatus(true);
				}
			}
		}

	private:
		////////////////////////// MemberVar /////////////////////////////
		TCHAR *name;

		int currentPartsSizeY;
		WindowPos  partsAreaPos;
		WindowSize partsAreaSize;
		std::list<OrgWindowParts*> partsList;

		bool open;
		bool canClose;
		std::function<void()> openListener;

		static const int SIZE_CLOSE_Y= 13;
		static const int BOX_POS_X= 3;
		static const int BOX_WIDTH= 10;
		static const int NAME_POS_X1= 5;
		static const int NAME_POS_X2= 3;

		///	Method : 左右マウスボタンダウンイベント受信
		void onLRButtonDown(const MouseEvent& e, bool lButton){

			if( lButton ){
				//ラベルクリックで開閉
				if( (e.localY < SIZE_CLOSE_Y) && canClose ){
					setOpenStatus(!open);
				}
			}

			//内部パーツ
			std::list<OrgWindowParts*>::iterator plItr;
			for (plItr = partsList.begin(); plItr != partsList.end(); plItr++) {

				if (*plItr) {
					WindowSize partsSize = (*plItr)->getSize();
					int tmpPosX = e.localX + pos.x - (*plItr)->getPos().x;
					int tmpPosY = e.localY + pos.y - (*plItr)->getPos().y;
					if ((0 <= tmpPosX) && (tmpPosX < partsSize.x) &&
						(0 <= tmpPosY) && (tmpPosY < partsSize.y)) {

						MouseEvent mouseEvent;
						mouseEvent.globalX = e.globalX;
						mouseEvent.globalY = e.globalY;
						mouseEvent.localX = tmpPosX;
						mouseEvent.localY = tmpPosY;
						mouseEvent.altKey = e.altKey;
						mouseEvent.shiftKey = e.shiftKey;
						mouseEvent.ctrlKey = e.ctrlKey;

						if (lButton) {
							(*plItr)->onLButtonDown(mouseEvent);
						}
						else {
							(*plItr)->onRButtonDown(mouseEvent);
						}
						return;
					}
				}
			}

		}
		///	Method : 左右マウスボタンアップイベント受信
		void onLRButtonUp(const MouseEvent& e, bool lButton){
			//内部パーツ
			std::list<OrgWindowParts*>::iterator plItr;
			for (plItr = partsList.begin(); plItr != partsList.end(); plItr++) {

				if (*plItr) {
					MouseEvent mouseEvent;
					mouseEvent.globalX = e.globalX;
					mouseEvent.globalY = e.globalY;
					mouseEvent.localX = e.localX + pos.x - (*plItr)->getPos().x;
					mouseEvent.localY = e.localY + pos.y - (*plItr)->getPos().y;
					mouseEvent.altKey = e.altKey;
					mouseEvent.shiftKey = e.shiftKey;
					mouseEvent.ctrlKey = e.ctrlKey;

					if (lButton) {
						(*plItr)->onLButtonUp(mouseEvent);
					}
					else {
						(*plItr)->onRButtonUp(mouseEvent);
					}
				}
			}
		}
	};

	///<summary>
	///	ウィンドウ内部品"ラベル"クラス
	///</summary>
	class OWP_Label : public OrgWindowParts{
	public:
		//////////////////// Constructor/Destructor //////////////////////
		OWP_Label(const TCHAR *_name ) : OrgWindowParts() {
			name = new TCHAR[256];
			if (_name) {
				size_t tclen = _tcslen(_name);
				size_t cplen;
				if (tclen != 0) {
					if (tclen <= 255) {
						cplen = tclen;
					}
					else {
						cplen = 255;
					}
					_tcsncpy_s(name, 256, _name, cplen);
					name[cplen] = (TCHAR)0;
				}
				else {
					_tcscpy_s(name, 256, TEXT("NoName"));
				}
			}
		}
		~OWP_Label(){
			delete[] name;
		}

		//////////////////////////// Method //////////////////////////////
		/// Method : 自動サイズ設定
		virtual void autoResize(){
			size.y= SIZE_Y;
		}
		//	Method : 描画
		virtual void draw(){
			if (!hdcM) {
				return;
			}

			//名前
			int pos1x= pos.x+NAME_POS_X;
			int pos1y= pos.y+size.y/2-5;
			hdcM->setFont(12,_T("ＭＳ ゴシック"));
			//SetTextColor(hdcM->hDC,RGB(240,240,240));
			SetTextColor(hdcM->hDC, OrgWindowParts::getTextColor());
			TextOut( hdcM->hDC,
					 pos1x, pos1y,
					 name, (int)_tcslen(name));

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

				if (g_enableDS) {
					if (_tcscmp(name, L".") == 0) {
						POINT barpos;
						barpos = g_currentsubmenupos;
						::ScreenToClient(getParent()->getHWnd(), &barpos);
						int BMP_W = g_submenuwidth;
						int BMP_H = 16;
						Gdiplus::Graphics* gdipg = new Gdiplus::Graphics(hdcM->hDC);
						if (gdipg) {
							Gdiplus::ImageAttributes attr;
							Gdiplus::ColorMatrix cmat = {
								1.0f, 0.0f, 0.0f, 0.0f, 0.0f,   // Red
								0.0f, 1.0f, 0.0f, 0.0f, 0.0f,   // Green
								0.0f, 0.0f, 1.0f, 0.0f, 0.0f,   // Blue
								0.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // Alpha
								0.0f, 0.0f, 0.0f, 0.0f, 1.0f    // must be 1
							};
							attr.SetColorMatrix(&cmat);
							//Gdiplus::Image* imgptr = new Gdiplus::Image(L"E:\\PG\\AdditiveIK\Media\\MameMedia\\img_l105.png");
							if (g_menuaimbarimage) {
								gdipg->DrawImage(g_menuaimbarimage, Gdiplus::Rect(barpos.x, barpos.y, BMP_W, BMP_H),
									0, 0, 140, 6,
									Gdiplus::UnitPixel, &attr, NULL, NULL);
							}
							delete gdipg;
						}
					}
				}
			}

		}
		/// Method : 内容変更
		void setName(const TCHAR *value ){
			if (name && value) {
				size_t tclen = _tcslen(value);
				size_t cplen;
				if (tclen != 0) {
					if (tclen <= 255) {
						cplen = tclen;
					}
					else {
						cplen = 255;
					}
					_tcsncpy_s(name, 256, value, cplen);
					name[cplen] = (TCHAR)0;
				}
				else {
					_tcscpy_s(name, 256, TEXT("NoName"));
				}
			}

			callRewrite();
		}

	private:
		////////////////////////// MemberVar /////////////////////////////
		TCHAR *name;

		static const int SIZE_Y= 15;
		static const int NAME_POS_X= 5;
	};

	///<summary>
	///	ウィンドウ内部品"ボタン"クラス
	///</summary>
	class OWP_Button : public OrgWindowParts{
	public:
		//////////////////// Constructor/Destructor //////////////////////
		OWP_Button( const TCHAR *_name=_T("") ) : OrgWindowParts() {
			name = new TCHAR[256];
			if (_name) {
				size_t tclen = _tcslen(_name);
				size_t cplen;
				if (tclen != 0) {
					if (tclen <= 255) {
						cplen = tclen;
					}
					else {
						cplen = 255;
					}
					_tcsncpy_s(name, 256, _name, cplen);
					name[cplen] = (TCHAR)0;
				}
				else {
					_tcscpy_s(name, 256, TEXT("NoName"));
				}
			}

			buttonPush=false;
			//buttonListener = [](){s_dummyfunc();};
			buttonListener = NULL;

			underButtonUpThreadFlag = 0;
		}
		~OWP_Button(){
			while (InterlockedAdd(&underButtonUpThreadFlag, 0) != 0) {
				//ボタンを押すアニメーションを再生中に　ボタンを削除しないように　待機ループ
				Sleep(1);
			}
			delete[] name;
		}

		//////////////////////////// Method //////////////////////////////
		/// Method : 自動サイズ設定
		virtual void autoResize(){
			size.y= SIZE_Y;
		}
		//	Method : 描画
		virtual void draw(){
			if (!hdcM) {
				return;
			}


			drawEdge();

			//ボックス
			int pos1x= pos.x+BOX_POS_X;
			int pos1y= pos.y+size.y/2-BOX_WIDTH/2;
			int pos2x= pos.x+BOX_POS_X+BOX_WIDTH-1;
			int pos2y= pos.y+size.y/2+BOX_WIDTH/2-1;
			if(buttonPush){		
				hdcM->setPenAndBrush(RGB(min(baseColor.r+20,255),min(baseColor.g+20,255),min(baseColor.b+20,255)),NULL);
			}else{				
				hdcM->setPenAndBrush(RGB(240,240,240),NULL); 
			}
			Rectangle(hdcM->hDC,pos1x,pos1y,pos2x+1,pos2y+1);
			if(!buttonPush){	
				hdcM->setPenAndBrush(RGB(min(baseColor.r+20,255),min(baseColor.g+20,255),min(baseColor.b+20,255)),NULL);
			}else{				
				hdcM->setPenAndBrush(RGB(240,240,240),NULL); 
			}
			MoveToEx(hdcM->hDC, pos1x,pos2y, NULL);
			LineTo(hdcM->hDC,   pos2x,pos2y);
			LineTo(hdcM->hDC,   pos2x,pos1y);

			//名前
			pos1x= pos.x+BOX_POS_X+BOX_WIDTH+3;
			pos1y= pos.y+size.y/2-5;
			hdcM->setFont(12,_T("ＭＳ ゴシック"));
			//TCHAR* isToAll = 0;
			//isToAll = _tcsstr(name, _T("ToAll"));
			//if(!isToAll){
			//	SetTextColor(hdcM->hDC, RGB(240, 240, 240));
			//}
			//else {
			//	//名前にToAllとついていた場合には、特殊ボタンとして色を変える
			//	SetTextColor(hdcM->hDC, RGB(64, 128 + 32, 128 + 32));
			//}
			
			SetTextColor(hdcM->hDC, OrgWindowParts::getTextColor());

			TextOut( hdcM->hDC,
					 pos1x, pos1y,
					 name, (int)_tcslen(name));
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
		//	Method : マウスダウンイベント受信
		virtual void onLButtonDown(const MouseEvent& e){
			if ((g_endappflag == 0) && parentWindow && IsWindow(parentWindow->getHWnd())) {

				if (this->buttonListener != NULL) {
					(this->buttonListener)();
				}

				buttonPush = true;

				RECT tmpRect;
				tmpRect.left = pos.x + 1;
				tmpRect.top = pos.y + 1;
				tmpRect.right = pos.x + size.x - 1;
				tmpRect.bottom = pos.y + size.y - 1;
				InvalidateRect(parentWindow->getHWnd(), &tmpRect, false);
				draw();

				//ボタンアップアニメーションのためのスレッド作成
				InterlockedExchange(&underButtonUpThreadFlag, (LONG)1);
				_beginthread(drawButtonUpThread, 0, (void*)this);
			}
		}
		//	Method : 左マウスボタン ダブルクリックイベント受信
		virtual void onLButtonDBLCLK(const MouseEvent& e) {//2023/10/04
		}
		//	Method : 右マウスボタン ダブルクリックイベント受信
		virtual void onRButtonDBLCLK(const MouseEvent& e) {//2023/10/04
			int dbgflag1 = 1;
		}

		/////////////////////////// Accessor /////////////////////////////
		//	Accessor : buttonListener
		void setButtonListener(std::function<void()> listener){
			this->buttonListener= listener;
		}
		void setName(const TCHAR *value){
			if (name && value) {
				size_t tclen = _tcslen(value);
				size_t cplen;
				if (tclen != 0) {
					if (tclen <= 255) {
						cplen = tclen;
					}
					else {
						cplen = 255;
					}
					_tcsncpy_s(name, 256, value, cplen);
					name[cplen] = (TCHAR)0;
				}
				else {
					_tcscpy_s(name, 256, TEXT("NoName"));
				}
			}

			callRewrite();
		}
	private:
		////////////////////////// MemberVar /////////////////////////////
		TCHAR *name;

		bool buttonPush;
		std::function<void()> buttonListener;

		static const int SIZE_Y= 15;
		static const int BOX_POS_X= 3;
		static const int BOX_WIDTH= 10;

		LONG underButtonUpThreadFlag;//ボタンを押している間にボタンを削除しないようにフラグを立てる

		//////////////////////////// Method //////////////////////////////
		//	Method : ボタンアップのスレッド
		static void drawButtonUpThread(LPVOID	pParam){
			Sleep(100);

			OWP_Button *thisClass= (OWP_Button*)pParam;

			thisClass->buttonPush=false;

			if (thisClass->parentWindow && IsWindow(thisClass->parentWindow->getHWnd())) {

				RECT tmpRect;
				tmpRect.left = thisClass->pos.x + 1;
				tmpRect.top = thisClass->pos.y + 1;
				tmpRect.right = thisClass->pos.x + thisClass->size.x - 1;
				tmpRect.bottom = thisClass->pos.y + thisClass->size.y - 1;
				InvalidateRect(thisClass->parentWindow->getHWnd(), &tmpRect, false);
			}

			InterlockedExchange(&(thisClass->underButtonUpThreadFlag), (LONG)0);
		}
	};

	///<summary>
	///	ウィンドウ内部品"プレイヤーボタン"クラス
	///</summary>
	class OWP_PlayerButton : public OrgWindowParts{
	public:
		//////////////////// Constructor/Destructor //////////////////////
		OWP_PlayerButton(int srctotalwidth) : OrgWindowParts() {
			setIsPlayerButton(true);
			wcscpy_s(strjointname, 1024, L"Unknown joint");

			if (g_4kresolution) {
				//BOX_WIDTHとSIZE_Yは setButtonSizeでも変更出来る
				BOX_WIDTH = 40;
				SIZE_Y = 44;

				//ボタンの配置開始位置
				//OFFSET_X = srctotalwidth / 3 + 80;
				OFFSET_X = srctotalwidth / 3 + 80 - BOX_WIDTH * 3;
			}
			else {
				//BOX_WIDTHとSIZE_Yは setButtonSizeでも変更出来る
				//BOX_WIDTH = 20;
				//SIZE_Y = 24;
				BOX_WIDTH = 26;
				SIZE_Y = 30;

				//OFFSET_X = srctotalwidth / 4 + 30;
				OFFSET_X = srctotalwidth / 4 + 30 - BOX_WIDTH * 3;
			}


		}
		~OWP_PlayerButton(){
		}

		WindowPos getButtonPos(int buttonno)
		{
			WindowPos retpos;

			if ((buttonno >= 0) && (buttonno <= 9)) {
				//ボタンの四隅になる座標を求める
				int pos1x = OFFSET_X + pos.x + BOX_POS_X + BOX_WIDTH * buttonno;
				int pos1y = pos.y + size.y / 2 - BOX_WIDTH / 2;
				int pos2x = OFFSET_X + pos.x + BOX_POS_X + BOX_WIDTH * (buttonno + 1) - 1;
				int pos2y = pos1y + BOX_WIDTH - 1;

				retpos.x = ((pos1x + pos2x) / 2);
				retpos.y = ((pos1y + pos2y) / 2);
			}
			else {
				_ASSERT(0);
				retpos.x = 0;
				retpos.y = 0;
			}

			return retpos;
		};

		//////////////////////////// Method //////////////////////////////
		/// Method : 自動サイズ設定
		virtual void autoResize(){
			size.y= SIZE_Y;
		}
		//	Method : 描画
		virtual void draw(){
			if (!hdcM) {
				return;
			}


			drawEdge();

			//全てのボタンについて繰り返す
			for(int i=0; i<=9; i++){

				//ボタンの四隅になる座標を求める
				int pos1x= OFFSET_X + pos.x+BOX_POS_X+BOX_WIDTH*i;
				int pos1y= pos.y+size.y/2-BOX_WIDTH/2;
				int pos2x= OFFSET_X + pos.x+BOX_POS_X+BOX_WIDTH*(i+1)-1;
				int pos2y= pos1y+BOX_WIDTH-1;

				//ボタンパラメータのインスタンスへのポインタを作成
				OneButtonParam *btnPrm;
				//switch(i){
				//case 0: btnPrm= &reset; break;
				//case 1: btnPrm= &backStep; break;
				//case 2: btnPrm= &backPlay; break;
				//case 3: btnPrm= &stop; break;
				//case 4: btnPrm= &frontPlay; break;
				//case 5: btnPrm= &frontStep; break;
				//case 6: btnPrm = &onefps; break;
				//case 7: btnPrm = &selecttolast; break;
				//case 8: btnPrm = &btreset; break;
				//case 9: btnPrm = &prevrange; break;
				//case 10: btnPrm = &nextrange; break;
				//case 11: btnPrm = &plusdisp; break;
				//case 12: btnPrm = &minusdisp; break;
				//case 13: btnPrm = &plusoffsetdisp; break;
				//case 14: btnPrm = &minusoffsetdisp; break;
				//case 15: btnPrm = &resetdisp; break;
				//}
				switch (i) {
				case 0: btnPrm = &physicsPlay; break;
				case 1: btnPrm = &physicsRec; break;
				case 2: btnPrm = 0; break;
				case 3: btnPrm = &backStep; break;
				case 4: btnPrm = &backPlay; break;
				case 5: btnPrm = &reset; break;
				case 6: btnPrm = &frontPlay; break;
				case 7: btnPrm = &frontStep; break;
				case 8: btnPrm = 0; break;
				case 9: btnPrm = &selecttolast; break;
				default: btnPrm = 0; break;
				}
				if (btnPrm) {
					//枠組み描画
					if (btnPrm->buttonPush) {
						hdcM->setPenAndBrush(RGB(min(baseColor.r + 20, 255), min(baseColor.g + 20, 255), min(baseColor.b + 20, 255)), NULL);
					}
					else {
						hdcM->setPenAndBrush(RGB(240, 240, 240), NULL);
					}
					Rectangle(hdcM->hDC, pos1x, pos1y, pos2x + 1, pos2y + 1);
					if (!btnPrm->buttonPush) {
						hdcM->setPenAndBrush(RGB(min(baseColor.r + 20, 255), min(baseColor.g + 20, 255), min(baseColor.b + 20, 255)), NULL);
					}
					else {
						hdcM->setPenAndBrush(RGB(240, 240, 240), NULL);
					}
					MoveToEx(hdcM->hDC, pos1x, pos2y, NULL);
					LineTo(hdcM->hDC, pos2x, pos2y);
					LineTo(hdcM->hDC, pos2x, pos1y);

					//中身描画
					int shiftDot = btnPrm->buttonPush ? 1 : 0;
					switch (i) {
					case 0:		//物理再生ボタン
					{
						hdcM->setPenAndBrush(RGB(125, 125, 125), RGB(125, 125, 125));
						int y1 = pos1y + 3;
						int y2 = pos2y - 2;
						int x1 = (pos1x + pos2x) / 2 - (y2 - y1) / 4;
						int x2 = x1 + (y2 - y1) / 2;
						for (int j = 0; j <= x2 - x1; j++) {
							MoveToEx(hdcM->hDC, x1 + j + shiftDot, y1 + j + shiftDot, NULL);
							LineTo(hdcM->hDC, x1 + j + shiftDot, y2 - j + shiftDot);
						}
					}break;
					case 1:		//物理RECボタン
					{
						hdcM->setPenAndBrush(RGB(168, 129, 129), RGB(168, 129, 129));
						int y1 = pos1y + 3;
						int y2 = pos2y - 2;
						int x1 = (pos1x + pos2x) / 2 - (y2 - y1) / 4;
						int x2 = x1 + (y2 - y1) / 2;

						Ellipse(hdcM->hDC, pos1x + 4, pos1y + 4, pos2x - 4, pos2y - 4);

						//for (int j = 0; j <= x2 - x1; j++) {
						//	MoveToEx(hdcM->hDC, x1 + j + shiftDot, y1 + j + shiftDot, NULL);
						//	LineTo(hdcM->hDC, x1 + j + shiftDot, y2 - j + shiftDot);
						//}
					}break;
					case 2:
						break;




					case 3:		//先頭フレームジャンプボタン
					{
						hdcM->setPenAndBrush(RGB(240, 240, 240), RGB(240, 240, 240));
						int x1 = (pos1x + pos2x) / 2 + 2;
						int x2 = pos2x - 2;
						Rectangle(hdcM->hDC, x1 + shiftDot, pos1y + 3 + shiftDot, x2 + shiftDot, pos2y - 2 + shiftDot);

						int y1 = pos1y + 3;
						int y2 = pos2y - 2;
						int x4 = (pos1x + pos2x) / 2;
						int x3 = x4 - (y2 - y1) / 2;
						for (int j = 0; j <= x4 - x3; j++) {
							MoveToEx(hdcM->hDC, x4 - j + shiftDot, y1 + j + shiftDot, NULL);
							LineTo(hdcM->hDC, x4 - j + shiftDot, y2 - j + shiftDot);
						}
					}break;
					case 4:		//逆再生ボタン
					{
						hdcM->setPenAndBrush(RGB(240, 240, 240), RGB(240, 240, 240));
						int y1 = pos1y + 3;
						int y2 = pos2y - 2;
						int x1 = (pos1x + pos2x) / 2 - (y2 - y1) / 4;
						int x2 = x1 + (y2 - y1) / 2;
						for (int j = 0; j <= x2 - x1; j++) {
							MoveToEx(hdcM->hDC, x2 - j + shiftDot, y1 + j + shiftDot, NULL);
							LineTo(hdcM->hDC, x2 - j + shiftDot, y2 - j + shiftDot);
						}
					}break;
					case 5:		//リセットボタン
						hdcM->setPenAndBrush(RGB(240, 240, 240), RGB(240, 240, 240));
						Rectangle(hdcM->hDC,
							pos1x + 4 + shiftDot, pos1y + 4 + shiftDot,
							pos2x - 3 + shiftDot, pos2y - 3 + shiftDot);
						break;
					case 6:		//再生ボタン
					{
						hdcM->setPenAndBrush(RGB(240, 240, 240), RGB(240, 240, 240));
						int y1 = pos1y + 3;
						int y2 = pos2y - 2;
						int x1 = (pos1x + pos2x) / 2 - (y2 - y1) / 4;
						int x2 = x1 + (y2 - y1) / 2;
						for (int j = 0; j <= x2 - x1; j++) {
							MoveToEx(hdcM->hDC, x1 + j + shiftDot, y1 + j + shiftDot, NULL);
							LineTo(hdcM->hDC, x1 + j + shiftDot, y2 - j + shiftDot);
						}
					}break;
					case 7:		//最終フレームジャンプボタン
					{
						hdcM->setPenAndBrush(RGB(240, 240, 240), RGB(240, 240, 240));
						int x1 = pos1x + 3;
						int x2 = (pos1x + pos2x) / 2 - 1;
						Rectangle(hdcM->hDC, x1 + shiftDot, pos1y + 3 + shiftDot, x2 + shiftDot, pos2y - 2 + shiftDot);

						int y1 = pos1y + 3;
						int y2 = pos2y - 2;
						int x3 = (pos1x + pos2x) / 2;
						int x4 = x3 + (y2 - y1) / 2;
						for (int j = 0; j <= x4 - x3; j++) {
							MoveToEx(hdcM->hDC, x3 + j + shiftDot, y1 + j + shiftDot, NULL);
							LineTo(hdcM->hDC, x3 + j + shiftDot, y2 - j + shiftDot);
						}
					}break;
					case 8:
						break;
					case 9:		//最終フレームまで選択
					{
						hdcM->setPenAndBrush(RGB(240, 240, 240), RGB(240, 240, 240));
						int x1 = pos1x + 4;
						int x2 = pos2x - 4;
						int y1 = pos1y + 3;
						int y2 = pos2y - 3;

						MoveToEx(hdcM->hDC, x1, y1, NULL);
						LineTo(hdcM->hDC, x1, y2);
						MoveToEx(hdcM->hDC, x2, y1, NULL);
						LineTo(hdcM->hDC, x2, y2);
					}break;
					default:
						break;
					}
				}
				
			}

			//2023/01/08
			//ジョイント名を　大きく　タイムライン上の　真ん中に表示
			if(getParent()){
				WindowSize parentsize = getParent()->getSizeSimple();
				//int pos1x = pos.x + BOX_POS_X + BOX_WIDTH * 18;
				int pos1x = pos.x + parentsize.x / 2;
				int pos1y = pos.y + size.y / 2 - BOX_WIDTH / 2;
				const WCHAR* pjointname = getJointName();
				if (pjointname) {
					size_t jointnamelen = wcslen(pjointname);
					if ((jointnamelen >= 1) && (jointnamelen <= 1020)) {
						//hdcM->setFont(18, _T("ＭＳ ゴシック"));
						if (g_4kresolution) {
							hdcM->setFont(32, _T("ＭＳ ゴシック"));
						}
						else {
							hdcM->setFont(24, _T("ＭＳ ゴシック"));
						}
						SetTextColor(hdcM->hDC, RGB(168, 129, 129));
						TextOut(hdcM->hDC,
							pos1x, pos1y,
							pjointname, (int)jointnamelen);
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

		//	Method : マウスダウンイベント受信
		virtual void onLButtonDown(const MouseEvent& e){

			//全てのボタンについて繰り返す
			for(int i=0; i<=9; i++){

				//まずボタンが押されたかを確認
				if( ((OFFSET_X + BOX_POS_X + BOX_WIDTH * i) <= e.localX) && (e.localX < (OFFSET_X + BOX_POS_X + BOX_WIDTH * (i+1))) ){
				}else{
					continue;
				}

				//ボタンパラメータのインスタンスへのポインタを作成
				OneButtonParam *btnPrm;
				switch (i) {
				case 0: btnPrm = &physicsPlay; break;
				case 1: btnPrm = &physicsRec; break;
				case 2: btnPrm = 0; break;
				case 3: btnPrm = &backStep; break;
				case 4: btnPrm = &backPlay; break;
				case 5: btnPrm = &reset; break;
				case 6: btnPrm = &frontPlay; break;
				case 7: btnPrm = &frontStep; break;
				case 8: btnPrm = 0; break;
				case 9: btnPrm = &selecttolast; break;
				default: btnPrm = 0; break;
				}
				if (btnPrm) {
					//ボタンリスナーを呼ぶ
					if (btnPrm->buttonListener != NULL) {
						(btnPrm->buttonListener)();
					}

					//ボタン押下状態をONにする
					btnPrm->buttonPush = true;

					//再描画通知
					callRewrite();

					//ボタンアップアニメーションのためのスレッド作成
					switch (i) {
					case 0: _beginthread(drawPhysicsPlayButtonUpThread, 0, (void*)this); break;
					case 1: _beginthread(drawPhysicsRecButtonUpThread, 0, (void*)this); break;
					case 2: break;
					case 3: _beginthread(drawBackStepButtonUpThread, 0, (void*)this); break;
					case 4: _beginthread(drawBackPlayButtonUpThread, 0, (void*)this); break;
					case 5: _beginthread(drawResetButtonUpThread, 0, (void*)this); break;
					case 6: _beginthread(drawFrontPlayButtonUpThread, 0, (void*)this); break;
					case 7: _beginthread(drawFrontStepButtonUpThread, 0, (void*)this); break;
					case 8: break;
					case 9: _beginthread(drawSelectToLastButtonUpThread, 0, (void*)this); break;
					default: break;
					}
				}
				return;
			}
		}
		//	Method : 左マウスボタン ダブルクリックイベント受信
		virtual void onLButtonDBLCLK(const MouseEvent& e) {//2023/10/04
		}
		//	Method : 右マウスボタン ダブルクリックイベント受信
		virtual void onRButtonDBLCLK(const MouseEvent& e) {//2023/10/04
			int dbgflag1 = 1;
		}


		/////////////////////////// Accessor /////////////////////////////
		//	Accessor : buttonListener
		void setJointName(const WCHAR* srcjointname) {
			if (srcjointname) {
				strjointname[1024 - 1] = 0L;//念のため終端
				wcscpy_s(strjointname, 1024, srcjointname);
				strjointname[1024 - 1] = 0L;//念のため終端
			}
		}
		const WCHAR* getJointName()
		{
			strjointname[1024 - 1] = 0L;//念のため終端
			return &(strjointname[0]);
		}

		void setPhysicsPlayButtonListener(std::function<void()> listener) {
			physicsPlay.buttonListener = listener;
		}
		void setPhysicsRecButtonListener(std::function<void()> listener) {
			physicsRec.buttonListener = listener;
		}
		void setFrontPlayButtonListener(std::function<void()> listener){
			frontPlay.buttonListener= listener;
		}
		void setBackPlayButtonListener(std::function<void()> listener){
			backPlay.buttonListener= listener;
		}
		//void setStopButtonListener(std::function<void()> listener){
		//	stop.buttonListener= listener;
		//}
		void setResetButtonListener(std::function<void()> listener){
			reset.buttonListener= listener;
		}
		void setFrontStepButtonListener(std::function<void()> listener){
			frontStep.buttonListener= listener;
		}
		void setBackStepButtonListener(std::function<void()> listener){
			backStep.buttonListener= listener;
		}
		//void setOneFpsButtonListener(std::function<void()> listener) {
		//	onefps.buttonListener = listener;
		//}
		void setSelectToLastButtonListener(std::function<void()> listener){
			selecttolast.buttonListener = listener;
		}
		//void setBtResetButtonListener(std::function<void()> listener){
		//	btreset.buttonListener = listener;
		//}
		//void setPrevRangeButtonListener(std::function<void()> listener){
		//	prevrange.buttonListener = listener;
		//}
		//void setNextRangeButtonListener(std::function<void()> listener){
		//	nextrange.buttonListener = listener;
		//}
		//void setPlusDispButtonListener(std::function<void()> listener) {
		//	plusdisp.buttonListener = listener;
		//}
		//void setMinusDispButtonListener(std::function<void()> listener) {
		//	minusdisp.buttonListener = listener;
		//}
		//void setPlusOffsetDispButtonListener(std::function<void()> listener) {
		//	plusoffsetdisp.buttonListener = listener;
		//}
		//void setMinusOffsetDispButtonListener(std::function<void()> listener) {
		//	minusoffsetdisp.buttonListener = listener;
		//}
		//void setResetDispButtonListener(std::function<void()> listener) {
		//	resetdisp.buttonListener = listener;
		//}

		/// Accessor : ボタンサイズを変更する
		void setButtonSize(int value){
			BOX_WIDTH= value;
			SIZE_Y= BOX_WIDTH+4;
		}
		int getButtonSize() const{
			return BOX_WIDTH;
		}

	private:
		////////////////////////// MemberVar /////////////////////////////

		class OneButtonParam{
		public:
			OneButtonParam(){
				buttonPush= false;
				//buttonListener = [](){s_dummyfunc();};
				buttonListener = NULL;
			}

			bool buttonPush;
			std::function<void()> buttonListener;
		//}frontPlay,backPlay,stop,reset,frontStep,backStep,onefps,selecttolast,btreset,prevrange,nextrange,plusdisp,minusdisp,plusoffsetdisp,minusoffsetdisp,resetdisp;
		}physicsPlay, physicsRec, backStep, backPlay, reset, frontPlay, frontStep, selecttolast;
		

		int SIZE_Y;
		static const int BOX_POS_X= 3;
		int BOX_WIDTH;
		int OFFSET_X;

		WCHAR strjointname[1024] = { 0L };

		//////////////////////////// Method //////////////////////////////
		//	Method : ボタンアップのスレッド
		static void drawPhysicsPlayButtonUpThread(LPVOID	pParam) {
			Sleep(100);

			OWP_PlayerButton* thisClass = (OWP_PlayerButton*)pParam;
			thisClass->physicsPlay.buttonPush = false;
			thisClass->callRewrite();
		}
		static void drawPhysicsRecButtonUpThread(LPVOID	pParam) {
			Sleep(100);

			OWP_PlayerButton* thisClass = (OWP_PlayerButton*)pParam;
			thisClass->physicsRec.buttonPush = false;
			thisClass->callRewrite();
		}
		static void drawFrontPlayButtonUpThread(LPVOID	pParam){
			Sleep(100);

			OWP_PlayerButton *thisClass= (OWP_PlayerButton*)pParam;
			thisClass->frontPlay.buttonPush=false;
			thisClass->callRewrite();
		}
		static void drawBackPlayButtonUpThread(LPVOID	pParam){
			Sleep(100);

			OWP_PlayerButton *thisClass= (OWP_PlayerButton*)pParam;
			thisClass->backPlay.buttonPush=false;
			thisClass->callRewrite();
		}
		//static void drawStopButtonUpThread(LPVOID	pParam){
		//	Sleep(100);

		//	OWP_PlayerButton *thisClass= (OWP_PlayerButton*)pParam;
		//	thisClass->stop.buttonPush=false;
		//	thisClass->callRewrite();
		//}
		static void drawResetButtonUpThread(LPVOID	pParam){
			Sleep(100);

			OWP_PlayerButton *thisClass= (OWP_PlayerButton*)pParam;
			thisClass->reset.buttonPush=false;
			thisClass->callRewrite();
		}
		static void drawFrontStepButtonUpThread(LPVOID	pParam){
			Sleep(100);

			OWP_PlayerButton *thisClass= (OWP_PlayerButton*)pParam;
			thisClass->frontStep.buttonPush=false;
			thisClass->callRewrite();
		}
		static void drawBackStepButtonUpThread(LPVOID	pParam){
			Sleep(100);

			OWP_PlayerButton *thisClass= (OWP_PlayerButton*)pParam;
			thisClass->backStep.buttonPush=false;
			thisClass->callRewrite();
		}
		//static void drawOneFpsButtonUpThread(LPVOID	pParam) {
		//	Sleep(100);

		//	OWP_PlayerButton* thisClass = (OWP_PlayerButton*)pParam;
		//	thisClass->onefps.buttonPush = false;
		//	thisClass->callRewrite();
		//}
		static void drawSelectToLastButtonUpThread(LPVOID	pParam){
			Sleep(100);

			OWP_PlayerButton *thisClass = (OWP_PlayerButton*)pParam;
			thisClass->selecttolast.buttonPush = false;
			thisClass->callRewrite();
		}
		//static void drawBtResetButtonUpThread(LPVOID	pParam){
		//	Sleep(100);

		//	OWP_PlayerButton *thisClass = (OWP_PlayerButton*)pParam;
		//	thisClass->btreset.buttonPush = false;
		//	thisClass->callRewrite();
		//}
		//static void drawPrevRangeButtonUpThread(LPVOID	pParam){
		//	Sleep(100);

		//	OWP_PlayerButton *thisClass = (OWP_PlayerButton*)pParam;
		//	thisClass->prevrange.buttonPush = false;
		//	thisClass->callRewrite();
		//}
		//static void drawNextRangeButtonUpThread(LPVOID	pParam){
		//	Sleep(100);

		//	OWP_PlayerButton *thisClass = (OWP_PlayerButton*)pParam;
		//	thisClass->nextrange.buttonPush = false;
		//	thisClass->callRewrite();
		//}
		//static void drawPlusDispButtonUpThread(LPVOID	pParam) {
		//	Sleep(100);

		//	OWP_PlayerButton* thisClass = (OWP_PlayerButton*)pParam;
		//	thisClass->plusdisp.buttonPush = false;
		//	thisClass->callRewrite();
		//}
		//static void drawMinusDispButtonUpThread(LPVOID	pParam) {
		//	Sleep(100);

		//	OWP_PlayerButton* thisClass = (OWP_PlayerButton*)pParam;
		//	thisClass->minusdisp.buttonPush = false;
		//	thisClass->callRewrite();
		//}
		//static void drawPlusOffsetDispButtonUpThread(LPVOID	pParam) {
		//	Sleep(100);

		//	OWP_PlayerButton* thisClass = (OWP_PlayerButton*)pParam;
		//	thisClass->plusoffsetdisp.buttonPush = false;
		//	thisClass->callRewrite();
		//}
		//static void drawMinusOffsetDispButtonUpThread(LPVOID	pParam) {
		//	Sleep(100);

		//	OWP_PlayerButton* thisClass = (OWP_PlayerButton*)pParam;
		//	thisClass->minusoffsetdisp.buttonPush = false;
		//	thisClass->callRewrite();
		//}
		//static void drawResetDispButtonUpThread(LPVOID	pParam) {
		//	Sleep(100);

		//	OWP_PlayerButton* thisClass = (OWP_PlayerButton*)pParam;
		//	thisClass->resetdisp.buttonPush = false;
		//	thisClass->callRewrite();
		//}
	};

	///<summary>
	///	ウィンドウ内部品"チェックボックス"クラス
	///</summary>
	class OWP_CheckBoxA : public OrgWindowParts {
	public:
		//////////////////// Constructor/Destructor //////////////////////
		OWP_CheckBoxA( const TCHAR *_name=_T(""), bool _value=false ) : OrgWindowParts() {
			name = new TCHAR[256];
			if (_name) {
				size_t tclen = _tcslen(_name);
				size_t cplen;
				if (tclen != 0) {
					if (tclen <= 255) {
						cplen = tclen;
					}
					else {
						cplen = 255;
					}
					_tcsncpy_s(name, 256, _name, cplen);
					name[cplen] = (TCHAR)0;
				}
				else {
					_tcscpy_s(name, 256, TEXT("NoName"));
				}
			}

			value= _value;

			//buttonListener = [](){s_dummyfunc();};
			buttonListener = NULL;
			contextmenuListener = NULL;
		}
		~OWP_CheckBoxA(){
			delete[] name;
		}

		//////////////////////////// Method //////////////////////////////
		/// Method : 自動サイズ設定
		virtual void autoResize(){
			size.y= SIZE_Y;
		}
		//	Method : 描画
		virtual void draw();
		//	Method : マウスダウンイベント受信
		virtual void onLButtonDown(const MouseEvent& e){
			//if ((e.localX >= pos.x) && (e.localX <= (pos.x + size.x)) && 
			//	(e.localY >= pos.y) && (e.localY <= (pos.y + size.y))) {
				setValue(value ^ true);
			//}
		}
		//	Method : コンテクストメニューイベント受信
		virtual void onRButtonDown(const MouseEvent& e) {
			//リスナーコール
			if (this->contextmenuListener != NULL) {
				(this->contextmenuListener)();
			}
		}
		//	Method : 左マウスボタン ダブルクリックイベント受信
		virtual void onLButtonDBLCLK(const MouseEvent& e) {//2023/10/04
		}
		//	Method : 右マウスボタン ダブルクリックイベント受信
		virtual void onRButtonDBLCLK(const MouseEvent& e) {//2023/10/04
			int dbgflag1 = 1;
		}

		/////////////////////////// Accessor /////////////////////////////
		//	Accessor : value
		void setValue(bool _value, bool calllistener = true){
			//if (parentWindow) {
			if ((g_endappflag == 0) && parentWindow && IsWindow(parentWindow->getHWnd())) {
				value = _value;

				RECT tmpRect;
				tmpRect.left = pos.x + 1;
				tmpRect.top = pos.y + 1;
				tmpRect.right = pos.x + size.x - 1;
				tmpRect.bottom = pos.y + size.y - 1;
				InvalidateRect(parentWindow->getHWnd(), &tmpRect, false);

				//リスナーコール
				//2024/02/27 プログラムからsetValueを呼び出す場合にはcalllistenerをfalseにして呼び出す　そうしないとsetValueのラムダ関数が再帰する
				if (calllistener && (this->buttonListener != NULL)) {
					(this->buttonListener)();
				}
			}
		}
		bool getValue() const{
			return value;
		}
		int getName(TCHAR* dstname, int dstleng) {
			if (dstname && (dstleng > 0) && (dstleng <= 512)) {
				_tcscpy_s(dstname, dstleng, name);
				return 0;
			}
			return 1;
		}
		//	Accessor : buttonListener
		void setButtonListener(std::function<void()> listener){
			this->buttonListener= listener;
		}
		void setContextMenuListener(std::function<void()> listener) {
			this->contextmenuListener = listener;
		}

	private:
		////////////////////////// MemberVar /////////////////////////////
		TCHAR *name;
		bool value;
		std::function<void()> buttonListener;
		std::function<void()> contextmenuListener;//右クリック用リスナー

		static const int SIZE_Y= 15;
		static const int BOX_POS_X= 3;
		static const int BOX_WIDTH= 10;
	};

	///<summary>
	///	ウィンドウ内部品"ラジオボタン"クラス
	///</summary>
	class OWP_RadioButton : public OrgWindowParts{
	public:
		//////////////////// Constructor/Destructor //////////////////////
		OWP_RadioButton( const TCHAR *name, bool srclimitnamelen) : OrgWindowParts() {

			limitnamelen = srclimitnamelen;

			if (limitnamelen) {
				//2023/02/17
				//sepalatorと一緒に使う場合に　右側にはみ出さないように文字数制限30
				TCHAR name30[31];
				ZeroMemory(name30, sizeof(TCHAR) * 31);
				if (name) {
					size_t namelen = _tcslen(name);
					size_t cplen;
					if (namelen <= 30) {
						cplen = namelen;
					}
					else {
						cplen = 30;
					}
					_tcsncpy_s(name30, 31, name, cplen);
				}
				else {
					_tcscpy_s(name30, 31, _T(" "));
				}
				nameList.push_back(name30);
			}
			else {
				if (name) {
					nameList.push_back(name);
				}
				else {
					nameList.push_back(_T(" "));
				}
			}

			selectIndex= 0;

			setIsRadioButton(true);

			//selectListener = [](){s_dummyfunc();};
			selectListener = NULL;
		}
		~OWP_RadioButton(){

		}

		//////////////////////////// Method //////////////////////////////
		/// Method : 自動サイズ設定
		virtual void autoResize(){
			size.y = SIZE_Y * (int)nameList.size() + 4;
		}
		///	Method : 描画
		virtual void draw(){
			if (!hdcM) {
				return;
			}

			drawEdge();

			//ラジオボタンの数だけ繰り返す
			for(int i=0; i<(int)nameList.size(); i++){
				//ボタン部分
				int pos1x= pos.x+BOX_POS_X;
				int pos1y= pos.y+SIZE_Y/2-BOX_WIDTH/2+ SIZE_Y*i+ 2;
				int pos2x= pos.x+BOX_POS_X+BOX_WIDTH-1;
				int pos2y= pos.y+SIZE_Y/2+BOX_WIDTH/2-1+ SIZE_Y*i+ 2;
				hdcM->setPenAndBrush(NULL,RGB(240,240,240));
				if( i==selectIndex ){
					Ellipse(hdcM->hDC, pos1x+2,pos1y+2, pos2x-2,pos2y-2);
				}
				hdcM->setPenAndBrush(RGB(min(baseColor.r+20,255),min(baseColor.g+20,255),min(baseColor.b+20,255)),NULL);
				Ellipse(hdcM->hDC, pos1x,pos1y, pos2x,pos2y);

				//名前
				pos1x= pos.x+BOX_POS_X+BOX_WIDTH+3;
				pos1y= pos.y+SIZE_Y/2-6+ SIZE_Y*i+ 2;
				hdcM->setFont(12,_T("ＭＳ ゴシック"));
				SetTextColor(hdcM->hDC,RGB(240,240,240));
				TextOut( hdcM->hDC,
						 pos1x, pos1y,
						 nameList[i].c_str(), (int)_tcslen(nameList[i].c_str()));

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
		}
		///	Method : マウスダウンイベント受信
		virtual void onLButtonDown(const MouseEvent& e){
			int targetIndex= (e.localY-2)/SIZE_Y;
			if( (0 <= targetIndex) && (targetIndex < (int)nameList.size()) ){
				setSelectIndex(targetIndex);
			}
		}
		//	Method : 左マウスボタン ダブルクリックイベント受信
		virtual void onLButtonDBLCLK(const MouseEvent& e) {//2023/10/04
		}
		//	Method : 右マウスボタン ダブルクリックイベント受信
		virtual void onRButtonDBLCLK(const MouseEvent& e) {//2023/10/04
			int dbgflag1 = 1;
		}

		/// Method : 項目の追加
		void addLine( const TCHAR *name ){
			if (limitnamelen) {
				//2023/02/17
				//sepalatorと一緒に使う場合に　右側にはみ出さないように文字数制限30
				TCHAR name30[31];
				ZeroMemory(name30, sizeof(TCHAR) * 31);
				if (name) {
					size_t namelen = _tcslen(name);
					size_t cplen;
					if (namelen <= 30) {
						cplen = namelen;
					}
					else {
						cplen = 30;
					}
					_tcsncpy_s(name30, 31, name, cplen);
				}
				else {
					_tcscpy_s(name30, 31, _T(" "));
				}
				nameList.push_back(name30);
			}
			else {
				if (name) {
					nameList.push_back(name);
				}
				else {
					nameList.push_back(_T(" "));
				}
			}

			//ウィンドウ内の全パーツの位置とサイズを自動設定
			if( parentWindow!=NULL ){
				parentWindow->autoResizeAllParts();
			}
		}
		/// Method : 項目の削除
		bool deleteLine(){

			//2つ以上の項目がある場合は最後の項目を削除
			if ((int)nameList.size() != 1){
				nameList.pop_back();

				//消した項目を選択していた場合は最後の項目を選択
				if (selectIndex == (int)nameList.size()){
					setSelectIndex(selectIndex-1);
				}

				//ウィンドウ内の全パーツの位置とサイズを自動設定
				if( parentWindow!=NULL ){
					parentWindow->autoResizeAllParts();
				}

				return true;
			}
			return false;
		}
		bool deleteLine(int index){

			//2つ以上の項目がある場合は指定したインデックスの項目を削除
			if (
			//((int)nameList.size() != 1)
			((int)nameList.size() >= 2)
			 && (0 <= index) && (index < (int)nameList.size()) ){
				for( int i=index; i<(int)nameList.size()-1; i++ ){
					nameList[i]= nameList[i+1];
				}
				nameList.pop_back();

				//消した項目以降の項目を選択していた場合は再選択
				if( selectIndex==index ){
					setSelectIndex(0);
				}else if( index<=selectIndex ){
					setSelectIndex(selectIndex-1);
				}

				//ウィンドウ内の全パーツの位置とサイズを自動設定
				if( parentWindow!=NULL ){
					parentWindow->autoResizeAllParts();
				}

				return true;
			}
			return false;
		}
		bool deleteLine( const TCHAR *name ){

			//2つ以上の項目がある場合は最初に名前が一致した項目を削除
			if ((int)nameList.size() != 1){
				for( int i=0; i<(int)nameList.size(); i++ ){
					if( nameList[i]==name ){
						return deleteLine(i);
					}
				}
			}
			return false;
		}

		/////////////////////////// Accessor /////////////////////////////
		//	Accessor : selectIndex
		void setSelectIndex(int value){
			value= max(0, min(value,(int)nameList.size()-1));
			//現在設定されている値と異なる値をセットした時にだけ変更する
			if( selectIndex!=value ){
				selectIndex= value;

				//リスナーコール
				if(this->selectListener!=NULL){
					(this->selectListener)();
				}

				callRewrite();
			}
		}
		int getSelectIndex() const{
			return selectIndex;
		}
		//	Accessor : selectListener
		void setSelectListener(std::function<void()> listener){
			selectListener= listener;
		}
	private:
		////////////////////////// MemberVar /////////////////////////////
		TCHAR *name;
		bool limitnamelen;
		int selectIndex;
		std::function<void()> selectListener;

		std::vector< std::basic_string<TCHAR> > nameList;

		static const int SIZE_Y= 15;
		static const int BOX_POS_X= 3;
		static const int BOX_WIDTH= 10;
	};

	///<summary>
	///	ウィンドウ内部品"スライダー"クラス
	///</summary>
	class OWP_Slider : public OrgWindowParts{
	public:
		//////////////////// Constructor/Destructor //////////////////////
		OWP_Slider( double _value=0.0, double _maxValue=1.0, double _minValue=0.0 ) : OrgWindowParts() {
			maxValue= max(_minValue,_maxValue);
			minValue= min(_minValue,_maxValue);
			value= max(min(_value,maxValue),minValue);

			drag=false;
			isslider = true;//!!!!!!
			//clickpos = WindowPos(0, 0);
			diffclickcenter = 0;//クリック位置の　ボタン中央からのずれ

			oldvaluevec.clear();
			oldvaluevec.push_back(value);
			oldvalueindex = 0;

			cursorListener = NULL;//2024/03/03
			lupListener = NULL;//2024/03/12
		}

		//////////////////////////// Method //////////////////////////////
		/// Method : 自動サイズ設定
		virtual void autoResize(){
			size.y= SIZE_Y;
		}
		//	Method : 描画
		virtual void draw(){
			if (!hdcM) {
				return;
			}

			drawEdge();


			int pos1x= pos.x+AXIS_POS_X;
			int pos1y= pos.y+size.y/2;
			int pos2x= pos.x+size.x-LABEL_SIZE_X;
			int pos2y= pos.y+size.y/2;
			//軸
			hdcM->setPenAndBrush(RGB(min(baseColor.r+20,255),min(baseColor.g+20,255),min(baseColor.b+20,255)),NULL);
			MoveToEx(hdcM->hDC, pos1x,pos1y, NULL);
			LineTo(hdcM->hDC,   pos2x,pos2y);
			MoveToEx(hdcM->hDC, (pos1x+pos2x)/2,pos2y+1, NULL);
			LineTo(hdcM->hDC,   (pos1x+pos2x)/2,pos2y-2);
			hdcM->setPenAndBrush(RGB(240,240,240),NULL);
			MoveToEx(hdcM->hDC, pos1x,pos1y+1, NULL);
			LineTo(hdcM->hDC,   pos1x,pos1y-2);
			MoveToEx(hdcM->hDC, pos2x,pos2y+1, NULL);
			LineTo(hdcM->hDC,   pos2x,pos2y-2);

			{//2023/10/03追加 CenterBar
				int col1 = 127;
				hdcM->setPenAndBrush(RGB(col1, col1, col1), RGB(col1, col1, col1));
				int BarWidth = 1;
				Rectangle(hdcM->hDC, pos1x, pos1y - BarWidth, pos2x, pos1y + BarWidth);
			}


			//ボタン
			int buttonPosX = pos1x + (int)( (value-minValue)*(float)(pos2x-pos1x)/(maxValue-minValue)+ 0.5f );
			//pos1x= buttonPosX-1;
			//pos1y= pos1y-AXIS_SIZE_Y/2;
			//pos2x= buttonPosX+1;
			//pos2y= pos1y+AXIS_SIZE_Y;
			//hdcM->setPenAndBrush(RGB(240,240,240),RGB(baseColor.r,baseColor.g,baseColor.b));
			//Rectangle(hdcM->hDC,pos1x,pos1y,pos2x+1,pos2y+1);
			//hdcM->setPenAndBrush(RGB(min(baseColor.r+20,255),min(baseColor.g+20,255),min(baseColor.b+20,255)),NULL);
			//MoveToEx(hdcM->hDC, pos1x,pos1y+2, NULL);
			//LineTo(hdcM->hDC,   pos1x,pos2y-1);
			//MoveToEx(hdcM->hDC, pos2x,pos1y+2, NULL);
			//LineTo(hdcM->hDC,   pos2x,pos2y-1);

			{//2023/10/03
				int col0 = 240;
				int col1 = 127;
				hdcM->setPenAndBrush(RGB(col1, col1, col1), RGB(col1, col1, col1));
				Ellipse(hdcM->hDC, buttonPosX - THUMB_SIZE, pos1y - THUMB_SIZE, buttonPosX + THUMB_SIZE, pos1y + THUMB_SIZE);
			}


			//ラベル
			pos1x= pos.x+size.x-LABEL_SIZE_X+3;
			pos1y= pos.y+size.y/2-5;
			pos2x= pos.x+size.x-4;
			pos2y= pos.y+size.y/2+5;
			hdcM->setPenAndBrush(RGB(min(baseColor.r+20,255),min(baseColor.g+20,255),min(baseColor.b+20,255)),NULL);
			MoveToEx(hdcM->hDC, pos1x,pos1y, NULL);
			LineTo(hdcM->hDC,   pos2x+1,pos1y);
			MoveToEx(hdcM->hDC, pos1x,pos2y, NULL);
			LineTo(hdcM->hDC,   pos2x+1,pos2y);
			hdcM->setFont(12,_T("ＭＳ ゴシック"));
			SetTextColor(hdcM->hDC,RGB(240,240,240));
			TCHAR tmpChar[20];
			_stprintf_s(tmpChar,20,_T("% 9.3G"),value);
			TextOut( hdcM->hDC,
					 pos1x, pos1y,
					 tmpChar, (int)_tcslen(tmpChar));
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
		//	Method : マウスダウンイベント受信
		virtual void onLButtonDown(const MouseEvent& e){
			if ((g_endappflag == 0) && parentWindow && IsWindow(parentWindow->getHWnd())) {

				WindowPos tmpPos = WindowPos(e.localX, e.localY) - WindowPos(AXIS_POS_X, size.y / 2);
				const int EDGE_WIDTH = 4;
				if ((-EDGE_WIDTH <= tmpPos.x) && (tmpPos.x <= (size.x - LABEL_SIZE_X + EDGE_WIDTH)) &&
					//((-EDGE_WIDTH - AXIS_SIZE_Y / 2) <= tmpPos.y) && 
					//(tmpPos.y <= (size.y + EDGE_WIDTH + AXIS_SIZE_Y / 2))) {
					((-EDGE_WIDTH - THUMB_SIZE) <= tmpPos.y) &&
					(tmpPos.y <= (size.y + EDGE_WIDTH + THUMB_SIZE))) {

					////2023/10/03  Thumbの真ん中以外をクリックしたときに　スライダーを動かさないために　コメントアウト
					//setValue(minValue + (maxValue - minValue) * (float)tmpPos.x / (float)(size.x - AXIS_POS_X - LABEL_SIZE_X));

					{//2023/10/03
							//int pos1x = pos.x + AXIS_POS_X;
							//int pos2x = pos.x + size.x - LABEL_SIZE_X;
							int pos1x = pos.x + AXIS_POS_X;
							int pos2x = pos.x + size.x - AXIS_POS_X - LABEL_SIZE_X;
							//int buttonCenter = pos1x + (int)((value - minValue) * (float)(pos2x - pos1x) / (maxValue - minValue) + 0.5f);
							int buttonCenter = (int)((value - minValue) * (float)(pos2x - pos1x) / (maxValue - minValue) + 0.5f);
							diffclickcenter = e.localX - buttonCenter;//クリック位置の　ボタン中央からのずれ
					}

					drag = true;

					RECT tmpRect;
					tmpRect.left = pos.x + 1;
					tmpRect.top = pos.y + 1;
					tmpRect.right = pos.x + size.x - 1;
					tmpRect.bottom = pos.y + size.y - 1;
					InvalidateRect(parentWindow->getHWnd(), &tmpRect, false);

				}
			}
		}
		//	Method : 左マウスボタン ダブルクリックイベント受信
		virtual void onLButtonDBLCLK(const MouseEvent& e) {//2023/10/04

			//スライダー　センターバー　左ダブルクリック　：　クリック位置にThumbButtonを移動

			if ((g_endappflag == 0) && parentWindow && IsWindow(parentWindow->getHWnd())) {

				WindowPos tmpPos = WindowPos(e.localX, e.localY) - WindowPos(AXIS_POS_X, size.y / 2);
				const int EDGE_WIDTH = 4;
				if ((-EDGE_WIDTH <= tmpPos.x) && (tmpPos.x <= (size.x - LABEL_SIZE_X + EDGE_WIDTH)) &&
					//((-EDGE_WIDTH - AXIS_SIZE_Y / 2) <= tmpPos.y) && 
					//(tmpPos.y <= (size.y + EDGE_WIDTH + AXIS_SIZE_Y / 2))) {
					((-EDGE_WIDTH - THUMB_SIZE) <= tmpPos.y) &&
					(tmpPos.y <= (size.y + EDGE_WIDTH + THUMB_SIZE))) {


					//クリック位置にThumbButtonを移動 (クリック位置に対応するvalue値を設定)
					setValue(minValue + (maxValue - minValue) * (float)tmpPos.x / (float)(size.x - AXIS_POS_X - LABEL_SIZE_X));
					saveUndoValue(value);

					RECT tmpRect;
					tmpRect.left = pos.x + 1;
					tmpRect.top = pos.y + 1;
					tmpRect.right = pos.x + size.x - 1;
					tmpRect.bottom = pos.y + size.y - 1;
					InvalidateRect(parentWindow->getHWnd(), &tmpRect, false);
				}
			}
		}
		//	Method : 右マウスボタン ダブルクリックイベント受信
		virtual void onRButtonDBLCLK(const MouseEvent& e) {//2023/10/04

			//スライダー　センターバー　右ダブルクリック　：　Value値をアンドゥ

			if ((g_endappflag == 0) && parentWindow && IsWindow(parentWindow->getHWnd())) {

				WindowPos tmpPos = WindowPos(e.localX, e.localY) - WindowPos(AXIS_POS_X, size.y / 2);
				const int EDGE_WIDTH = 4;
				if ((-EDGE_WIDTH <= tmpPos.x) && (tmpPos.x <= (size.x - LABEL_SIZE_X + EDGE_WIDTH)) &&
					//((-EDGE_WIDTH - AXIS_SIZE_Y / 2) <= tmpPos.y) && 
					//(tmpPos.y <= (size.y + EDGE_WIDTH + AXIS_SIZE_Y / 2))) {
					((-EDGE_WIDTH - THUMB_SIZE) <= tmpPos.y) &&
					(tmpPos.y <= (size.y + EDGE_WIDTH + THUMB_SIZE))) {

					double oldvalue = getUndoValue();
					if (oldvalue != value) {
						setValue(oldvalue);

						RECT tmpRect;
						tmpRect.left = pos.x + 1;
						tmpRect.top = pos.y + 1;
						tmpRect.right = pos.x + size.x - 1;
						tmpRect.bottom = pos.y + size.y - 1;
						InvalidateRect(parentWindow->getHWnd(), &tmpRect, false);
					}
				}
			}
		}

		//	Method : 左マウスボタンアップイベント受信
		virtual void onLButtonUp(const MouseEvent& e){
			drag=false;
			saveUndoValue(value);

			//if (!listenmouse) {
			//	return;//!!!!!!!!!!!!!!!!
			//}
			if (this->lupListener != NULL) {
				(this->lupListener)();//2024/03/12 for undo
			}
		}
		//	Method : マウス移動イベント受信
		virtual void onMouseMove(const MouseEvent& e){
			if ((g_endappflag == 0) && parentWindow && IsWindow(parentWindow->getHWnd())) {

				WindowPos tmpPos = WindowPos(e.localX, e.localY) - WindowPos(AXIS_POS_X, size.y / 2);
				if (drag) {

					//setValue(minValue + (maxValue - minValue) * (float)tmpPos.x / (float)(size.x - AXIS_POS_X - LABEL_SIZE_X));

					{//2023/10/03 ボタンのクリック位置を起点にして　ボタンを動かす
						setValue(minValue + (maxValue - minValue) * (float)(tmpPos.x - diffclickcenter) / (float)(size.x - AXIS_POS_X - LABEL_SIZE_X));
					}


					RECT tmpRect;
					tmpRect.left = pos.x + 1;
					tmpRect.top = pos.y + 1;
					tmpRect.right = pos.x + size.x - 1;
					tmpRect.bottom = pos.y + size.y - 1;
					InvalidateRect(parentWindow->getHWnd(), &tmpRect, false);

				}
			}
		}

		virtual void onMouseWheel(const MouseEvent& e) {//2023/10/03

			//if ((e.localX >= pos.x) && (e.localX <= (pos.x + size.x)) && 
			//	(e.localY >= pos.y) && (e.localY <= (pos.y + size.y))) {

			//onMouseWheelのe.localX, localYは pos.x, pos.yを引いてから渡されてくる
			bool undercapturing = false;//SetCaptureはparentwindowに対して行われる　キャプチャーするべきsliderかどうかの印
			if ((e.localX >= 0) && (e.localX <= size.x) && 
				(e.localY >= 0) && (e.localY <= size.y)) {
				undercapturing = true;
			}
			else {
				undercapturing = false;
			}

			if (undercapturing) {
				int wheeldelta = e.wheeldelta;
				double pixdelta = 0.0;
				if (wheeldelta > 0) {
					pixdelta = 1.0;
				}
				else if (wheeldelta < 0) {
					pixdelta = -1.0;
				}
				else {
					pixdelta = 0.0;
				}

				int pos1x = pos.x + AXIS_POS_X;
				int pos2x = pos.x + size.x - LABEL_SIZE_X;
				//double buttonCenter = pixdelta + pos1x + (value - minValue) * (double)(pos2x - pos1x) / (maxValue - minValue) + 0.5;
				double buttonCenter = pixdelta + (value - minValue) * (double)(pos2x - pos1x) / (maxValue - minValue);

				double newvalue = minValue + (maxValue - minValue) * buttonCenter / (double)(size.x - AXIS_POS_X - LABEL_SIZE_X);
				setValue(newvalue);
			}
		}



		/////////////////////////// Accessor /////////////////////////////
		//	Accessor : maxValue
		double getMaxValue() const{
			return maxValue;
		}
		void setMaxValue(double _maxValue){
			maxValue= max(_maxValue,minValue);
			value= min(value,maxValue);
		}
		//	Accessor : minValue
		double getMinValue() const{
			return minValue;
		}
		void setMinValue(double _minValue){
			minValue= min(_minValue,maxValue);
			value= max(value,minValue);
		}
		//	Accessor : value
		double getValue() const{
			return value;
		}
		void setValue(double _value, bool calllistener = true){
			value= min(max(_value,minValue),maxValue);

			//リスナーコール
			if(this->cursorListener!=NULL){
				(this->cursorListener)();
			}
		}
		void saveUndoValue(double savevalue)
		{
			//アンドゥ用に保存する
	
			if (!oldvaluevec.empty()) {
				size_t chkvecsize = oldvaluevec.size();
				double chkvalue = oldvaluevec[chkvecsize - 1];
				if (chkvalue == savevalue) {
					//前回の値と同じ場合には保存しない
					return;
				}
			}

			size_t chkvecsize2 = oldvaluevec.size();
			if (chkvecsize2 > 1000000) {//プログラムのシンプルさのために　保存数上限を越えたら空にする
				_ASSERT(0);
				oldvaluevec.clear();
			}
			oldvaluevec.push_back(value);
			oldvalueindex = oldvaluevec.size() - 1;
		}
		double getUndoValue()
		{
			if (!oldvaluevec.empty()) {
				size_t vecsize = oldvaluevec.size();
				size_t valueindex;
				if (oldvalueindex >= 1) {
					valueindex = oldvalueindex - 1;
				}
				else {
					valueindex = vecsize - 1;
				}
				if (valueindex < vecsize) {
					double oldvalue = oldvaluevec[valueindex];
					oldvalueindex = valueindex;
					return oldvalue;//!!!!!!!!!
				}
				else {
					return value;
				}
			}
			else {
				return value;
			}
		}

		//	Accessor : cursorListener
		void setCursorListener(std::function<void()> listener){
			this->cursorListener= listener;
		}
		void setLUpListener(std::function<void()> listener) {
			this->lupListener = listener;
		}

	private:
		////////////////////////// MemberVar /////////////////////////////
		double maxValue,minValue,value;
		std::function<void()> cursorListener;
		std::function<void()> lupListener;

		bool drag;

		static const int SIZE_Y= 20;
		static const int LABEL_SIZE_X= 65;
		static const int AXIS_POS_X= 5;
		static const int AXIS_SIZE_Y= 10;
		static const int THUMB_SIZE = 5;

		int diffclickcenter;// ボタンのクリック位置を起点にして　ボタンを動かす

		std::vector<double> oldvaluevec;
		size_t oldvalueindex;
	};

	///<summary>
	///	ウィンドウ内部品"タイムラインテーブル"クラス
	///</summary>
	class OWP_Timeline : public OrgWindowParts{
	public:
		//////////////////// Constructor/Destructor //////////////////////
		OWP_Timeline(const bool _heightwheel, const bool srcshortlabel, const std::basic_string<TCHAR> &_name=_T(""), const double &_maxTime=1.0, const double &_timeSize=8.0 ) : OrgWindowParts() {
			
			TIME_ERROR_WIDTH = 0.0001;

			lineData.push_back(new LineData(0,0,false,false,_name,this,0));
			maxTime= _maxTime;
			timeSize= _timeSize;
			//cursorListener = [](){s_dummyfunc();};
			//selectListener = [](){s_dummyfunc();};
			//mouseMDownListener = [](){s_dummyfunc();};
			//mouseWheelListener = [](){s_dummyfunc();};
			//mouseRUpListener = [](){s_dummyfunc();};
			cursorListener = NULL;
			selectListener = NULL;
			mouseMDownListener = NULL;
			mouseWheelListener = NULL;
			mouseRUpListener = NULL;

			//keyShiftListener = [this](){
			//	shiftKeyTime(getShiftKeyTime());
			//};
			////keyDeleteListener = [](const KeyInfo& dummy){s_dummyfunc();};
			keyShiftListener = NULL;
			keyDeleteListener = NULL;

			showPos_time=0;
			if (g_4kresolution) {
				showPos_width = 280.0;
			}
			else {
				//showPos_width = 76.0;
				showPos_width = 93.0;//2023/01/08
			}
			showPos_line=0;
			currentTime=0;
			currentLine=0;

			allowSameTimeKey=false;

			timeSnapSize=1.0;

			rewriteOnChange= true;
			canMouseControll= true;

			dragLabel= false;
			dragTime= false;
			dragScrollBarLabel= false;
			dragScrollBarTime= false;
			dragSelect= false;
			dragShift= false;
			wheeldelta = 0;

			dispkeyflag = false;

			shortlabel = srcshortlabel;
			if (shortlabel) {
				LABEL_SIZE_X = 135;
			}
			else {
				//LABEL_SIZE_X = 280;
				LABEL_SIZE_X = 600 - SCROLL_BAR_WIDTH - 20;
			}
			if (g_4kresolution) {
				LABEL_SIZE_Y = 22;
			}
			else {
				LABEL_SIZE_Y = 15;
			}

			heightwheel = _heightwheel;//高さ方向にホイールスクロールするかどうか

		}
		~OWP_Timeline(){
			selectAll(true);
			deleteKey();

			std::vector<LineData*>::iterator itr;
			for(itr = lineData.begin(); itr != lineData.end(); itr++){
				delete (*itr);
			}
		}


		//static void OWP_Timeline::InitKeys();
		//static void OWP_Timeline::DestroyKeys();


		//////////////////////////// Method //////////////////////////////
		/// Method : 自動サイズ設定
		virtual void autoResize(){
			size.y-= (size.y-SCROLL_BAR_WIDTH-AXIS_SIZE_Y-MARGIN*2)%(LABEL_SIZE_Y-1);
		}
		WindowPos getCurrentLinePos()
		{
			WindowPos retpos;
			int i = currentLine;
			int j = currentLine - showPos_line;
			retpos.x = pos.x + MARGIN + (size.x - SCROLL_BAR_WIDTH - MARGIN * 2) / 2;
			retpos.y = pos.y + MARGIN + AXIS_SIZE_Y + j * (LABEL_SIZE_Y - 1) + LABEL_SIZE_Y / 2;

			////行データ
			//int showLineNum = (size.y - SCROLL_BAR_WIDTH - AXIS_SIZE_Y - MARGIN * 2) / (LABEL_SIZE_Y - 1);
			//for (int i = showPos_line, j = 0; i < (int)lineData.size() && j < showLineNum; i++, j++) {
			//	bool highLight = false;
			//	if (i == currentLine) highLight = true;
			//	if (i >= 0) {
			//		lineData[i]->draw(hdcM,
			//			pos.x + MARGIN,
			//			pos.y + MARGIN + AXIS_SIZE_Y + j * (LABEL_SIZE_Y - 1),
			//			size.x - SCROLL_BAR_WIDTH - MARGIN * 2,
			//			timeSize, showPos_time, highLight);
			//	}
			//}

			return retpos;
		}
		//	Method : 描画
		virtual void callRewrite();
		virtual void draw();
		//void draw(){
		//	drawEdge();

		//	//時間軸目盛り
		//	{
		//		const int AXIS_CURSOR_SIZE=4;
		//		const int AXIS_LABEL_SIDE_MARGIN=7;

		//		//目盛り線 & ラベル
		//		int x0= pos.x+MARGIN+LABEL_SIZE_X;
		//		int x1= pos.x+size.x-MARGIN-SCROLL_BAR_WIDTH;
		//		int y0= pos.y+MARGIN;
		//		int y1= y0+AXIS_SIZE_Y+1;
		//		for(int i=(int)showPos_time; i<=(int)maxTime; i++){
		//			int xx= (int)(((double)i-showPos_time)*timeSize) + x0 + 1;

		//			if( x1+AXIS_LABEL_SIDE_MARGIN <= xx ) break;
		//			if( x0-AXIS_LABEL_SIDE_MARGIN <= xx ){
		//				hdcM->setPenAndBrush(RGB(min(baseColor.r+20,255),min(baseColor.g+20,255),min(baseColor.b+20,255)),NULL);
		//				MoveToEx(hdcM->hDC, xx,y1-5, NULL);
		//				LineTo(hdcM->hDC,   xx,y1);

		//				if (((i < 1000) && (i % 5 == 0)) || ((i >= 1000) && (i % 10 == 0))){
		//					TCHAR tmpChar[20];
		//					_stprintf_s(tmpChar, 20, _T("%.3G"), (double)i);
		//					hdcM->setFont(12, _T("ＭＳ ゴシック"));
		//					SetTextColor(hdcM->hDC, RGB(240, 240, 240));
		//					TextOut(hdcM->hDC,
		//						xx - (int)((double)_tcslen(tmpChar)*2.0), y0,
		//						tmpChar, (int)_tcslen(tmpChar));
		//				}
		//			}
		//		}

		//		//カーソル
		//		int xx= (int)((currentTime-showPos_time)*timeSize) + x0 + 1;
		//		hdcM->setPenAndBrush(RGB(240,240,240),NULL);
		//		if( x0-AXIS_CURSOR_SIZE <= xx && xx <= x1+AXIS_CURSOR_SIZE ){
		//			for(int i=0; i<AXIS_CURSOR_SIZE; i++){
		//				MoveToEx(hdcM->hDC, xx-i,y1-i-2, NULL);
		//				LineTo(hdcM->hDC,   xx+i+1,y1-i-2);
		//			}
		//		}

		//		//枠
		//		hdcM->setPenAndBrush(NULL,RGB(baseColor.r,baseColor.g,baseColor.b));
		//		Rectangle(hdcM->hDC,pos.x,y0,x0,y1);
		//		Rectangle(hdcM->hDC,x1,y0,pos.x+size.x,y1);
		//		hdcM->setPenAndBrush(RGB(min(baseColor.r+20,255),min(baseColor.g+20,255),min(baseColor.b+20,255)),NULL);
		//		Rectangle(hdcM->hDC,x0,y0,x1,y1);
		//	}
		//	
		//	drawEdge(false);

		//	//行データ
		//	int showLineNum= (size.y-SCROLL_BAR_WIDTH-AXIS_SIZE_Y-MARGIN*2)/(LABEL_SIZE_Y-1);
		//	for(int i=showPos_line,j=0; i<(int)lineData.size() && j<showLineNum; i++,j++){
		//		bool highLight=false;
		//		if( i==currentLine ) highLight=true;
		//		if( i >= 0 ){
		//			lineData[i]->draw(	hdcM,
		//								pos.x+MARGIN,
		//								pos.y+MARGIN+AXIS_SIZE_Y+j*(LABEL_SIZE_Y-1),
		//								size.x-SCROLL_BAR_WIDTH-MARGIN*2,
		//								timeSize,showPos_time,highLight );
		//		}
		//	}

		//	//ドラッグによる選択範囲
		//	if( dragSelect && dragSelectTime1!=dragSelectTime2 ){
		//		int xx0= pos.x+MARGIN+LABEL_SIZE_X+ 1;
		//		int yy0= pos.y+MARGIN+AXIS_SIZE_Y;
		//		int xx1= pos.x+size.x-MARGIN-SCROLL_BAR_WIDTH- 1;
		//		int yy1= pos.y+size.y-MARGIN-SCROLL_BAR_WIDTH;
		//		int x0= xx0+ (int)((min(dragSelectTime1,dragSelectTime2)- showPos_time)* timeSize);
		//		int x1= xx0+ (int)((max(dragSelectTime1,dragSelectTime2)- showPos_time)* timeSize);
		//		int y0= yy0+ (min(dragSelectLine1,dragSelectLine2)- showPos_line)* (LABEL_SIZE_Y-1)+ 1;
		//		int y1= yy0+ (max(dragSelectLine1,dragSelectLine2)- showPos_line+ 1)* (LABEL_SIZE_Y-1)- 1;

		//		{//枠描画
		//			hdcM->setPenAndBrush(RGB(min(baseColor.r+20,255),min(baseColor.g+20,255),min(baseColor.b+20,255)),NULL);
		//			if( xx0<=x0 ){		//左枠
		//				MoveToEx(hdcM->hDC, x0, max(yy0,y0+1), NULL);
		//				LineTo(hdcM->hDC,   x0, min(y1,yy1));
		//			}
		//			if( x1<=xx1 ){		//右枠
		//				MoveToEx(hdcM->hDC, x1, max(yy0,y0+1), NULL);
		//				LineTo(hdcM->hDC,   x1, min(y1,yy1));
		//			}
		//			if( yy0<=y0 ){		//上枠
		//				MoveToEx(hdcM->hDC, max(xx0,x0+1), y0, NULL);
		//				LineTo(hdcM->hDC,   min(x1,xx1), y0);
		//			}
		//			if( y1<=yy1 ){		//下枠
		//				MoveToEx(hdcM->hDC, max(xx0,x0+1), y1, NULL);
		//				LineTo(hdcM->hDC,   min(x1,xx1), y1);
		//			}
		//		}
		//	}

		//	//時間軸スクロールバー
		//	{
		//		int x0= pos.x+MARGIN+LABEL_SIZE_X;
		//		int x1= pos.x+size.x-MARGIN-SCROLL_BAR_WIDTH;
		//		int y0= pos.y+size.y-MARGIN-SCROLL_BAR_WIDTH;
		//		int y1= y0+SCROLL_BAR_WIDTH;

		//		//枠
		//		hdcM->setPenAndBrush(RGB(min(baseColor.r+20,255),min(baseColor.g+20,255),min(baseColor.b+20,255)),NULL);
		//		Rectangle(hdcM->hDC,x0,y0,x1,y1);

		//		//中身
		//		double showTimeLength= ((double)(x1-x0-3))/timeSize;
		//		double barSize= ((double)(x1-x0-4))*showTimeLength/maxTime;
		//		double barStart= ((double)(x1-x0-4))*showPos_time/maxTime;
		//		if( showTimeLength<maxTime ){
		//			hdcM->setPenAndBrush(NULL,RGB(min(baseColor.r+20,255),min(baseColor.g+20,255),min(baseColor.b+20,255)));
		//			Rectangle(hdcM->hDC,x0+2+(int)barStart,y0+2,x0+2+(int)(barStart+barSize),y1-2);
		//		}
		//	}

		//	//ラベルスクロールバー
		//	{
		//		int x0= pos.x+size.x-MARGIN-SCROLL_BAR_WIDTH-1;
		//		int x1= x0+SCROLL_BAR_WIDTH+1;
		//		int y0= pos.y+MARGIN+AXIS_SIZE_Y;
		//		int y1= pos.y+size.y-MARGIN-SCROLL_BAR_WIDTH+1;

		//		//枠
		//		hdcM->setPenAndBrush(RGB(min(baseColor.r+20,255),min(baseColor.g+20,255),min(baseColor.b+20,255)),NULL);
		//		Rectangle(hdcM->hDC,x0,y0,x1,y1);

		//		//中身
		//		int barSize= (y1-y0-4)*showLineNum/(int)lineData.size();
		//		int barStart= (y1-y0-4)*showPos_line/(int)lineData.size();
		//		if( showLineNum<(int)lineData.size() ){
		//			hdcM->setPenAndBrush(NULL,RGB(min(baseColor.r+20,255),min(baseColor.g+20,255),min(baseColor.b+20,255)));
		//			Rectangle(hdcM->hDC,x0+2,y0+2+barStart,x1-2,y0+2+barStart+barSize+1);
		//		}
		//	}
		//}
		

		//	Method : 行を追加	(既に同名のキーがある場合はFalseを返す)
		bool newLine(int _depth, int nullflag, 
			bool srcikstopflag, bool srcconstraintflag,
			const std::basic_string<TCHAR>& _name, 
			COLORREF srctextcol = RGB(255, 255, 255)){
			
			for(int i=0; i<(int)lineData.size(); i++){
				if(lineData[i]->name==_name){
					return false;
				}
			}
			lineData.push_back(new LineData(_depth, nullflag, 
				srcikstopflag, srcconstraintflag,
				_name, this, (int)lineData.size(), srctextcol));

			//再描画要求
			if( rewriteOnChange ){
				callRewrite();
			}
			return true;
		}
		//	Method : 行を削除
		void deleteLine(){
			for(int i=0; i<(int)lineData.size(); i++){
				delete lineData[i];
			}
			lineData.clear();

			//再描画要求
			if( rewriteOnChange ){
				callRewrite();
			}
		}
		bool deleteLine(const std::basic_string<TCHAR>& _name){
			int popPos=-1;
			for(int i=0; i<(int)lineData.size(); i++){
				if(lineData[i]->name==_name){
					delete lineData[i];
					popPos=i;
				}
			}
			if(popPos==-1) return false;
			for(int i=popPos+1; i<(int)lineData.size(); i++){
				lineData[i-1]=lineData[i];
				lineData[i-1]->lineIndex= i-1;
			}
			lineData.pop_back();

			//再描画要求
			if( rewriteOnChange ){
				callRewrite();
			}
			return true;
		}
		bool deleteLine(int index){
			if((unsigned int)lineData.size()<=(unsigned int)index) return false;

			bool ret= deleteLine(lineData[index]->name);

			//再描画要求
			if( ret && rewriteOnChange ){
				callRewrite();
			}
			return ret;
		}
		//	Method : キーを追加
		bool newKey(const std::basic_string<TCHAR>& _name, const double &time, void* object=NULL, const double &length=1.0){
			for(int i=0; i<(int)lineData.size(); i++){
				if(lineData[i]->name==_name){
					bool ret= lineData[i]->newKey(time,0,object,length);

					//再描画要求
					if( ret && rewriteOnChange ){
						callRewrite();
					}
					return ret;
				}
			}
			return false;
		}
		bool setHasRigFlag(const std::basic_string<TCHAR>& _name, bool flag) {
			for (int i = 0; i < (int)lineData.size(); i++) {
				if (lineData[i]->name == _name) {
					bool ret = lineData[i]->setHasRigFlag(flag);

					//再描画要求
					if (ret && rewriteOnChange) {
						callRewrite();
					}
					return ret;
				}
			}
			return false;

		}


		////	Method : キーを削除
		//bool deleteKey(const std::basic_string<TCHAR>& _name, double time){
		//	for(int i=0; i<(int)lineData.size(); i++){
		//		if(lineData[i]->name==_name){
		//			selectClear();
		//			bool ret= lineData[i]->selectKey(time);
		//			lineData[i]->deleteKey();

		//			//再描画要求
		//			if( ret && rewriteOnChange ){
		//				callRewrite();
		//			}
		//			return ret;
		//		}
		//	}
		//	return false;
		//}
		//bool deleteKey(const std::basic_string<TCHAR>& _name, int index){
		//	for(int i=0; i<(int)lineData.size(); i++){
		//		if(lineData[i]->name==_name){
		//			bool ret= lineData[i]->deleteKey(index);

		//			//再描画要求
		//			if( ret && rewriteOnChange ){
		//				callRewrite();
		//			}
		//			return ret;
		//		}
		//	}
		//	return false;
		//}
		//bool deleteKey(int lineIndex, double time){
		//	if((unsigned int)lineData.size()<=(unsigned int)lineIndex) return false;

		//	selectClear();
		//	bool ret= lineData[lineIndex]->selectKey(time);
		//	lineData[lineIndex]->deleteKey();

		//	//再描画要求
		//	if( ret && rewriteOnChange ){
		//		callRewrite();
		//	}

		//	return ret;
		//}
		//bool deleteKey(int lineIndex, int keyIndex){
		//	if((unsigned int)lineData.size()<=(unsigned int)lineIndex) return false;

		//	bool ret= lineData[lineIndex]->deleteKey(keyIndex);

		//	//再描画要求
		//	if( ret && rewriteOnChange ){
		//		callRewrite();
		//	}

		//	return ret;
		//}
		unsigned int deleteKey(){
			unsigned int deleteNum= 0;

			for(int i=0; i<(int)lineData.size(); i++){
				deleteNum+= lineData[i]->deleteKey();
			}

			//再描画要求
			if( (deleteNum != 0) && rewriteOnChange ){
				callRewrite();
			}

			return deleteNum;
		}
		//	Method : すべてのキーの選択を解除する
		void selectClear(bool noCallListener= false){
			for(int i=0; i<(int)lineData.size(); i++){
				lineData[i]->selectClear();
			}

			//リスナーコール
			if(!noCallListener && (this->selectListener != NULL)){
				(this->selectListener)();
			}

			//再描画要求
			if( rewriteOnChange ){
				callRewrite();
			}
		}
		//	Method : すべてのキーを選択する
		void selectAll(bool noCallListener= false){
			for(int i=0; i<(int)lineData.size(); i++){
				lineData[i]->selectAll();
			}

			//リスナーコール
			if(!noCallListener && (this->selectListener != NULL)){
				(this->selectListener)();
			}

			//再描画要求
			if( rewriteOnChange ){
				callRewrite();
			}
		}

		//MaxSelectionFrameを返す
		double OnButtonSelect(double startframe, double endframe, int tothelastflag)
		{
			if ((g_endappflag == 0) && parentWindow && IsWindow(parentWindow->getHWnd())) {
				this->selectClear();

				double tmpstart, tmpend;
				if (tothelastflag == 0) {
					if (startframe <= endframe) {
						tmpstart = startframe;
						tmpend = endframe;
					}
					else {
						tmpstart = endframe;
						tmpend = startframe;
					}
				}
				else {
					tmpstart = startframe;
					tmpend = startframe;
				}

				//double curframe = getCurrentTime();
				double maxframe = startframe;

				//for (int j = 0; j < (int)lineData.size(); j++){
				int j = 1;
				if (j < (int)lineData.size()) {//lineのインデックス
					LineData* curLineData = lineData[j];
					if (curLineData) {
						if (tothelastflag == 1) {

							//maxframe = (double)(curLineData->key.size() - 1);

							int startindex = curLineData->getKeyIndex(tmpstart);
							if (startindex >= 0) {
								int currentkeynum = (int)curLineData->key.size();
								for (int i = startindex; i < currentkeynum; i++) {//keyのインデックス
									if (curLineData->key[i]->time >= (tmpstart - TIME_ERROR_WIDTH)) {
										curLineData->key[i]->select = true;
										if (maxframe < curLineData->key[i]->time) {
											maxframe = curLineData->key[i]->time;
										}
									}
								}
							}
							curLineData->setMinSelected(startindex);
							curLineData->setMaxSelected(max(0, (int)curLineData->key.size() - 1));

							//for (int i = 0; i < (int)curLineData->key.size(); i++) {
							//	if (curLineData->key[i]->time >= (tmpstart - TIME_ERROR_WIDTH)) {
							//		curLineData->key[i]->select = true;
							//		if (maxframe < curLineData->key[i]->time) {
							//			maxframe = curLineData->key[i]->time;
							//		}
							//	}
							//}
						}
						else {

							//maxframe = tmpend;

							int startindex = curLineData->getKeyIndex(tmpstart);
							int endindex = curLineData->getKeyIndex(tmpend);
							if ((startindex >= 0) && (endindex >= 0)) {
								for (int i = startindex; i <= endindex; i++) {//keyのインデックス
									if ((curLineData->key[i]->time >= (tmpstart - TIME_ERROR_WIDTH)) &&
										(curLineData->key[i]->time <= (tmpend + TIME_ERROR_WIDTH))) {
										curLineData->key[i]->select = true;
										if (maxframe < curLineData->key[i]->time) {
											maxframe = curLineData->key[i]->time;
										}
									}
								}
							}
							curLineData->setMinSelected(startindex);
							curLineData->setMaxSelected(endindex);


							//for (int i = 0; i < (int)curLineData->key.size(); i++) {
							//	if ((curLineData->key[i]->time >= (tmpstart - TIME_ERROR_WIDTH)) &&
							//		(curLineData->key[i]->time <= (tmpend + TIME_ERROR_WIDTH))) {
							//		curLineData->key[i]->select = true;
							//		if (maxframe < curLineData->key[i]->time) {
							//			maxframe = curLineData->key[i]->time;
							//		}
							//	}
							//}
						}
					}
				}


				//dragSelect = true;
				dragSelectTime1 = tmpstart;
				dragSelectTime2 = maxframe;
				//showPos_time = tmpstart;
				showPos_line = 0;
				currentLine = 1;//!!!!!!!

				//再描画領域
				RECT tmpRect;
				tmpRect.left = pos.x + 1;
				tmpRect.top = pos.y + 1;
				tmpRect.right = pos.x + size.x - 1;
				tmpRect.bottom = pos.y + size.y - 1;
				InvalidateRect(parentWindow->getHWnd(), &tmpRect, false);

				//再描画要求
				//if (rewriteOnChange){
				callRewrite();
				//}

				return maxframe;
			}
			else {
				return 1.0;
			}
		}

		/// Method : すべての選択されているキーを取得する
		std::list<KeyInfo> getSelectedKey() const{
			std::list<KeyInfo> ret;
			ret.clear();

			for(int i=0; i<(int)lineData.size(); i++){
				LineData *curLineData= lineData[i];
				if (curLineData) {
					int curkeynum = (int)curLineData->key.size();

					//for(int j=0; j<curkeynum; j++){
					int startindex = max(curLineData->getMinSelected(), 0);
					int endindex = min((curLineData->getMaxSelected() + 1), curkeynum);
					if ((startindex >= 0) && (endindex >= 0)) {
						for (int j = startindex; j < endindex; j++) {//2022/09/12
							LineData::Key* curKey = curLineData->key[j];

							if (curKey->select) {
								KeyInfo tmp;
								tmp.label = curLineData->name.c_str();
								tmp.time = curKey->time;
								tmp.lineIndex = i;
								tmp.timeIndex = j;
								tmp.object = curKey->object;
								ret.push_back(tmp);
							}
						}
					}
				}
			}

			//if ((ret.size() == 1) && ((*ret.begin()).time != currentTime)) {
			//	//初期状態。セレクトではない。
			//	(lineData[(*ret.begin()).lineIndex])->key[(*ret.begin()).timeIndex]->select = false;
			//	ret.clear();
			//}

//_ASSERT(0);
			return ret;
		}
		///// Method : 全ての選択されているキーを移動する
		//void shiftKeyTime(const double &shiftTime){
		//	for(int i=0; i<(int)lineData.size(); i++){
		//		lineData[i]->shiftKey(shiftTime);
		//	}

		//	//再描画要求
		//	if( rewriteOnChange ){
		//		callRewrite();
		//	}
		//}



		void WheelShowPosTime()
		{
			//int xx0 = MARGIN + LABEL_SIZE_X;
			//int xx1 = size.x - MARGIN - SCROLL_BAR_WIDTH;
			//double showTimeLength = ((double)(xx1 - xx0 - 3)) / timeSize;
			//if (showTimeLength < maxTime) {
				//double barSize = ((double)(xx1 - xx0 - 4)) * showTimeLength / maxTime;
				//int movableX = xx1 - xx0 - (int)barSize;
				//int movableXStart = xx0 + (int)barSize / 2;
				//double showTimeLength = ((double)(x1 - x0 - 3)) / timeSize;
				//if (showTimeLength < maxTime) {
				//	showPos_time = max(0, min(_showPosTime, maxTime - showTimeLength));
				//}
				//else {
				//	showPos_time = 0;
				//}
				double newshowpostime;
				if (wheeldelta > 0) {
					newshowpostime = min((showPos_time + 1.0), maxTime);
				}
				else {
					newshowpostime = max(0, (showPos_time - 1.0));
				}
				showPos_time = newshowpostime;
				
				for (int i = 0; i < (int)lineData.size(); i++) {
					lineData[i]->selectKey(showPos_time);
				}


				//再描画要求
				//if (rewriteOnChange) {
					callRewrite();
				//}

				//setShowPosTime(((double)(e.localX - movableXStart)) * (maxTime - showTimeLength) / (double)movableX);
				//dragScrollBarTime = true;
			//}
		}




		///	Method : マウスダウンイベント受信
		virtual void onLButtonDown(const MouseEvent& e){
			selectClear(true);

			if( !canMouseControll ) return;
			if (g_underselecttolast) return;
			if (g_undereditrange) return;

			int x0= MARGIN;
			int x1= x0+LABEL_SIZE_X;
			int x2= size.x-MARGIN-SCROLL_BAR_WIDTH;
			int x3= size.x-MARGIN;
			int y0= MARGIN;
			int y1= y0+AXIS_SIZE_Y+1;
			int y2= size.y-MARGIN-SCROLL_BAR_WIDTH;
			int y3= size.y-MARGIN;

			//ラベル
			if( (x0 <= e.localX) && (e.localX < x2) && 
				(y1 <= e.localY) && (e.localY < y2) ){

				setCurrentLine( showPos_line+ (e.localY-y1)/(LABEL_SIZE_Y-1) );
				dragLabel=true;
			}


			//時間軸目盛り
			if (!dragScrollBarTime) {
				if (((x1 - 2) <= e.localX) && (e.localX < x2) &&
					(y0 <= e.localY) && (e.localY < y2)) {
					setCurrentTime(showPos_time + (double)(e.localX - x1) / timeSize);

					dragTime = true;
				}
			}

			{//ドラッグでの範囲選択
				dragSelectTime1= currentTime;
				dragSelectLine1= currentLine;
			}

			////時間軸スクロールバー
			if( (x1 <= e.localX) && (e.localX < x2) && 
				(y2 <= e.localY) && (e.localY < y3) ){
				int xx0= MARGIN+LABEL_SIZE_X;
				int xx1= size.x-MARGIN-SCROLL_BAR_WIDTH;

				double showTimeLength= ((double)(xx1-xx0-3))/timeSize;
				if( showTimeLength<maxTime ){
					double barSize= ((double)(xx1-xx0-4))*showTimeLength/maxTime;

					int movableX= xx1-xx0-(int)barSize;
					int movableXStart= xx0+(int)barSize/2;

					setShowPosTime( ((double)(e.localX-movableXStart))*(maxTime-showTimeLength)/(double)movableX );

					dragScrollBarTime=true;
				}
			}

			//ラベルスクロールバー
			if( (x2 <= e.localX) && (e.localX < x3) && 
				(y1 <= e.localY) && (e.localY < y2) ){
				int yy0= MARGIN+AXIS_SIZE_Y;
				int yy1= size.y-MARGIN-SCROLL_BAR_WIDTH+1;

				int showLineNum= (size.y-SCROLL_BAR_WIDTH-AXIS_SIZE_Y-MARGIN*2)/(LABEL_SIZE_Y-1);
				if( showLineNum < (int)lineData.size() ){
					int barSize= (yy1-yy0-4)*showLineNum/(int)lineData.size();

					int movableY= yy1-yy0-barSize;
					int movableYStart= yy0+barSize/2;

					setShowPosLine( (e.localY-movableYStart)*((int)lineData.size()-showLineNum)/movableY );

					dragScrollBarLabel=true;
				}
			}

			//Ctrl+ドラッグによるキー移動
			if( e.ctrlKey && !dragScrollBarTime && !dragScrollBarLabel ){
				dragShift= true;
			}

		}
		//	Method : 左マウスボタン ダブルクリックイベント受信
		virtual void onLButtonDBLCLK(const MouseEvent& e) {//2023/10/04
		}
		//	Method : 右マウスボタン ダブルクリックイベント受信
		virtual void onRButtonDBLCLK(const MouseEvent& e) {//2023/10/04
			int dbgflag1 = 1;
		}

		///	Method : 左マウスボタンアップイベント受信
		virtual void onLButtonUp(const MouseEvent& e){
			if ((g_endappflag == 0) && parentWindow && IsWindow(parentWindow->getHWnd())) {

				if (!canMouseControll) return;
				if (g_underselecttolast || g_undereditrange) {
					g_underselecttolast = false;
					g_undereditrange = false;

					//ドラッグフラグを初期化
					dragLabel = false;
					dragTime = false;
					dragScrollBarLabel = false;
					dragScrollBarTime = false;
					dragSelect = false;
					dragShift = false;

					return;
				}

				//ドラッグ選択範囲内のキーを選択状態にする
				if (!dragShift && !dragScrollBarLabel && !dragScrollBarTime) {
					selectClear(true);
					if (dragSelect) {

						for (int i = min(dragSelectLine1, dragSelectLine2);
							(i <= max(dragSelectLine1, dragSelectLine2) && i < (signed int)lineData.size());
							i++) {
							lineData[i]->selectKey(min(dragSelectTime1, dragSelectTime2),
								max(dragSelectTime1, dragSelectTime2));
						}
					}
					//リスナーコール
					if (this->selectListener != NULL) {
						(this->selectListener)();
					}
				}

				//Ctrl+ドラッグによるキー移動
				if (dragShift) {
					//リスナーコール
					if (this->keyShiftListener != NULL) {
						(this->keyShiftListener)();
					}
					ghostShiftTime = 0.0;
				}
				//ドラッグフラグを初期化
				dragLabel = false;
				dragTime = false;
				dragScrollBarLabel = false;
				dragScrollBarTime = false;
				dragSelect = false;
				dragShift = false;

				//再描画領域
				RECT tmpRect;
				tmpRect.left = pos.x + 1;
				tmpRect.top = pos.y + 1;
				tmpRect.right = pos.x + size.x - 1;
				tmpRect.bottom = pos.y + size.y - 1;
				InvalidateRect(parentWindow->getHWnd(), &tmpRect, false);
			}
		}
		virtual void onRButtonDown(const MouseEvent& e){
			selectClear(true);

			if (!canMouseControll) return;
			if (g_underselecttolast) return;
			if (g_undereditrange) return;

			int x0 = MARGIN;
			int x1 = x0 + LABEL_SIZE_X;
			int x2 = size.x - MARGIN - SCROLL_BAR_WIDTH;
			int x3 = size.x - MARGIN;
			int y0 = MARGIN;
			int y1 = y0 + AXIS_SIZE_Y + 1;
			int y2 = size.y - MARGIN - SCROLL_BAR_WIDTH;
			int y3 = size.y - MARGIN;

			//ラベル
			if ((x0 <= e.localX) && (e.localX < x2) && 
				(y1 <= e.localY) && (e.localY < y2)){
				setCurrentLine(showPos_line + (e.localY - y1) / (LABEL_SIZE_Y - 1));

				//dragLabel = true;
			}

			/*
			//時間軸目盛り
			if (x1 - 2 <= e.localX && e.localX<x2
				&& y0 <= e.localY && e.localY<y2){
				setCurrentTime(showPos_time + (double)(e.localX - x1) / timeSize);

				dragTime = true;
			}

			{//ドラッグでの範囲選択
				dragSelectTime1 = currentTime;
				dragSelectLine1 = currentLine;
			}

			//時間軸スクロールバー
			if (x1 <= e.localX && e.localX<x2
				&& y2 <= e.localY && e.localY<y3){
				int xx0 = MARGIN + LABEL_SIZE_X;
				int xx1 = size.x - MARGIN - SCROLL_BAR_WIDTH;

				double showTimeLength = ((double)(xx1 - xx0 - 3)) / timeSize;
				if (showTimeLength<maxTime){
					double barSize = ((double)(xx1 - xx0 - 4))*showTimeLength / maxTime;

					int movableX = xx1 - xx0 - (int)barSize;
					int movableXStart = xx0 + (int)barSize / 2;

					setShowPosTime(((double)(e.localX - movableXStart))*(maxTime - showTimeLength) / (double)movableX);

					dragScrollBarTime = true;
				}
			}

			//ラベルスクロールバー
			if (x2 <= e.localX && e.localX<x3
				&& y1 <= e.localY && e.localY<y2){
				int yy0 = MARGIN + AXIS_SIZE_Y;
				int yy1 = size.y - MARGIN - SCROLL_BAR_WIDTH + 1;

				int showLineNum = (size.y - SCROLL_BAR_WIDTH - AXIS_SIZE_Y - MARGIN * 2) / (LABEL_SIZE_Y - 1);
				if (showLineNum<(int)lineData.size()){
					int barSize = (yy1 - yy0 - 4)*showLineNum / (int)lineData.size();

					int movableY = yy1 - yy0 - barSize;
					int movableYStart = yy0 + barSize / 2;

					setShowPosLine((e.localY - movableYStart)*((int)lineData.size() - showLineNum) / movableY);

					dragScrollBarLabel = true;
				}
			}

			//Ctrl+ドラッグによるキー移動
			if (e.ctrlKey && !dragScrollBarTime && !dragScrollBarLabel){
				dragShift = true;
			}
			*/
		}
		///	Method : 左マウスボタンアップイベント受信
		virtual void onRButtonUp(const MouseEvent& e){
			if ((g_endappflag == 0) && parentWindow && IsWindow(parentWindow->getHWnd())) {

				if (!canMouseControll) return;
				if (g_underselecttolast || g_undereditrange) {
					g_underselecttolast = false;
					g_undereditrange = false;

					//ドラッグフラグを初期化
					dragLabel = false;
					dragTime = false;
					dragScrollBarLabel = false;
					dragScrollBarTime = false;
					dragSelect = false;
					dragShift = false;

					return;
				}

				/*
				//ドラッグ選択範囲内のキーを選択状態にする
				if (!dragShift && !dragScrollBarLabel && !dragScrollBarTime){
					selectClear(true);
					if (dragSelect){

						for (int i = min(dragSelectLine1, dragSelectLine2);
							i <= max(dragSelectLine1, dragSelectLine2) && i < (signed int)lineData.size(); i++){
							lineData[i]->selectKey(min(dragSelectTime1, dragSelectTime2),
								max(dragSelectTime1, dragSelectTime2));
						}
					}
					//リスナーコール
					if (this->selectListener != NULL){
						(this->selectListener)();
					}
				}

				//Ctrl+ドラッグによるキー移動
				if (dragShift){
					//リスナーコール
					if (this->keyShiftListener != NULL){
						(this->keyShiftListener)();
					}
					ghostShiftTime = 0.0;
				}
				*/
				//ドラッグフラグを初期化
				dragLabel = false;
				dragTime = false;
				dragScrollBarLabel = false;
				dragScrollBarTime = false;
				dragSelect = false;
				dragShift = false;

				//再描画領域
				RECT tmpRect;
				tmpRect.left = pos.x + 1;
				tmpRect.top = pos.y + 1;
				tmpRect.right = pos.x + size.x - 1;
				tmpRect.bottom = pos.y + size.y - 1;
				InvalidateRect(parentWindow->getHWnd(), &tmpRect, false);

				if (this->mouseRUpListener != NULL) {
					(this->mouseRUpListener)();
				}
			}
		}

		///	Method : マウス移動イベント受信
		virtual void onMouseMove(const MouseEvent& e){
			if( !canMouseControll ) return;
			if (g_underselecttolast) return;
			if (g_undereditrange) return;

			int x0= MARGIN;
			int x1= x0+LABEL_SIZE_X;
			int x2= size.x-MARGIN-SCROLL_BAR_WIDTH;
			int x3= size.x-MARGIN;
			int y0= MARGIN;
			int y1= y0+AXIS_SIZE_Y+1;
			int y2= size.y-MARGIN-SCROLL_BAR_WIDTH;
			int y3= size.y-MARGIN;

			//ラベル
			bool callCursorListener= false;
			if(dragLabel){
				int oldLine= currentLine;
				setCurrentLine( showPos_line+ (e.localY-y1)/(LABEL_SIZE_Y-1), true );
				if( oldLine!=currentLine ){
					callCursorListener= true;
				}
			}

			//時間軸目盛り
			if(dragTime){
				double oldTime= currentTime;
				setCurrentTime( showPos_time+ (double)(e.localX-x1)/timeSize, true );
				if( oldTime!=currentTime ){
					callCursorListener= true;
				}
			}

			//カーソルリスナーコール
			if( callCursorListener && this->cursorListener!=NULL ){
				(this->cursorListener)();
			}

			//Ctrl+ドラッグでのキー移動
			if( dragShift ){
				ghostShiftTime= currentTime-dragSelectTime1;
			}

			//ドラッグでの範囲選択
			if( !dragShift && (dragSelect || dragLabel || dragTime) && !dragScrollBarLabel && !dragScrollBarTime ){
				dragSelect=true;
				dragSelectTime2= currentTime;
				dragSelectLine2= currentLine;
			}

			//ドラッグ選択範囲内のキーを選択状態にする
			if( dragSelect ){
				selectClear(true);
				for(int i=min(dragSelectLine1,dragSelectLine2);
					i<=max(dragSelectLine1,dragSelectLine2) && i<(signed int)lineData.size(); i++){
					lineData[i]->selectKey( min(dragSelectTime1,dragSelectTime2),
											max(dragSelectTime1,dragSelectTime2) );
				}
			}

			//時間軸スクロールバー
			if( dragScrollBarTime ){
				int xx0= MARGIN+LABEL_SIZE_X;
				int xx1= size.x-MARGIN-SCROLL_BAR_WIDTH;

				double showTimeLength= ((double)(xx1-xx0-3))/timeSize;
				double barSize= ((double)(xx1-xx0-4))*showTimeLength/maxTime;

				int movableX= xx1-xx0-(int)barSize;
				int movableXStart= xx0+(int)barSize/2;

				setShowPosTime( ((double)(e.localX-movableXStart))*(maxTime-showTimeLength)/(double)movableX );
			}

			//ラベルスクロールバー
			if( dragScrollBarLabel ){
				int yy0= MARGIN+AXIS_SIZE_Y;
				int yy1= size.y-MARGIN-SCROLL_BAR_WIDTH+1;

				int showLineNum= (size.y-SCROLL_BAR_WIDTH-AXIS_SIZE_Y-MARGIN*2)/(LABEL_SIZE_Y-1);
				int barSize= (yy1-yy0-4)*showLineNum/(int)lineData.size();

				int movableY= yy1-yy0-barSize;
				int movableYStart= yy0+barSize/2;

				setShowPosLine( (e.localY-movableYStart)*((int)lineData.size()-showLineNum)/movableY );
			}
		}
		virtual void onMButtonDown(const MouseEvent& e){
			if (!canMouseControll) return;

			int x0 = MARGIN;
			int x1 = x0 + LABEL_SIZE_X;
			int x2 = size.x - MARGIN - SCROLL_BAR_WIDTH;
			int x3 = size.x - MARGIN;
			int y0 = MARGIN;
			int y1 = y0 + AXIS_SIZE_Y + 1;
			int y2 = size.y - MARGIN - SCROLL_BAR_WIDTH;
			int y3 = size.y - MARGIN;

			//ラベル
			if ((x0 <= e.localX) && (e.localX < x2) && (y1 <= e.localY) && (e.localY < y2)){
				setCurrentLine(showPos_line + (e.localY - y1) / (LABEL_SIZE_Y - 1));

				//dragLabel = true;
			}

			//時間軸目盛り
			if (((x1 - 2) <= e.localX) && (e.localX < x2) && (y0 <= e.localY) && (e.localY < y2)){
				setCurrentTime(showPos_time + (double)(e.localX - x1) / timeSize);

				//dragTime = true;
			}
			if (this->mouseMDownListener != NULL){
				(this->mouseMDownListener)();
			}
		}
		virtual void onMButtonUp(const MouseEvent& e){
			if ((g_endappflag == 0) && parentWindow && IsWindow(parentWindow->getHWnd())) {

				if (!canMouseControll) return;

				//ドラッグフラグを初期化
				dragLabel = false;
				dragTime = false;
				dragScrollBarLabel = false;
				dragScrollBarTime = false;
				dragSelect = false;
				dragShift = false;

				//再描画領域
				RECT tmpRect;
				tmpRect.left = pos.x + 1;
				tmpRect.top = pos.y + 1;
				tmpRect.right = pos.x + size.x - 1;
				tmpRect.bottom = pos.y + size.y - 1;
				InvalidateRect(parentWindow->getHWnd(), &tmpRect, false);
			}
		}
		virtual void onMouseWheel(const MouseEvent& e){

			if (heightwheel) {//高さ方向にホイールスクロールするかどうか
				//#################################################
				//高さ方向のホイール処理　クライアントエリア全領域
				//#################################################
				//int x0 = size.x - SCROLL_BAR_WIDTH - 1;//スクロールバーだけ
				int x0 = 0;
				int x1 = size.x;
				int y0 = 0;
				int y1 = size.y;
				if ((e.localX >= x0) && (e.localX <= x1) &&
					(e.localY >= y0) && (e.localY <= y1)) {

					//#########################################
					//クライアントエリアでホイールを回した場合
					//#########################################

					int wheeldelta = e.wheeldelta;
					int linedelta = 0;
					if (wheeldelta > 0) {
						linedelta = -1;
					}
					else if (wheeldelta < 0) {
						linedelta = 1;
					}
					else {
						linedelta = 0;
					}
					int curline = getShowPosLine();
					setShowPosLine(curline + linedelta);
					autoResize();
				}
			}


			//#####################################
			//時間方向のホイール処理　LTimeline用
			//#####################################
			wheeldelta = e.wheeldelta;
			if (this->mouseWheelListener != NULL){
				(this->mouseWheelListener)();
			}
		}
		int getMouseWheelDelta(){ return wheeldelta; }
		/// Method : 行インデックスを取得する
		int getLineIndex(const std::basic_string<TCHAR>& _name) const{
			for(int i=0; i<(int)lineData.size(); i++){
				if(lineData[i]->name==_name){
					return i;
				}
			}
			return -1;
		}

		/////////////////////////// Accessor /////////////////////////////
		//	Accessor : maxTime
		double getMaxTime() const{
			return maxTime;
		}
		void setMaxTime(double _maxTime){
			maxTime= max(_maxTime,0);
			currentTime= min(currentTime,maxTime);

			//再描画要求
			if( rewriteOnChange ){
				callRewrite();
			}
		}
		bool getDispKeyFlag()
		{
			return dispkeyflag;
		}
		void setDispKeyFlag(bool srcflag)
		{
			dispkeyflag = srcflag;
		}

		/// Accessor : timeSnapSize
		double getTimeSnapSize() const{
			return timeSnapSize;
		}
		void setTimeSnapSize(const double &value){
			timeSnapSize= value;
		}
		/// Accessor : rewriteOnChange
		bool getRewriteOnChangeFlag() const{
			return rewriteOnChange;
		}
		void setRewriteOnChangeFlag(bool value){
			rewriteOnChange= value;
		}
		/// Accessor : canMouseControll
		bool getCanMouseControllFlag() const{
			return canMouseControll;
		}
		void setCanMouseControllFlag(bool value){
			canMouseControll= value;
		}
		//	Accessor : currentTime
		double getCurrentTime() const{
			return currentTime;
		}
		void setCurrentTime(double _currentTime, bool CallListener = false, bool needRewrite = true);
		//	Accessor : currentLine
		int getCurrentLine() const{
			return currentLine;
		}
		void setCurrentLine(int _currentLine, bool noCallListener=false){
			int showLineNum= (size.y-SCROLL_BAR_WIDTH-AXIS_SIZE_Y-MARGIN*2)/(LABEL_SIZE_Y-1);

			currentLine= min(max(_currentLine,0),(int)lineData.size()-1);

			if( currentLine <= showPos_line ){
				showPos_line= currentLine;
			}
			if( (showPos_line + showLineNum - 1) <= currentLine ){
				showPos_line= currentLine-showLineNum+1;
			}

			//リスナーコール
			if( !noCallListener && (this->cursorListener != NULL) ){
				(this->cursorListener)();
			}

			//再描画要求
			if( rewriteOnChange ){
				callRewrite();
			}
		}
		std::basic_string<TCHAR> getCurrentLineName() const{
			return lineData[currentLine]->name;
		}
		void setCurrentLineName(const std::basic_string<TCHAR>& value){
			for(int i=0; i<(int)lineData.size(); i++){
				if( lineData[i]->name==value ){
					setCurrentLine(i);
				}
			}
		}
		//	Accessor : showPos_time
		double getShowPosTime() const{
			return showPos_time;
		}
		double calcShowPosTime(double _showPosTime);
		void setShowPosTime(const double& _showPosTime) {
			showPos_time = calcShowPosTime(_showPosTime);

			//再描画要求
			//if (rewriteOnChange) {
			//	callRewrite();
			//}
		};
		//	Accessor : showPos_line
		int getShowPosLine() const{
			return showPos_line;
		}

		void setShowPosLine(int _showPosLine){
			int showLineNum = (size.y - SCROLL_BAR_WIDTH - AXIS_SIZE_Y - MARGIN * 2) / (LABEL_SIZE_Y - 1);
			int y0= MARGIN+AXIS_SIZE_Y;
			int y1= size.y-MARGIN-SCROLL_BAR_WIDTH+1;

			if( showLineNum < (int)lineData.size() ){
				showPos_line= max(0,min( _showPosLine ,(int)lineData.size()-showLineNum));
			}else{
				showPos_line= 0;
			}

			//再描画要求
			if( rewriteOnChange ){
				callRewrite();
			}
		}
		/// Accessor : keyShiftTime
		double getShiftKeyTime() const {
			return ghostShiftTime;
		}
		///	Accessor : cursorListener
		void setCursorListener(std::function<void()> listener){
			this->cursorListener= listener;
		}
		///	Accessor : selectListener
		void setSelectListener(std::function<void()> listener){
			this->selectListener= listener;
		}
		///	Accessor : keyShiftListener
		void setKeyShiftListener(std::function<void()> listener){
			this->keyShiftListener= listener;
		}
		///	Accessor : keyShiftListener
		void setKeyDeleteListener(std::function<void(const KeyInfo&)> listener){
			this->keyDeleteListener= listener;
		}
		void setMouseMDownListener(std::function<void()> listener){
			this->mouseMDownListener = listener;
		}
		void setMouseWheelListener(std::function<void()> listener){
			this->mouseWheelListener = listener;
		}
		void setMouseRUpListener(std::function<void()> listener){
			this->mouseRUpListener = listener;
		}


		KeyInfo ExistKey( int srcline, double srctime )
		{
			KeyInfo retki;
			retki.lineIndex = -1;
			retki.time = -1.0;
			//retki.blendweight.clear();
			retki.object = 0;
			if( (srcline >= 0) && (srcline < (int)lineData.size()) ){
				LineData* curLineData= lineData[srcline];
				if( curLineData ){
					int i = curLineData->getKeyIndex( srctime );
					if( i >= 0 ){
						retki.lineIndex = srcline;
						retki.time = curLineData->key[i]->time;
						retki.object = curLineData->key[i]->object;
					}
				}
			}

			return retki;
		}


	private:
		////////////////////////// MemberVar /////////////////////////////
		double maxTime,currentTime,showPos_time,showPos_width;
		int currentLine,showPos_line;
		int wheeldelta;
		std::function<void()> cursorListener;
		std::function<void()> selectListener;
		std::function<void()> keyShiftListener;
		std::function<void()> mouseMDownListener;
		std::function<void()> mouseWheelListener;
		std::function<void()> mouseRUpListener;
		std::function<void(const KeyInfo&)> keyDeleteListener;

		bool dispkeyflag;
		bool heightwheel;//高さ方向にホイールスクロールするかどうか

		//行データクラス-------------
		public: class LineData{
		public:
			LineData(int _depth, int nullflag, 
				bool srcikstopflag, bool srcconstraintflag, 
				const std::basic_string<TCHAR>& _name, OWP_Timeline *_parent, unsigned int _lineIndex, COLORREF srctextcol = RGB(256,256, 256)){
				depth = _depth;
				m_nullflag = nullflag;
				ikstopflag = srcikstopflag;
				constraintflag = srcconstraintflag;
				name= _name;
				parent= _parent;
				lineIndex= _lineIndex;
				key.clear();
				hasrigflag = false;
				textcol = srctextcol;
				minselected = 0;
				maxselected = 0;
			}
			//LineData( const LineData& a ){
			//	_ASSERT_EXPR( 0, L"コピーコンストラクタは使えません" );
			//}
			~LineData(){
				if (!key.empty()) {
					std::vector<Key*>::iterator it;
					for (it = key.begin(); it != key.end(); it++) {
						//delete (*it);
						if ((Key*)(*it)) {
							((Key*)(*it))->InvalidateKeys();
						}
					}
				}
			}

			bool setHasRigFlag(bool flag)
			{
				hasrigflag = flag;
				return true;
			}
			bool getHasRigFlag()
			{
				return hasrigflag;
			}

			int getMinSelected()
			{
				return minselected;
			}
			void setMinSelected(int srcval)
			{
				minselected = srcval;
			}
			int getMaxSelected()
			{
				return maxselected;
			}
			void setMaxSelected(int srcval)
			{
				maxselected = srcval;
			}
			bool getIKStopFlag()
			{
				return ikstopflag;
			}
			void setIKStopFlag(bool srcflag)
			{
				ikstopflag = srcflag;
			}
			bool getConstraintFlag()
			{
				return constraintflag;
			}
			void setConstraintFlag(bool srcflag)
			{
				constraintflag = srcflag;
			}

			//キーデータクラス---------------
			class Key{
			public:
				Key()
				{
					InitParams();
				}
				Key(double _time, int _type=0, void *_object=NULL, double _length=1.0, bool _select=false ){
					SetParams(_time, _type, _object, _length, _select);
				}

				void SetParams(double _time, int _type = 0, void* _object = NULL, double _length = 1.0, bool _select = false) {
					//InitParams();//useflag, indexofpool, allogheadflagは設定済

					time = _time;
					type = _type;
					length = _length;
					select = _select;
					object = _object;
				}

				void InitParams() {
					time = 0.0;
					type = 0;
					length = 0.0;
					select = false;
					object = NULL;

					m_useflag = 0;//0: not use, 1: in use
					m_indexofpool = 0;//index of pool vector
					m_allocheadflag = 0;//1: head pointer at allocated
				};

				void InvalidateKeys();

				int GetUseFlag()
				{
					return m_useflag;
				};
				void SetUseFlag(int srcflag)
				{
					m_useflag = srcflag;
				};
				int GetIndexOfPool()
				{
					return m_indexofpool;
				};
				void SetIndexOfPool(int srcindex)
				{
					m_indexofpool = srcindex;
				};
				int IsAllocHead()
				{
					return m_allocheadflag;
				};
				void SetIsAllocHead(int srcflag)
				{
					m_allocheadflag = srcflag;
				};

				////////////////////////// MemberVar /////////////////////////////
				double time,length;
				int type;
				bool select;
				void *object;
				int m_useflag;//0: not use, 1: in use
				int m_indexofpool;//index of pool vector
				int m_allocheadflag;//1: head pointer at allocated
			};

			////////////////////////// MemberVar /////////////////////////////
			int m_nullflag;
			bool ikstopflag;
			bool constraintflag;
			std::basic_string<TCHAR> name;
			std::vector<Key*> key;
			unsigned int lineIndex;
			int depth;
			bool hasrigflag;
			COLORREF textcol;
			int minselected;//2022/09/12 : key[].selectの検索ヒント
			int maxselected;//2022/09/12 : key[].selectの検索ヒント

			//////////////////////////// Method //////////////////////////////
			//	Method : 描画
			virtual void callRewrite();

			void draw(	HDCMaster *hdcM,
						int posX, int posY,
						int width,
						double timeSize,
						double startTime,
						bool highLight=false){
				unsigned char baseR= parent->baseColor.r;
				unsigned char baseG= parent->baseColor.g;
				unsigned char baseB= parent->baseColor.b;

				//highLight
				int x0= posX;
				int x1= posX+parent->LABEL_SIZE_X;
				int x2= posX+width;
				int y0= posY;
				int y1= posY+parent->LABEL_SIZE_Y;
				if( highLight ){
					//hdcM->setPenAndBrush(NULL,RGB(min(baseR+20,255),min(baseG+20,255),min(baseB+20,255)));
					hdcM->setPenAndBrush(NULL, RGB(255, 128, 0));
					Rectangle(hdcM->hDC,x0,y0,x1,y1);
				}

				//ラベル
				if (g_4kresolution) {
					//hdcM->setFont(20, _T("ＭＳ ゴシック"));
					hdcM->setFont(18, _T("ＭＳ ゴシック"));
				}
				else {
					hdcM->setFont(12, _T("ＭＳ ゴシック"));
				}
				
				if (textcol == RGB(255, 255, 255)) {
					if (hasrigflag) {
						SetTextColor(hdcM->hDC, RGB(0, 255, 0));
					}
					else if (m_nullflag == 0) {
						SetTextColor(hdcM->hDC, RGB(220, 220, 220));
					}
					else if (m_nullflag == 1) {
						SetTextColor(hdcM->hDC, RGB(0, 220, 220));
					}
					else if (m_nullflag == 2) {
						SetTextColor(hdcM->hDC, RGB(64, 128, 255));
					}
					else {
						//_ASSERT(0);
						SetTextColor(hdcM->hDC, RGB(255, 0, 0));
						//return;
					}
				}
				else {
					SetTextColor(hdcM->hDC, textcol);
				}

				std::basic_string<TCHAR> prname;
				int depthcnt;
				for (depthcnt = 0; depthcnt < depth; depthcnt++){
					prname += TEXT("  ");
				}
				prname += name;

				if (getIKStopFlag()) {
					prname += TEXT(" (⛔)");
				}
				if (getConstraintFlag()) {
					prname += TEXT(" (⏸)");
				}

				TextOut(hdcM->hDC,
					posX + 2, posY + parent->LABEL_SIZE_Y / 2 - 5,
					prname.c_str(), (int)_tcslen(prname.c_str()));

				//TextOut( hdcM->hDC,
				//		 posX+2, posY+parent->LABEL_SIZE_Y/2-5,
				//		 name.c_str(), _tcslen(name.c_str()));

				//枠
				hdcM->setPenAndBrush(RGB(min(baseR+20,255),min(baseG+20,255),min(baseB+20,255)),NULL);
				Rectangle(hdcM->hDC,x0,y0,x2,y1);
				hdcM->setPenAndBrush(RGB(min(baseR+20,255),min(baseG+20,255),min(baseB+20,255)),RGB(baseR,baseG,baseB));
				Rectangle(hdcM->hDC,x1-2,y0,x1+1,y1);
				Rectangle(hdcM->hDC,x1,y0,x2,y1);
				int x3= (int)((parent->maxTime-startTime)*timeSize) + x1+2;		//maxTime
				if( (x1 <= x3) && (x3 <= x2) ){
					MoveToEx(hdcM->hDC, x3,y0, NULL);
					LineTo(hdcM->hDC,   x3,y1);
				}
				if( (x1 <= x3 + 2) && (x3 + 2 <= x2) ){
					MoveToEx(hdcM->hDC, x3+2,y0, NULL);
					LineTo(hdcM->hDC,   x3+2,y1);
				}
				int x4= (int)((parent->currentTime-startTime)*timeSize) + x1+1;		//currentTime
				if( (x1 <= x4) && (x4 < x2) ){
					MoveToEx(hdcM->hDC, x4,y0, NULL);
					LineTo(hdcM->hDC,   x4,y1);
				}
				
				//ゴーストキー
				x1++; x2--;
				y0++; y1--;
				//for(int i=0; i<(int)key.size(); i++){
				//	int xx0= (int)((key[i]->time-startTime+parent->ghostShiftTime)*timeSize) + x1;
				//	int xx1= (int)(key[i]->length*timeSize) + xx0 + 1;

				//	if( x2 <= xx0 ){
				//		break;
				//	}
				//	if( x1<=xx1 && key[i]->select ){

				//		if( (x1 <= (xx1 - 1)) && ((xx0 + 1) <= x2) ){

				//			hdcM->setPenAndBrush(NULL,RGB(min(baseR+20,255),min(baseG+20,255),min(baseB+20,255)));
				//			Rectangle(hdcM->hDC,max(xx0+1,x1),y0+1,min(xx1-1,x2),y1-1);

				//		}

				//		if( x1 <= xx0 ){
				//			hdcM->setPenAndBrush(RGB(baseR,baseG,baseB),NULL);
				//			MoveToEx(hdcM->hDC, xx0,y0, NULL);
				//			LineTo(hdcM->hDC,   xx0,y1);
				//		}

				//	}
				//}

				//キー
				if (parent->getDispKeyFlag()) {//2022/09/12 LTimelineの場合に表示　Timelineの場合非表示
					int startindex = getKeyIndex(startTime);
					if (startindex >= 0) {
						int currentkeynum = (int)key.size();
						for (int i = startindex; i < currentkeynum; i++) {
							int xx0 = (int)((key[i]->time - startTime) * timeSize) + x1;
							int xx1 = (int)(key[i]->length * timeSize) + xx0 + 1;

							if (x2 <= xx0) {
								break;
							}
							if (x1 <= xx1) {

								if ((x1 <= (xx1 - 1)) && ((xx0 + 1) <= x2)) {
									//if (key[i]->select) {
									//if ((key[i]->select) || ((g_previewFlag != 0) && (key[i]->time >= g_motionbrush_startframe) && (key[i]->time <= g_motionbrush_endframe))) {
									//if((key[i]->time >= g_playingstart) && (key[i]->time <= g_playingend)){
									if((key[i]->select) ||
										((key[i]->time >= g_playingstart) && (key[i]->time <= g_playingend)) ||
										((key[i]->time >= g_motionbrush_startframe) && (key[i]->time <= g_motionbrush_endframe))){
										//hdcM->setPenAndBrush(NULL, RGB(240, 240, 240));
										//hdcM->setPenAndBrush(NULL, RGB(255, 128, 128));
										hdcM->setPenAndBrush(NULL, RGB(64, 128, 255));//AO
										Rectangle(hdcM->hDC, max(xx0 + 1, x1), y0 + 1, min(xx1 - 1, x2), y1 - 1);
										//hdcM->setPenAndBrush(NULL, RGB(min(baseR + 20, 255), min(baseG + 20, 255), min(baseB + 20, 255)));//四角の中身を黒く抜くときはこの２行
										//Rectangle(hdcM->hDC, max(xx0 + 2, x1), y0 + 2, min(xx1 - 2, x2), y1 - 2);

									}
									else {
										hdcM->setPenAndBrush(NULL, RGB(240, 240, 240));
										Rectangle(hdcM->hDC, max(xx0 + 1, x1), y0 + 1, min(xx1 - 1, x2), y1 - 1);
									}

								}

								if (x1 <= xx0) {
									hdcM->setPenAndBrush(RGB(baseR, baseG, baseB), NULL);
									MoveToEx(hdcM->hDC, xx0, y0, NULL);
									LineTo(hdcM->hDC, xx0, y1);
								}

							}
						}
					}
				}
				


				//for(int i=0; i<(int)key.size(); i++){
				//	int xx0= (int)((key[i]->time-startTime)*timeSize) + x1;
				//	int xx1= (int)(key[i]->length*timeSize) + xx0 + 1;

				//	if( x2 <= xx0 ){
				//		break;
				//	}
				//	if( x1 <= xx1 ){

				//		if( (x1 <= (xx1 - 1)) && ((xx0 + 1) <= x2) ){

				//			if( key[i]->select ){
				//				hdcM->setPenAndBrush(NULL,RGB(240,240,240));
				//				Rectangle(hdcM->hDC,max(xx0+1,x1),y0+1,min(xx1-1,x2),y1-1);
				//				hdcM->setPenAndBrush(NULL,RGB(min(baseR+20,255),min(baseG+20,255),min(baseB+20,255)));
				//				Rectangle(hdcM->hDC,max(xx0+2,x1),y0+2,min(xx1-2,x2),y1-2);

				//			}else{
				//				hdcM->setPenAndBrush(NULL,RGB(240,240,240));
				//				Rectangle(hdcM->hDC,max(xx0+1,x1),y0+1,min(xx1-1,x2),y1-1);
				//			}

				//		}

				//		if( x1 <= xx0 ){
				//			hdcM->setPenAndBrush(RGB(baseR,baseG,baseB),NULL);
				//			MoveToEx(hdcM->hDC, xx0,y0, NULL);
				//			LineTo(hdcM->hDC,   xx0,y1);
				//		}

				//	}
				//}

			}

			//	Method : キーを作成する
			bool newKey(const double &_time, int _type=0, void* _object=NULL, double _length=1.0, bool _select=false){

				if(parent->allowSameTimeKey==false){
					int i= getKeyIndex(_time);
					if(i != -1){
						//上書きリスナーコール
						if( parent->keyDeleteListener!=NULL ){
							KeyInfo ki;
							ki.label= name.c_str();
							ki.lineIndex= lineIndex;
							ki.time= key[i]->time;
							ki.timeIndex= -1;
							ki.object= key[i]->object;
							(parent->keyDeleteListener)(ki);
						}

						key[i]->time = _time;//2022/10/22

						key[i]->type=   _type;
						key[i]->length= _length;
						key[i]->select= _select;
						key[i]->object= _object;
						return true;
					}
				}
				
				if( _time>=parent->maxTime || (_time < 0.0) ){
/*					//作成不能削除リスナーコール
					if( parent->keyDeleteListener!=NULL ){
						KeyInfo ki;
						ki.label= name.c_str();
						ki.lineIndex= lineIndex;
						ki.time= _time;
						ki.timeIndex= -1;
						ki.object= _object;
						parent->keyDeleteListener(ki);
					}//*/
					return false;
				}
				if((_time + _length) >= parent->maxTime){
					_length= parent->maxTime-_time;
				}


				unsigned int currentindex = (unsigned int)(_time + 0.1);
				if (key.size() != currentindex) {
					//1.0.0.28 : key[] become fullframe time index array. key[(int)time].
					_ASSERT(0);
					return false;
				}
				else {
					Key* newkey = (Key*)GetNewKey();
					if (newkey) {
						newkey->SetParams(_time, _type, _object, _length, _select);
						//key[currentindex] = newkey;
						key.push_back(newkey);
					}
					else {
						_ASSERT(0);
						return false;
					}
				}
				return true;

				//if(key.size() != 0){
				//	int pushPos=(int)key.size();
				//	for(int i=0; i<(int)key.size(); i++){
				//		if(_time <= key[i]->time){
				//			pushPos=i;
				//			break;
				//		}
				//	}
				//	key.push_back(key[(int)key.size()-1]);
				//	for(int i=(int)key.size()-2; i>pushPos; i--){
				//		key[i]=key[i-1];
				//	}
				//	//key[pushPos]= new Key(_time,_type,_object,_length,_select);
				//	Key* newkey = (Key*)GetNewKey();
				//	if (newkey) {
				//		newkey->SetParams(_time, _type, _object, _length, _select);
				//		key[pushPos] = newkey;
				//	}
				//	else {
				//		_ASSERT(0);
				//		return false;
				//	}

				//}else{
				//	//key.push_back(new Key(_time,_type,_object,_length,_select));
				//	Key* newkey = (Key*)GetNewKey();
				//	if (newkey) {
				//		newkey->SetParams(_time, _type, _object, _length, _select);
				//		key.push_back(newkey);
				//	}
				//	else {
				//		_ASSERT(0);
				//		return false;
				//	}
				//}
				//
				//return true;
			}



			//	Method : すべてのキーの選択を解除する
			void selectClear(){
				int currentkeynum = (int)key.size();
				for(int i = 0; i < currentkeynum; i++){
					if (key[i]) {
						key[i]->select = false;
					}
				}
			}
			//	Method : すべてのキーを選択する
			void selectAll(){
				int currentkeynum = (int)key.size();
				for(int i = 0; i < currentkeynum; i++){
					if (key[i]) {
						key[i]->select = true;
					}
				}
				parent->ghostShiftTime=0;
			}
			//	Method : 指定された時刻にあるキーをひとつ選択する
			bool selectKey(const double &_time){

				int currentindex = getKeyIndex(_time);
				if (currentindex >= 0) {
					key[currentindex]->select = true;
					return true;
				}
				setMinSelected(currentindex);
				setMaxSelected(currentindex);

				//for(int i=0; i<(int)key.size(); i++){
				//	if( ((_time - TIME_ERROR_WIDTH) <= key[i]->time) && 
				//		(key[i]->time <= (_time + TIME_ERROR_WIDTH)) ){
				//		key[i]->select=true;
				//		return true;
				//	}
				//}
				parent->ghostShiftTime=0;
				return false;

			}
			///	Method : 指定された時刻範囲にあるキーをすべて選択する
			///			 厳密モード(startTime<=keyTime<endTime)
			int selectKey(const double &startTime, const double &endTime){

				int selectCount = 0;
				int startindex, endindex;
				startindex = getKeyIndex(startTime);
				endindex = getKeyIndex(endTime);
				if ((startindex >= 0) && (endindex >= 0)) {
					for (int i = startindex; i <= endindex; i++) {
						if ((startTime <= key[i]->time) && (key[i]->time <= endTime)) {//endTime ==含む　2021/11/09
							key[i]->select = true;
							selectCount++;
						}
					}
				}
				setMinSelected(startindex);
				setMaxSelected(endindex);

//				int selectCount=0;
//				for(int i=0; i<(int)key.size(); i++){
////					if( startTime-TIME_ERROR_WIDTH <= key[i]->time
////					 && key[i]->time <= endTime+TIME_ERROR_WIDTH ){
//					if((startTime <= key[i]->time) && (key[i]->time <= endTime)){//endTime ==含む　2021/11/09
//						key[i]->select=true;
//						selectCount++;
//					}
//				}
				parent->ghostShiftTime=0;
				return selectCount;

			}
			///	Method : 指定された時刻範囲にあるキーをすべて選択する
			///			 大雑把モード(startTime-TIME_ERROR_WIDTH<=keyTime<=endTime+TIME_ERROR_WIDTH)
			int selectKey2(const double &startTime, const double &endTime){

				int selectCount = 0;
				int startindex, endindex;
				startindex = getKeyIndex(startTime);
				endindex = getKeyIndex(endTime);
				if ((startindex >= 0) && (endindex >= 0)) {
					for (int i = startindex; i <= endindex; i++) {
						if ((startTime <= key[i]->time) && (key[i]->time <= endTime)) {//endTime ==含む　2021/11/09
							key[i]->select = true;
							selectCount++;
						}
					}
				}
				setMinSelected(startindex);
				setMaxSelected(endindex);

				//int selectCount=0;
				//for(int i=0; i<(int)key.size(); i++){
				//	if( ((startTime - TIME_ERROR_WIDTH) <= key[i]->time) && 
				//		(key[i]->time <= (endTime + TIME_ERROR_WIDTH)) ){
				//		key[i]->select=true;
				//		selectCount++;
				//	}
				//}
				parent->ghostShiftTime=0;
				return selectCount;

			}
			//	Method : 指定された時刻にあるキーのインデックスをひとつ取得する
			int getKeyIndex(const double &_time){
				
				unsigned int currentindex = (unsigned int)(_time + 0.1);
				if (key.size() <= currentindex) {
					return -1;
				}
				else {
					return currentindex;
				}

				//for(int i=0; i<(int)key.size(); i++){
				//	if( ((_time - TIME_ERROR_WIDTH) <= key[i]->time) && 
				//		(key[i]->time <= (_time + TIME_ERROR_WIDTH)) ){
				//		return i;
				//	}
				//}
				//return -1;

			}
			////	Method : 選択されているキーをすべて削除する
			//unsigned int deleteKey(bool noCallListener=false){
			//	Method : キーをすべて削除する
			unsigned int deleteKey(bool noCallListener = false) {
				unsigned int deleteNum = 0;
				for (unsigned int i = 0; i < (int)key.size(); i++) {
					//if (key[i]->select) {
						//リスナーコール
						if (!noCallListener && (parent->keyDeleteListener != NULL)) {
							KeyInfo ki;
							ki.label = name.c_str();
							ki.lineIndex = lineIndex;
							ki.time = key[i]->time;
							ki.timeIndex = -1;
							ki.object = key[i]->object;
							(parent->keyDeleteListener)(ki);
						}

						//delete key[i];
						if (key[i]) {
							key[i]->InvalidateKeys();
						}

						deleteNum++;
					//}
					//else {
					//	key[i - deleteNum] = key[i];
					//}
				}
				//for (unsigned int i = 0; i < deleteNum; i++) {
				//	key.pop_back();
				//}
				key.clear();

				return deleteNum;

			}
			////	Method : 指定されたキーを削除する
			//bool deleteKey(int index, bool noCallListener=false){
			//	if( (unsigned int)key.size() <= (unsigned int)index ) return false;

			//	//リスナーコール
			//	if( !noCallListener && (parent->keyDeleteListener != NULL) ){
			//		KeyInfo ki;
			//		ki.label= name.c_str();
			//		ki.lineIndex= lineIndex;
			//		ki.time= key[index]->time;
			//		ki.timeIndex= -1;
			//		ki.object= key[index]->object;
			//		(parent->keyDeleteListener)(ki);
			//	}

			//	//delete key[index];
			//	key[index]->InvalidateKeys();

			//	for(int i=index; i<(int)key.size(); i++){
			//		key[i-1]=key[i];
			//	}
			//	key.pop_back();
			//	return true;
			//}
//			//	Method : 指定されたキーを移動する
//			bool shiftKey(const double &shiftTime, int index){
//				if( key.size() <= (unsigned int)index ) return false;
//
//				double dstTime= key[index]->time+shiftTime;
//				int type= key[index]->type;
//				double length= key[index]->length;
//				bool select= key[index]->select;
//				void *object= key[index]->object;
//				deleteKey(index,true);
//
//				//既にキーが存在する場合
//				if( !newKey(dstTime,type,object,length,select) ){
//					//置き換えによるキー削除リスナーコール
//					if( parent->keyDeleteListener != NULL ){
//						KeyInfo ki;
//						ki.label= name.c_str();
//						ki.lineIndex= lineIndex;
//						ki.time= dstTime;
//						ki.timeIndex= -1;
//						ki.object= object;
//						(parent->keyDeleteListener)(ki);
//					}
//
//					//存在するキーの値を置き換え後のキーの値に変更
//					int dstIndex= getKeyIndex(dstTime);
//					key[dstIndex]->type= type;
//					key[dstIndex]->length= length;
//					key[dstIndex]->object= object;
//				}
//
//				return true;
//			}
//			//	Method : 選択されているキーをすべて移動する
//			unsigned int shiftKey(const double &shiftTime){
//
//				unsigned int shiftNum=0;
//				std::list<Key> shiftKeyList;
//				for(int i=0; i<(int)key.size(); i++){
//					if( key[i]->select ){
//						shiftKeyList.push_back(*(key[i]));
////						shiftKey(shiftTime,i);
//						shiftNum++;
//					}
//				}
//
//				deleteKey(true);
//
//				std::list<Key>::iterator itr;
//				for(itr = shiftKeyList.begin(); itr != shiftKeyList.end(); itr++){
//					if( !newKey( itr->time+shiftTime, itr->type, itr->object, itr->length, itr->select ) ){
//						//作成不能削除リスナーコール
//						if( parent->keyDeleteListener != NULL ){
//							KeyInfo ki;
//							ki.label= name.c_str();
//							ki.lineIndex= lineIndex;
//							ki.time= itr->time+shiftTime;
//							ki.timeIndex= -1;
//							ki.object= itr->object;
//							(parent->keyDeleteListener)(ki);
//						}
//					}
//				}
//
//				return shiftNum;
//			}
//			//	Method : 選択されているキーをゴーストキーの位置へすべて移動する
//			unsigned int shiftKey(){
//				return shiftKey(parent->ghostShiftTime);
//			}

			/////////////////////////// Accessor /////////////////////////////
			int getSelectNum(){

				int selectCount=0;
				int currentkeynum = (int)key.size();
				int startindex = max(getMinSelected(), 0);
				int endindex = min((getMaxSelected() + 1), currentkeynum);
				if ((startindex >= 0) && (endindex >= 0)) {
					for (int j = startindex; j < endindex; j++) {//2022/09/12
						if (key[j]->select) {
							selectCount++;
						}
					}
				}
				//for(int i = 0; i < currentkeynum; i++){
				//	if( key[i]->select ){
				//		selectCount++;
				//	}
				//}
				return selectCount;

			}

		private:
			////////////////////////// MemberVar /////////////////////////////
			OWP_Timeline *parent;
			//static const double TIME_ERROR_WIDTH;
		};
		std::vector<LineData*> lineData;
		double ghostShiftTime;


		//static const int LABEL_SIZE_Y= 15;
		int LABEL_SIZE_Y= 15;
		//static const int LABEL_SIZE_X= 75;
		//static const int LABEL_SIZE_X= 250;


		//static const int LABEL_SIZE_X = 280;
		int LABEL_SIZE_X = 280;
		bool shortlabel = false;


		static const int AXIS_SIZE_Y= 15;
		//static const int SCROLL_BAR_WIDTH= 10;
		static const int SCROLL_BAR_WIDTH = 20;
		static const int MARGIN= 3;
		double timeSize;
		bool allowSameTimeKey;

		double timeSnapSize;

		bool rewriteOnChange;		//キー操作時に再描画を行うか否かのフラグ
		bool canMouseControll;		//マウスでの操作が可能か否かのフラグ

		bool dragLabel;
		bool dragTime;
		bool dragScrollBarLabel;
		bool dragScrollBarTime;

		bool dragSelect;		//ドラッグでの範囲選択
		double dragSelectTime1,dragSelectTime2;
		int dragSelectLine1,dragSelectLine2;

		bool dragShift;			//ドラッグでのキー移動
	};

	///<summary>
	///	ウィンドウ内部品"オイラーグラフ"クラス
	///</summary>
	class OWP_EulerGraph : public OrgWindowParts {
	public:
		//////////////////// Constructor/Destructor //////////////////////
		OWP_EulerGraph(const std::basic_string<TCHAR> &_name = _T(""), const double &_maxTime = 1.0, const double &_timeSize = 8.0) : OrgWindowParts() {

			TIME_ERROR_WIDTH = 0.0001;

			//常識：コンストラクタではthisはまだ使えない！！！
			//lineData.push_back(new EulLineData(0, 0, _T("X"), this, 0));
			//lineData.push_back(new EulLineData(0, 0, _T("Y"), this, 1));
			//lineData.push_back(new EulLineData(0, 0, _T("Z"), this, 2));
			//lineData.push_back(new EulLineData(0, 0, _T("S"), this, 3));
			//lineData.push_back(new EulLineData(0, 0, _T("X"), 0, 0));
			//lineData.push_back(new EulLineData(0, 0, _T("Y"), 0, 1));
			//lineData.push_back(new EulLineData(0, 0, _T("Z"), 0, 2));
			//lineData.push_back(new EulLineData(0, 0, _T("S"), 0, 3));
			currentmodel = 0;

			maxTime = _maxTime;
			timeSize = _timeSize;
			//cursorListener = []() {s_dummyfunc(); };
			//selectListener = []() {s_dummyfunc(); };
			//mouseMDownListener = []() {s_dummyfunc(); };
			//mouseWheelListener = []() {s_dummyfunc(); };
			//mouseRUpListener = []() {s_dummyfunc(); };
			cursorListener = NULL;
			selectListener = NULL;
			mouseMDownListener = NULL;
			mouseWheelListener = NULL;
			mouseRUpListener = NULL;

			//keyShiftListener = [this]() {
			//	shiftKeyTime(getShiftKeyTime());
			//};
			////keyDeleteListener = [](const KeyInfo& dummy) {s_dummyfunc(); };
			keyShiftListener = NULL;
			keyDeleteListener = NULL;

			showPos_time = 0;
			if (g_4kresolution) {
				showPos_width = 280.0;
			}
			else {
				//showPos_width = 76.0;
				showPos_width = 93.0;//2023/01/08
			}
			showPos_line = 0;
			currentTime = 0;
			currentLine = 0;

			allowSameTimeKey = false;

			timeSnapSize = 1.0;

			rewriteOnChange = true;
			canMouseControll = true;

			dragLabel = false;
			dragTime = false;
			dragScrollBarLabel = false;
			dragScrollBarTime = false;
			dragSelect = false;
			dragShift = false;
			wheeldelta = 0;
		
			mineul = 0.0;
			maxeul = 0.0;
			ikkind = 0;
			isseteulminmax = false;

			dispscale = 1.0;
			dispoffset = 0.0;

			if (g_4kresolution) {
				GRAPH_SIZE_Y = 170 * 2;
			}
			else {
				GRAPH_SIZE_Y = 170;
			}

			eultip = ChaVector3(0.0f, 0.0f, 0.0f);

		}
		~OWP_EulerGraph() {
			selectAll(true);
			deleteKey();

			std::vector<EulLineData*>::iterator it;
			for (it = lineData.begin(); it != lineData.end(); it++) {
				delete (*it);
			}
		}


		//static void OWP_EulerGraph::InitEulKeys();
		//static void OWP_EulerGraph::DestroyEulKeys();


		//////////////////////////// Method //////////////////////////////
		/// Method : 自動サイズ設定
		virtual void autoResize() {
			//size.y -= (size.y - SCROLL_BAR_WIDTH - AXIS_SIZE_Y - MARGIN * 2) % (LABEL_SIZE_Y - 1);
		}
		virtual void callRewrite();

		//	Method : 描画
		virtual void draw();
		//void draw() {
		//	//drawEdge();

		//	////時間軸目盛り
		//	//{
		//	//	const int AXIS_CURSOR_SIZE = 4;
		//	//	const int AXIS_LABEL_SIDE_MARGIN = 7;

		//	//	//目盛り線 & ラベル
		//	//	int x0 = pos.x + MARGIN + LABEL_SIZE_X;
		//	//	int x1 = pos.x + size.x - MARGIN - SCROLL_BAR_WIDTH;
		//	//	int y0 = pos.y + MARGIN;
		//	//	int y1 = y0 + AXIS_SIZE_Y + 1;
		//	//	for (int i = (int)showPos_time; i <= (int)maxTime; i++) {
		//	//		int xx = (int)(((double)i - showPos_time)*timeSize) + x0 + 1;

		//	//		if (x1 + AXIS_LABEL_SIDE_MARGIN <= xx) break;
		//	//		if (x0 - AXIS_LABEL_SIDE_MARGIN <= xx) {
		//	//			hdcM->setPenAndBrush(RGB(min(baseColor.r + 20, 255), min(baseColor.g + 20, 255), min(baseColor.b + 20, 255)), NULL);
		//	//			MoveToEx(hdcM->hDC, xx, y1 - 5, NULL);
		//	//			LineTo(hdcM->hDC, xx, y1);

		//	//			if (((i < 1000) && (i % 5 == 0)) || ((i >= 1000) && (i % 10 == 0))) {
		//	//				TCHAR tmpChar[20];
		//	//				_stprintf_s(tmpChar, 20, _T("%.3G"), (double)i);
		//	//				hdcM->setFont(12, _T("ＭＳ ゴシック"));
		//	//				SetTextColor(hdcM->hDC, RGB(240, 240, 240));
		//	//				TextOut(hdcM->hDC,
		//	//					xx - (int)((double)_tcslen(tmpChar)*2.0), y0,
		//	//					tmpChar, (int)_tcslen(tmpChar));
		//	//			}
		//	//		}
		//	//	}

		//	//	//カーソル
		//	//	int xx = (int)((currentTime - showPos_time)*timeSize) + x0 + 1;
		//	//	hdcM->setPenAndBrush(RGB(240, 240, 240), NULL);
		//	//	if (x0 - AXIS_CURSOR_SIZE <= xx && xx <= x1 + AXIS_CURSOR_SIZE) {
		//	//		for (int i = 0; i<AXIS_CURSOR_SIZE; i++) {
		//	//			MoveToEx(hdcM->hDC, xx - i, y1 - i - 2, NULL);
		//	//			LineTo(hdcM->hDC, xx + i + 1, y1 - i - 2);
		//	//		}
		//	//	}

		//	//	//枠
		//	//	hdcM->setPenAndBrush(NULL, RGB(baseColor.r, baseColor.g, baseColor.b));
		//	//	Rectangle(hdcM->hDC, pos.x, y0, x0, y1);
		//	//	Rectangle(hdcM->hDC, x1, y0, pos.x + size.x, y1);
		//	//	hdcM->setPenAndBrush(RGB(min(baseColor.r + 20, 255), min(baseColor.g + 20, 255), min(baseColor.b + 20, 255)), NULL);
		//	//	Rectangle(hdcM->hDC, x0, y0, x1, y1);
		//	//}

		//	//drawEdge(false);

		//	//行データ
		//	//int showLineNum = (size.y - SCROLL_BAR_WIDTH - AXIS_SIZE_Y - MARGIN * 2) / (LABEL_SIZE_Y - 1);
		//	int showLineNum = 3;
		//	//for (int i = showPos_line, j = 0; i<(int)lineData.size() && j<showLineNum; i++, j++) {
		//	for(int i = 0; i < showLineNum; i++){
		//		bool highLight = false;
		//		//if (i == currentLine) highLight = true;
		//		if (i >= 0) {
		//			lineData[i]->draw(hdcM,
		//				pos.x + MARGIN,
		//				//pos.y + MARGIN + AXIS_SIZE_Y + j*(LABEL_SIZE_Y - 1),
		//				pos.y + MARGIN + AXIS_SIZE_Y,
		//				size.x - SCROLL_BAR_WIDTH - MARGIN * 2,
		//				timeSize, showPos_time, highLight);
		//		}
		//	}

		//	////ドラッグによる選択範囲
		//	//if (dragSelect && dragSelectTime1 != dragSelectTime2) {
		//	//	int xx0 = pos.x + MARGIN + LABEL_SIZE_X + 1;
		//	//	int yy0 = pos.y + MARGIN + AXIS_SIZE_Y;
		//	//	int xx1 = pos.x + size.x - MARGIN - SCROLL_BAR_WIDTH - 1;
		//	//	int yy1 = pos.y + size.y - MARGIN - SCROLL_BAR_WIDTH;
		//	//	int x0 = xx0 + (int)((min(dragSelectTime1, dragSelectTime2) - showPos_time)* timeSize);
		//	//	int x1 = xx0 + (int)((max(dragSelectTime1, dragSelectTime2) - showPos_time)* timeSize);
		//	//	int y0 = yy0 + (min(dragSelectLine1, dragSelectLine2) - showPos_line)* (LABEL_SIZE_Y - 1) + 1;
		//	//	int y1 = yy0 + (max(dragSelectLine1, dragSelectLine2) - showPos_line + 1)* (LABEL_SIZE_Y - 1) - 1;

		//	//	{//枠描画
		//	//		hdcM->setPenAndBrush(RGB(min(baseColor.r + 20, 255), min(baseColor.g + 20, 255), min(baseColor.b + 20, 255)), NULL);
		//	//		if (xx0 <= x0) {		//左枠
		//	//			MoveToEx(hdcM->hDC, x0, max(yy0, y0 + 1), NULL);
		//	//			LineTo(hdcM->hDC, x0, min(y1, yy1));
		//	//		}
		//	//		if (x1 <= xx1) {		//右枠
		//	//			MoveToEx(hdcM->hDC, x1, max(yy0, y0 + 1), NULL);
		//	//			LineTo(hdcM->hDC, x1, min(y1, yy1));
		//	//		}
		//	//		if (yy0 <= y0) {		//上枠
		//	//			MoveToEx(hdcM->hDC, max(xx0, x0 + 1), y0, NULL);
		//	//			LineTo(hdcM->hDC, min(x1, xx1), y0);
		//	//		}
		//	//		if (y1 <= yy1) {		//下枠
		//	//			MoveToEx(hdcM->hDC, max(xx0, x0 + 1), y1, NULL);
		//	//			LineTo(hdcM->hDC, min(x1, xx1), y1);
		//	//		}
		//	//	}
		//	//}

		//	////時間軸スクロールバー
		//	//{
		//	//	int x0 = pos.x + MARGIN + LABEL_SIZE_X;
		//	//	int x1 = pos.x + size.x - MARGIN - SCROLL_BAR_WIDTH;
		//	//	int y0 = pos.y + size.y - MARGIN - SCROLL_BAR_WIDTH;
		//	//	int y1 = y0 + SCROLL_BAR_WIDTH;

		//	//	//枠
		//	//	hdcM->setPenAndBrush(RGB(min(baseColor.r + 20, 255), min(baseColor.g + 20, 255), min(baseColor.b + 20, 255)), NULL);
		//	//	Rectangle(hdcM->hDC, x0, y0, x1, y1);

		//	//	//中身
		//	//	double showTimeLength = ((double)(x1 - x0 - 3)) / timeSize;
		//	//	double barSize = ((double)(x1 - x0 - 4))*showTimeLength / maxTime;
		//	//	double barStart = ((double)(x1 - x0 - 4))*showPos_time / maxTime;
		//	//	if (showTimeLength<maxTime) {
		//	//		hdcM->setPenAndBrush(NULL, RGB(min(baseColor.r + 20, 255), min(baseColor.g + 20, 255), min(baseColor.b + 20, 255)));
		//	//		Rectangle(hdcM->hDC, x0 + 2 + (int)barStart, y0 + 2, x0 + 2 + (int)(barStart + barSize), y1 - 2);
		//	//	}
		//	//}

		//	////ラベルスクロールバー
		//	//{
		//	//	int x0 = pos.x + size.x - MARGIN - SCROLL_BAR_WIDTH - 1;
		//	//	int x1 = x0 + SCROLL_BAR_WIDTH + 1;
		//	//	int y0 = pos.y + MARGIN + AXIS_SIZE_Y;
		//	//	int y1 = pos.y + size.y - MARGIN - SCROLL_BAR_WIDTH + 1;

		//	//	//枠
		//	//	hdcM->setPenAndBrush(RGB(min(baseColor.r + 20, 255), min(baseColor.g + 20, 255), min(baseColor.b + 20, 255)), NULL);
		//	//	Rectangle(hdcM->hDC, x0, y0, x1, y1);

		//	//	//中身
		//	//	int barSize = (y1 - y0 - 4)*showLineNum / (int)lineData.size();
		//	//	int barStart = (y1 - y0 - 4)*showPos_line / (int)lineData.size();
		//	//	if (showLineNum<(int)lineData.size()) {
		//	//		hdcM->setPenAndBrush(NULL, RGB(min(baseColor.r + 20, 255), min(baseColor.g + 20, 255), min(baseColor.b + 20, 255)));
		//	//		Rectangle(hdcM->hDC, x0 + 2, y0 + 2 + barStart, x1 - 2, y0 + 2 + barStart + barSize + 1);
		//	//	}
		//	//}
		//}
		//	Method : 行を追加	(既に同名のキーがある場合はFalseを返す)
		bool newLine(int _depth, int nullflag, const std::basic_string<TCHAR>& _name) {
			for (int i = 0; i<(int)lineData.size(); i++) {
				if (lineData[i]->name == _name) {
					return false;
				}
			}
			EulLineData* neweulline = new EulLineData(_depth, nullflag, _name, this, (int)lineData.size());
			lineData.push_back(neweulline);

			//再描画要求
			if (rewriteOnChange) {
				callRewrite();
			}
			return true;
		}
		//	Method : 行を削除
		void deleteLine() {
			for (int i = 0; i<(int)lineData.size(); i++) {
				delete lineData[i];
			}
			lineData.clear();

			//再描画要求
			if (rewriteOnChange) {
				callRewrite();
			}
		}
		bool deleteLine(const std::basic_string<TCHAR>& _name) {
			int popPos = -1;
			for (int i = 0; i<(int)lineData.size(); i++) {
				if (lineData[i]->name == _name) {
					delete lineData[i];
					popPos = i;
				}
			}
			if (popPos == -1) return false;
			for (int i = popPos + 1; i<(int)lineData.size(); i++) {
				lineData[i - 1] = lineData[i];
				lineData[i - 1]->lineIndex = i - 1;
			}
			lineData.pop_back();

			//再描画要求
			if (rewriteOnChange) {
				callRewrite();
			}
			return true;
		}
		bool deleteLine(int index) {
			if ((unsigned int)lineData.size() <= (unsigned int)index) return false;

			bool ret = deleteLine(lineData[index]->name);

			//再描画要求
			if (ret && rewriteOnChange) {
				callRewrite();
			}
			return ret;
		}
		//	Method : キーを追加
		bool newKey(bool needCallRewrite, const std::basic_string<TCHAR>& _name, const double &time, double _value = 0.0, const double &length = 1.0) {
			for (int i = 0; i<(int)lineData.size(); i++) {
				if (lineData[i]->name == _name) {
					bool ret = lineData[i]->newKey(time, 0, _value, length);

					//再描画要求
					if (needCallRewrite && ret && rewriteOnChange) {
						callRewrite();
					}
					return ret;
				}
			}
			return false;
		}
		//	Method : キーに値をセット
		bool setKey(bool needCallRewrite, const std::basic_string<TCHAR>& _name, const double &time, double _value = 0.0) {
			for (int i = 0; i<(int)lineData.size(); i++) {
				if (lineData[i]->name == _name) {
					bool ret = lineData[i]->setKey(time, 0, _value);

					//再描画要求
					if (needCallRewrite && ret && rewriteOnChange) {
						callRewrite();
					}
					return ret;
				}
			}
			return false;
		}
		////	Method : キーを削除
		//bool deleteKey(const std::basic_string<TCHAR>& _name, double time) {
		//	for (int i = 0; i<(int)lineData.size(); i++) {
		//		if (lineData[i]->name == _name) {
		//			selectClear();
		//			bool ret = lineData[i]->selectKey(time);
		//			lineData[i]->deleteKey();

		//			//再描画要求
		//			if (ret && rewriteOnChange) {
		//				callRewrite();
		//			}
		//			return ret;
		//		}
		//	}
		//	return false;
		//}
		//bool deleteKey(const std::basic_string<TCHAR>& _name, int index) {
		//	for (int i = 0; i<(int)lineData.size(); i++) {
		//		if (lineData[i]->name == _name) {
		//			bool ret = lineData[i]->deleteKey(index);

		//			//再描画要求
		//			if (ret && rewriteOnChange) {
		//				callRewrite();
		//			}
		//			return ret;
		//		}
		//	}
		//	return false;
		//}
		//bool deleteKey(int lineIndex, double time) {
		//	if ((unsigned int)lineData.size() <= (unsigned int)lineIndex) return false;

		//	selectClear();
		//	bool ret = lineData[lineIndex]->selectKey(time);
		//	lineData[lineIndex]->deleteKey();

		//	//再描画要求
		//	if (ret && rewriteOnChange) {
		//		callRewrite();
		//	}

		//	return ret;
		//}
		//bool deleteKey(int lineIndex, int keyIndex) {
		//	if ((unsigned int)lineData.size() <= (unsigned int)lineIndex) return false;

		//	bool ret = lineData[lineIndex]->deleteKey(keyIndex);

		//	//再描画要求
		//	if (ret && rewriteOnChange) {
		//		callRewrite();
		//	}

		//	return ret;
		//}
		unsigned int deleteKey() {
			unsigned int deleteNum = 0;

			for (int i = 0; i<(int)lineData.size(); i++) {
				deleteNum += lineData[i]->deleteKey();
			}

			//再描画要求
			if ((deleteNum != 0) && rewriteOnChange) {
				callRewrite();
			}

			return deleteNum;
		}
		//	Method : すべてのキーの選択を解除する
		void selectClear(bool noCallListener = false) {
			for (int i = 0; i<(int)lineData.size(); i++) {
				lineData[i]->selectClear();
			}

			//リスナーコール
			if (!noCallListener && (this->selectListener != NULL)) {
				(this->selectListener)();
			}

			//再描画要求
			if (rewriteOnChange) {
				callRewrite();
			}
		}
		//	Method : すべてのキーを選択する
		void selectAll(bool noCallListener = false) {
			for (int i = 0; i<(int)lineData.size(); i++) {
				lineData[i]->selectAll();
			}

			//リスナーコール
			if (!noCallListener && (this->selectListener != NULL)) {
				(this->selectListener)();
			}

			//再描画要求
			if (rewriteOnChange) {
				callRewrite();
			}
		}

		void OnButtonSelect(double startframe, double endframe, int tothelastflag)
		{
			if ((g_endappflag == 0) && parentWindow && IsWindow(parentWindow->getHWnd())) {

				this->selectClear();

				double tmpstart, tmpend;
				if (tothelastflag == 0) {
					if (startframe <= endframe) {
						tmpstart = startframe;
						tmpend = endframe;
					}
					else {
						tmpstart = endframe;
						tmpend = startframe;
					}
				}
				else {
					tmpstart = startframe;
					tmpend = startframe;
				}

				//double curframe = getCurrentTime();
				double maxframe = startframe;

				//for (int j = 0; j < (int)lineData.size(); j++){
				int j = 1;
				if (j < (int)lineData.size()) {
					EulLineData* curLineData = lineData[j];
					if (curLineData) {
						if (tothelastflag == 1) {

							//maxframe = (double)(curLineData->key.size() - 1);

							int startindex = curLineData->getKeyIndex(tmpstart);
							if (startindex >= 0) {
								int currentkeynum = (int)curLineData->key.size();
								for (int i = startindex; i < currentkeynum; i++) {
									if (curLineData->key[i]->time >= (tmpstart - TIME_ERROR_WIDTH)) {
										curLineData->key[i]->select = true;
										if (maxframe < curLineData->key[i]->time) {
											maxframe = curLineData->key[i]->time;
										}
									}
								}
							}
							curLineData->setMinSelected(startindex);
							curLineData->setMaxSelected(max(0, (int)curLineData->key.size() - 1));

							//for (int i = 0; i < (int)curLineData->key.size(); i++) {
							//	if (curLineData->key[i]->time >= (tmpstart - TIME_ERROR_WIDTH)) {
							//		curLineData->key[i]->select = true;
							//		if (maxframe < curLineData->key[i]->time) {
							//			maxframe = curLineData->key[i]->time;
							//		}
							//	}
							//}
						}
						else {

							//maxframe = tmpend;

							int startindex = curLineData->getKeyIndex(tmpstart);
							int endindex = curLineData->getKeyIndex(tmpend);
							if ((startindex >= 0) && (endindex >= 0)) {
								for (int i = startindex; i <= endindex; i++) {
									if ((curLineData->key[i]->time >= (tmpstart - TIME_ERROR_WIDTH)) &&
										(curLineData->key[i]->time <= (tmpend + TIME_ERROR_WIDTH))) {
										curLineData->key[i]->select = true;
										if (maxframe < curLineData->key[i]->time) {
											maxframe = curLineData->key[i]->time;
										}
									}
								}
							}
							curLineData->setMinSelected(startindex);
							curLineData->setMaxSelected(endindex);


							//for (int i = 0; i < (int)curLineData->key.size(); i++) {
							//	if ((curLineData->key[i]->time >= (tmpstart - TIME_ERROR_WIDTH)) &&
							//		(curLineData->key[i]->time <= (tmpend + TIME_ERROR_WIDTH))) {
							//		curLineData->key[i]->select = true;
							//		if (maxframe < curLineData->key[i]->time) {
							//			maxframe = curLineData->key[i]->time;
							//		}
							//	}
							//}
						}
					}
				}


				//dragSelect = true;
				dragSelectTime1 = tmpstart;
				dragSelectTime2 = maxframe;
				//showPos_time = tmpstart;
				showPos_line = 0;
				currentLine = 1;//!!!!!!!

								//再描画領域
				RECT tmpRect;
				tmpRect.left = pos.x + 1;
				tmpRect.top = pos.y + 1;
				tmpRect.right = pos.x + size.x - 1;
				tmpRect.bottom = pos.y + size.y - 1;
				InvalidateRect(parentWindow->getHWnd(), &tmpRect, false);

				//再描画要求
				//if (rewriteOnChange){
				callRewrite();
				//}
			}
		}

		/// Method : すべての選択されているキーを取得する
		std::list<KeyInfo> getSelectedKey() const {
			std::list<KeyInfo> ret;
			ret.clear();

			for (int i = 0; i<(int)lineData.size(); i++) {
				EulLineData *curLineData = lineData[i];
				if (curLineData) {
					int curkeynum = (int)curLineData->key.size();

					//for (int j = 0; j<curkeynum; j++) {
					int startindex = max(curLineData->getMinSelected(), 0);
					int endindex = min((curLineData->getMaxSelected() + 1), curkeynum);
					if ((startindex >= 0) && (endindex >= 0)) {
						for (int j = startindex; j < endindex; j++) {//2022/09/12
							EulLineData::EulKey* curKey = curLineData->key[j];

							if (curKey->select) {
								KeyInfo tmp;
								tmp.label = curLineData->name.c_str();
								tmp.time = curKey->time;
								tmp.lineIndex = i;
								tmp.timeIndex = j;
								tmp.object = NULL;// curKey->object;
								ret.push_back(tmp);
							}
						}
					}
				}
				
			}
			if ((ret.size() == 1) && ((*ret.begin()).time != currentTime)) {
				//初期状態。セレクトではない。
				(lineData[(*ret.begin()).lineIndex])->key[(*ret.begin()).timeIndex]->select = false;
				ret.clear();
			}

			//_ASSERT(0);
			return ret;
		}
		///// Method : 全ての選択されているキーを移動する
		//void shiftKeyTime(const double &shiftTime) {
		//	for (int i = 0; i<(int)lineData.size(); i++) {
		//		lineData[i]->shiftKey(shiftTime);
		//	}

		//	//再描画要求
		//	if (rewriteOnChange) {
		//		callRewrite();
		//	}
		//}
		///	Method : マウスダウンイベント受信
		virtual void onLButtonDown(const MouseEvent& e) {
			selectClear(true);

			if (!canMouseControll) return;
			if (g_underselecttolast) return;
			if (g_undereditrange) return;

			int x0 = MARGIN;
			int x1 = x0 + LABEL_SIZE_X;
			int x2 = size.x - MARGIN - SCROLL_BAR_WIDTH;
			int x3 = size.x - MARGIN;
			int y0 = MARGIN;
			int y1 = y0 + AXIS_SIZE_Y + 1;
			int y2 = size.y - MARGIN - SCROLL_BAR_WIDTH;
			int y3 = size.y - MARGIN;

			//ラベル
			if ((x0 <= e.localX) && (e.localX < x2) && 
				(y1 <= e.localY) && (e.localY < y2)) {
				setCurrentLine(showPos_line + (e.localY - y1) / (LABEL_SIZE_Y - 1));

				dragLabel = true;
			}

			//時間軸目盛り
			if (((x1 - 2) <= e.localX) && (e.localX < x2) && 
				(y0 <= e.localY) && (e.localY < y2)) {
				setCurrentTime(showPos_time + (double)(e.localX - x1) / timeSize);

				dragTime = true;
			}

			{//ドラッグでの範囲選択
				dragSelectTime1 = currentTime;
				dragSelectLine1 = currentLine;
			}

			//時間軸スクロールバー
			if ((x1 <= e.localX) && (e.localX < x2) && 
				(y2 <= e.localY) && (e.localY < y3)) {
				int xx0 = MARGIN + LABEL_SIZE_X;
				int xx1 = size.x - MARGIN - SCROLL_BAR_WIDTH;

				double showTimeLength = ((double)(xx1 - xx0 - 3)) / timeSize;
				if (showTimeLength<maxTime) {
					double barSize = ((double)(xx1 - xx0 - 4))*showTimeLength / maxTime;

					int movableX = xx1 - xx0 - (int)barSize;
					int movableXStart = xx0 + (int)barSize / 2;

					setShowPosTime(((double)(e.localX - movableXStart))*(maxTime - showTimeLength) / (double)movableX);

					dragScrollBarTime = true;
				}
			}

			//ラベルスクロールバー
			if ((x2 <= e.localX) && (e.localX < x3) && 
				(y1 <= e.localY) && (e.localY < y2)) {
				int yy0 = MARGIN + AXIS_SIZE_Y;
				int yy1 = size.y - MARGIN - SCROLL_BAR_WIDTH + 1;

				int showLineNum = (size.y - SCROLL_BAR_WIDTH - AXIS_SIZE_Y - MARGIN * 2) / (LABEL_SIZE_Y - 1);
				if (showLineNum<(int)lineData.size()) {
					int barSize = (yy1 - yy0 - 4)*showLineNum / (int)lineData.size();

					int movableY = yy1 - yy0 - barSize;
					int movableYStart = yy0 + barSize / 2;

					setShowPosLine((e.localY - movableYStart)*((int)lineData.size() - showLineNum) / movableY);

					dragScrollBarLabel = true;
				}
			}

			//Ctrl+ドラッグによるキー移動
			if (e.ctrlKey && !dragScrollBarTime && !dragScrollBarLabel) {
				dragShift = true;
			}

		}
		///	Method : 左マウスボタンアップイベント受信
		virtual void onLButtonUp(const MouseEvent& e) {
			if ((g_endappflag == 0) && parentWindow && IsWindow(parentWindow->getHWnd())) {

				if (!canMouseControll) return;
				if (g_underselecttolast || g_undereditrange) {
					g_underselecttolast = false;
					g_undereditrange = false;

					//ドラッグフラグを初期化
					dragLabel = false;
					dragTime = false;
					dragScrollBarLabel = false;
					dragScrollBarTime = false;
					dragSelect = false;
					dragShift = false;

					return;
				}

				//ドラッグ選択範囲内のキーを選択状態にする
				if (!dragShift && !dragScrollBarLabel && !dragScrollBarTime) {
					selectClear(true);
					if (dragSelect) {

						for (int i = min(dragSelectLine1, dragSelectLine2);
							i <= max(dragSelectLine1, dragSelectLine2) && i < (signed int)lineData.size(); i++) {
							lineData[i]->selectKey(min(dragSelectTime1, dragSelectTime2),
								max(dragSelectTime1, dragSelectTime2));
						}
					}
					//リスナーコール
					if (this->selectListener != NULL) {
						(this->selectListener)();
					}
				}

				//Ctrl+ドラッグによるキー移動
				if (dragShift) {
					//リスナーコール
					if (this->keyShiftListener != NULL) {
						(this->keyShiftListener)();
					}
					ghostShiftTime = 0.0;
				}
				//ドラッグフラグを初期化
				dragLabel = false;
				dragTime = false;
				dragScrollBarLabel = false;
				dragScrollBarTime = false;
				dragSelect = false;
				dragShift = false;

				//再描画領域
				RECT tmpRect;
				tmpRect.left = pos.x + 1;
				tmpRect.top = pos.y + 1;
				tmpRect.right = pos.x + size.x - 1;
				tmpRect.bottom = pos.y + size.y - 1;
				InvalidateRect(parentWindow->getHWnd(), &tmpRect, false);
			}
		}
		//	Method : 左マウスボタン ダブルクリックイベント受信
		virtual void onLButtonDBLCLK(const MouseEvent& e) {//2023/10/04
		}
		//	Method : 右マウスボタン ダブルクリックイベント受信
		virtual void onRButtonDBLCLK(const MouseEvent& e) {//2023/10/04
			int dbgflag1 = 1;
		}

		virtual void onRButtonDown(const MouseEvent& e) {
			selectClear(true);

			if (!canMouseControll) return;
			if (g_underselecttolast) return;
			if (g_undereditrange) return;

			int x0 = MARGIN;
			int x1 = x0 + LABEL_SIZE_X;
			int x2 = size.x - MARGIN - SCROLL_BAR_WIDTH;
			int x3 = size.x - MARGIN;
			int y0 = MARGIN;
			int y1 = y0 + AXIS_SIZE_Y + 1;
			int y2 = size.y - MARGIN - SCROLL_BAR_WIDTH;
			int y3 = size.y - MARGIN;

			//ラベル
			if ((x0 <= e.localX) && (e.localX < x2) && 
				(y1 <= e.localY) && (e.localY < y2)) {
				setCurrentLine(showPos_line + (e.localY - y1) / (LABEL_SIZE_Y - 1));

				//dragLabel = true;
			}

			/*
			//時間軸目盛り
			if (x1 - 2 <= e.localX && e.localX<x2
			&& y0 <= e.localY && e.localY<y2){
			setCurrentTime(showPos_time + (double)(e.localX - x1) / timeSize);

			dragTime = true;
			}

			{//ドラッグでの範囲選択
			dragSelectTime1 = currentTime;
			dragSelectLine1 = currentLine;
			}

			//時間軸スクロールバー
			if (x1 <= e.localX && e.localX<x2
			&& y2 <= e.localY && e.localY<y3){
			int xx0 = MARGIN + LABEL_SIZE_X;
			int xx1 = size.x - MARGIN - SCROLL_BAR_WIDTH;

			double showTimeLength = ((double)(xx1 - xx0 - 3)) / timeSize;
			if (showTimeLength<maxTime){
			double barSize = ((double)(xx1 - xx0 - 4))*showTimeLength / maxTime;

			int movableX = xx1 - xx0 - (int)barSize;
			int movableXStart = xx0 + (int)barSize / 2;

			setShowPosTime(((double)(e.localX - movableXStart))*(maxTime - showTimeLength) / (double)movableX);

			dragScrollBarTime = true;
			}
			}

			//ラベルスクロールバー
			if (x2 <= e.localX && e.localX<x3
			&& y1 <= e.localY && e.localY<y2){
			int yy0 = MARGIN + AXIS_SIZE_Y;
			int yy1 = size.y - MARGIN - SCROLL_BAR_WIDTH + 1;

			int showLineNum = (size.y - SCROLL_BAR_WIDTH - AXIS_SIZE_Y - MARGIN * 2) / (LABEL_SIZE_Y - 1);
			if (showLineNum<(int)lineData.size()){
			int barSize = (yy1 - yy0 - 4)*showLineNum / (int)lineData.size();

			int movableY = yy1 - yy0 - barSize;
			int movableYStart = yy0 + barSize / 2;

			setShowPosLine((e.localY - movableYStart)*((int)lineData.size() - showLineNum) / movableY);

			dragScrollBarLabel = true;
			}
			}

			//Ctrl+ドラッグによるキー移動
			if (e.ctrlKey && !dragScrollBarTime && !dragScrollBarLabel){
			dragShift = true;
			}
			*/
		}
		///	Method : 左マウスボタンアップイベント受信
		virtual void onRButtonUp(const MouseEvent& e) {
			if ((g_endappflag == 0) && parentWindow && IsWindow(parentWindow->getHWnd())) {

				if (!canMouseControll) return;
				if (g_underselecttolast || g_undereditrange) {
					g_underselecttolast = false;
					g_undereditrange = false;

					//ドラッグフラグを初期化
					dragLabel = false;
					dragTime = false;
					dragScrollBarLabel = false;
					dragScrollBarTime = false;
					dragSelect = false;
					dragShift = false;

					return;
				}

				/*
				//ドラッグ選択範囲内のキーを選択状態にする
				if (!dragShift && !dragScrollBarLabel && !dragScrollBarTime){
				selectClear(true);
				if (dragSelect){

				for (int i = min(dragSelectLine1, dragSelectLine2);
				i <= max(dragSelectLine1, dragSelectLine2) && i < (signed int)lineData.size(); i++){
				lineData[i]->selectKey(min(dragSelectTime1, dragSelectTime2),
				max(dragSelectTime1, dragSelectTime2));
				}
				}
				//リスナーコール
				if (this->selectListener != NULL){
				(this->selectListener)();
				}
				}

				//Ctrl+ドラッグによるキー移動
				if (dragShift){
				//リスナーコール
				if (this->keyShiftListener != NULL){
				(this->keyShiftListener)();
				}
				ghostShiftTime = 0.0;
				}
				*/
				//ドラッグフラグを初期化
				dragLabel = false;
				dragTime = false;
				dragScrollBarLabel = false;
				dragScrollBarTime = false;
				dragSelect = false;
				dragShift = false;

				//再描画領域
				RECT tmpRect;
				tmpRect.left = pos.x + 1;
				tmpRect.top = pos.y + 1;
				tmpRect.right = pos.x + size.x - 1;
				tmpRect.bottom = pos.y + size.y - 1;
				InvalidateRect(parentWindow->getHWnd(), &tmpRect, false);

				if (this->mouseRUpListener != NULL) {
					(this->mouseRUpListener)();
				}
			}
		}

		///	Method : マウス移動イベント受信
		virtual void onMouseMove(const MouseEvent& e) {
			if (!canMouseControll) return;
			if (g_underselecttolast) return;
			if (g_undereditrange) return;

			int x0 = MARGIN;
			int x1 = x0 + LABEL_SIZE_X;
			int x2 = size.x - MARGIN - SCROLL_BAR_WIDTH;
			int x3 = size.x - MARGIN;
			int y0 = MARGIN;
			int y1 = y0 + AXIS_SIZE_Y + 1;
			int y2 = size.y - MARGIN - SCROLL_BAR_WIDTH;
			int y3 = size.y - MARGIN;

			//ラベル
			bool callCursorListener = false;
			if (dragLabel) {
				int oldLine = currentLine;
				setCurrentLine(showPos_line + (e.localY - y1) / (LABEL_SIZE_Y - 1), true);
				if (oldLine != currentLine) {
					callCursorListener = true;
				}
			}

			//時間軸目盛り
			if (dragTime) {
				double oldTime = currentTime;
				setCurrentTime(showPos_time + (double)(e.localX - x1) / timeSize, true);
				if (oldTime != currentTime) {
					callCursorListener = true;
				}
			}

			//カーソルリスナーコール
			if (callCursorListener && (this->cursorListener != NULL)) {
				(this->cursorListener)();
			}

			//Ctrl+ドラッグでのキー移動
			if (dragShift) {
				ghostShiftTime = currentTime - dragSelectTime1;
			}

			//ドラッグでの範囲選択
			if (!dragShift && (dragSelect || dragLabel || dragTime) && !dragScrollBarLabel && !dragScrollBarTime) {
				dragSelect = true;
				dragSelectTime2 = currentTime;
				dragSelectLine2 = currentLine;
			}

			//ドラッグ選択範囲内のキーを選択状態にする
			if (dragSelect) {
				selectClear(true);
				for (int i = min(dragSelectLine1, dragSelectLine2);
					i <= max(dragSelectLine1, dragSelectLine2) && i<(signed int)lineData.size(); i++) {
					lineData[i]->selectKey(min(dragSelectTime1, dragSelectTime2),
						max(dragSelectTime1, dragSelectTime2));
				}
			}

			//時間軸スクロールバー
			if (dragScrollBarTime) {
				int xx0 = MARGIN + LABEL_SIZE_X;
				int xx1 = size.x - MARGIN - SCROLL_BAR_WIDTH;

				double showTimeLength = ((double)(xx1 - xx0 - 3)) / timeSize;
				double barSize = ((double)(xx1 - xx0 - 4))*showTimeLength / maxTime;

				int movableX = xx1 - xx0 - (int)barSize;
				int movableXStart = xx0 + (int)barSize / 2;

				setShowPosTime(((double)(e.localX - movableXStart))*(maxTime - showTimeLength) / (double)movableX);
			}

			//ラベルスクロールバー
			if (dragScrollBarLabel) {
				int yy0 = MARGIN + AXIS_SIZE_Y;
				int yy1 = size.y - MARGIN - SCROLL_BAR_WIDTH + 1;

				int showLineNum = (size.y - SCROLL_BAR_WIDTH - AXIS_SIZE_Y - MARGIN * 2) / (LABEL_SIZE_Y - 1);
				int barSize = (yy1 - yy0 - 4)*showLineNum / (int)lineData.size();

				int movableY = yy1 - yy0 - barSize;
				int movableYStart = yy0 + barSize / 2;

				setShowPosLine((e.localY - movableYStart)*((int)lineData.size() - showLineNum) / movableY);
			}
		}
		virtual void onMButtonDown(const MouseEvent& e) {
			if (!canMouseControll) return;

			int x0 = MARGIN;
			int x1 = x0 + LABEL_SIZE_X;
			int x2 = size.x - MARGIN - SCROLL_BAR_WIDTH;
			int x3 = size.x - MARGIN;
			int y0 = MARGIN;
			int y1 = y0 + AXIS_SIZE_Y + 1;
			int y2 = size.y - MARGIN - SCROLL_BAR_WIDTH;
			int y3 = size.y - MARGIN;

			//ラベル
			if ((x0 <= e.localX) && (e.localX < x2) && 
				(y1 <= e.localY) && (e.localY < y2)) {
				setCurrentLine(showPos_line + (e.localY - y1) / (LABEL_SIZE_Y - 1));

				//dragLabel = true;
			}

			//時間軸目盛り
			if (((x1 - 2) <= e.localX) && (e.localX < x2) && 
				(y0 <= e.localY) && (e.localY < y2)) {
				setCurrentTime(showPos_time + (double)(e.localX - x1) / timeSize);

				//dragTime = true;
			}
			if (this->mouseMDownListener != NULL) {
				(this->mouseMDownListener)();
			}
		}
		virtual void onMButtonUp(const MouseEvent& e) {
			if ((g_endappflag == 0) && parentWindow && IsWindow(parentWindow->getHWnd())) {

				if (!canMouseControll) return;

				//ドラッグフラグを初期化
				dragLabel = false;
				dragTime = false;
				dragScrollBarLabel = false;
				dragScrollBarTime = false;
				dragSelect = false;
				dragShift = false;

				//再描画領域
				RECT tmpRect;
				tmpRect.left = pos.x + 1;
				tmpRect.top = pos.y + 1;
				tmpRect.right = pos.x + size.x - 1;
				tmpRect.bottom = pos.y + size.y - 1;
				InvalidateRect(parentWindow->getHWnd(), &tmpRect, false);
			}
		}
		virtual void onMouseWheel(const MouseEvent& e) {
			wheeldelta = e.wheeldelta;
			if (this->mouseWheelListener != NULL) {
				(this->mouseWheelListener)();
			}
		}
		int getMouseWheelDelta() { return wheeldelta; }
		/// Method : 行インデックスを取得する
		int getLineIndex(const std::basic_string<TCHAR>& _name) const {
			for (int i = 0; i<(int)lineData.size(); i++) {
				if (lineData[i]->name == _name) {
					return i;
				}
			}
			return -1;
		}

		void WheelShowPosTime()
		{
			double newshowpostime;
			if (wheeldelta > 0) {
				newshowpostime = min((showPos_time + 1.0), maxTime);
			}
			else {
				newshowpostime = max(0, (showPos_time - 1.0));
			}
			showPos_time = newshowpostime;

			for (int i = 0; i < (int)lineData.size(); i++) {
				lineData[i]->selectKey(showPos_time);
			}


			//再描画要求
			//if (rewriteOnChange) {
			//callRewrite();//Main.cpp refreshEulerGraphで再描画する
			//}

			//setShowPosTime(((double)(e.localX - movableXStart)) * (maxTime - showTimeLength) / (double)movableX);
			//dragScrollBarTime = true;
		}



		/////////////////////////// Accessor /////////////////////////////
		//	Accessor : maxTime
		void SetCurrentModel(CModel* srcmodel) {
			currentmodel = srcmodel;
		}
		CModel* GetCurrentModel()
		{
			return currentmodel;
		}

		void ResetScaleAndOffset()
		{
			dispscale = 1.0;
			dispoffset = 0.0;
		}
		double getDispScale()
		{
			return dispscale;
		}
		void setDispScale(double srcscale)
		{
			dispscale = srcscale;
		}
		void PlusDisp()
		{
			dispscale += 0.25;
		}
		void MinusDisp()
		{
			if ((dispscale - 0.25) > 0.0) {
				dispscale -= 0.25;
			}
		}
		double getDispOffset()
		{
			return dispoffset;
		}
		void setDispOffset(double srcoffset)
		{
			dispoffset = srcoffset;
		}
		void PlusOffset()
		{
			dispoffset += 5.0;
		}
		void MinusOffset()
		{
			dispoffset -= 5.0;
		}

		double getMaxTime() const {
			return maxTime;
		}
		void setMaxTime(double _maxTime) {
			maxTime = max(_maxTime, 0);
			currentTime = min(currentTime, maxTime);

			//再描画要求
			if (rewriteOnChange) {
				callRewrite();
			}
		}
		/// Accessor : timeSnapSize
		double getTimeSnapSize() const {
			return timeSnapSize;
		}
		void setTimeSnapSize(const double &value) {
			timeSnapSize = value;
		}
		double getShowposWidth() const {
			return showPos_width;
		}
		/// Accessor : rewriteOnChange
		bool getRewriteOnChangeFlag() const {
			return rewriteOnChange;
		}
		void setRewriteOnChangeFlag(bool value) {
			rewriteOnChange = value;
		}
		/// Accessor : canMouseControll
		bool getCanMouseControllFlag() const {
			return canMouseControll;
		}
		void setCanMouseControllFlag(bool value) {
			canMouseControll = value;
		}
		//	Accessor : currentTime
		double getCurrentTime() const {
			return currentTime;
		}
		void setCurrentTime(double _currentTime, bool CallListener = false, bool needRewrite = false);
		//	Accessor : currentLine
		int getCurrentLine() const {
			return currentLine;
		}
		void setCurrentLine(int _currentLine, bool noCallListener = false) {
			int showLineNum = (size.y - SCROLL_BAR_WIDTH - AXIS_SIZE_Y - MARGIN * 2) / (LABEL_SIZE_Y - 1);

			currentLine = min(max(_currentLine, 0), (int)lineData.size() - 1);

			if (currentLine <= showPos_line) {
				showPos_line = currentLine;
			}
			if ((showPos_line + showLineNum - 1 )<= currentLine) {
				showPos_line = currentLine - showLineNum + 1;
			}

			//リスナーコール
			if (!noCallListener && (this->cursorListener != NULL)) {
				(this->cursorListener)();
			}

			//再描画要求
			if (rewriteOnChange) {
				callRewrite();
			}
		}
		std::basic_string<TCHAR> getCurrentLineName() const {
			return lineData[currentLine]->name;
		}
		void setCurrentLineName(const std::basic_string<TCHAR>& value) {
			for (int i = 0; i<(int)lineData.size(); i++) {
				if (lineData[i]->name == value) {
					setCurrentLine(i);
				}
			}
		}
		//	Accessor : showPos_time
		double getShowPosTime() const {
			return showPos_time;
		}
		double calcShowPosTime(double _showPosTime);
		void setShowPosTime(const double& _showPosTime) {
			showPos_time = calcShowPosTime(_showPosTime);

			//再描画要求
			//if (rewriteOnChange) {
			//	callRewrite();
			//}
		};
		//	Accessor : showPos_line
		int getShowPosLine() const {
			return showPos_line;
		}
		void setShowPosLine(int _showPosLine) {
			int y0 = MARGIN + AXIS_SIZE_Y;
			int y1 = size.y - MARGIN - SCROLL_BAR_WIDTH + 1;

			int showLineNum = (size.y - SCROLL_BAR_WIDTH - AXIS_SIZE_Y - MARGIN * 2) / (LABEL_SIZE_Y - 1);
			if (showLineNum<(int)lineData.size()) {
				showPos_line = max(0, min(_showPosLine, (int)lineData.size() - showLineNum));
			}
			else {
				showPos_line = 0;
			}

			//再描画要求
			if (rewriteOnChange) {
				callRewrite();
			}
		}
		/// Accessor : keyShiftTime
		double getShiftKeyTime() const {
			return ghostShiftTime;
		}
		///	Accessor : cursorListener
		void setCursorListener(std::function<void()> listener) {
			this->cursorListener = listener;
		}
		///	Accessor : selectListener
		void setSelectListener(std::function<void()> listener) {
			this->selectListener = listener;
		}
		///	Accessor : keyShiftListener
		void setKeyShiftListener(std::function<void()> listener) {
			this->keyShiftListener = listener;
		}
		///	Accessor : keyShiftListener
		void setKeyDeleteListener(std::function<void(const KeyInfo&)> listener) {
			this->keyDeleteListener = listener;
		}
		void setMouseMDownListener(std::function<void()> listener) {
			this->mouseMDownListener = listener;
		}
		void setMouseWheelListener(std::function<void()> listener) {
			this->mouseWheelListener = listener;
		}
		void setMouseRUpListener(std::function<void()> listener) {
			this->mouseRUpListener = listener;
		}

		void setEulMinMax(int _ikkind, float _minval, float _maxval) {
			mineul = (double)_minval;
			maxeul = (double)_maxval;
			ikkind = _ikkind;
			isseteulminmax = true;
		}
		void getEulMinMax(bool* dstisset, float* dstmin, float* dstmax) {
			if (dstisset && dstmin && dstmax) {
				*dstisset = isseteulminmax;
				*dstmin = (float)mineul;
				*dstmax = (float)maxeul;
			}
		}

		int getEuler(double srcframe, ChaVector3* dsteul);

		KeyInfo ExistKey(int srcline, double srctime)
		{
			KeyInfo retki;
			retki.lineIndex = -1;
			retki.time = -1.0;
			//retki.blendweight.clear();
			retki.object = 0;
			if ((srcline >= 0) && (srcline < (int)lineData.size())) {
				EulLineData* curLineData = lineData[srcline];
				if (curLineData) {
					int i = curLineData->getKeyIndex(srctime);
					if (i >= 0) {
						retki.lineIndex = srcline;
						retki.time = curLineData->key[i]->time;
						retki.object = NULL;// curLineData->key[i]->object;
					}
				}
			}

			return retki;
		}

		void setEulTip(ChaVector3 srceul)
		{
			eultip = srceul;
		}
		ChaVector3 getEulTip()
		{
			return eultip;
		}

	private:
		////////////////////////// MemberVar /////////////////////////////
		double maxTime, currentTime, showPos_time, showPos_width;
		int currentLine, showPos_line;
		int wheeldelta;
		std::function<void()> cursorListener;
		std::function<void()> selectListener;
		std::function<void()> keyShiftListener;
		std::function<void()> mouseMDownListener;
		std::function<void()> mouseWheelListener;
		std::function<void()> mouseRUpListener;
		std::function<void(const KeyInfo&)> keyDeleteListener;
		bool isseteulminmax;
		double mineul;
		double maxeul;
		int ikkind;
		double dispscale;
		double dispoffset;
		CModel* currentmodel;
		ChaVector3 eultip;//カレントのオイラー角を　グラフ上に表示するため　値をMain.cppでセットする

		//行データクラス-------------
		public : class EulLineData {
		public:
			EulLineData(int _depth, int nullflag, const std::basic_string<TCHAR>& _name, OWP_EulerGraph* _parent, unsigned int _lineIndex) {
				depth = _depth;
				m_nullflag = nullflag;
				name = _name;
				parent = _parent;
				lineIndex = _lineIndex;
				minselected = 0;
				maxselected = 0;
				key.clear();
				//InitEulKeys();
			};
			//EulLineData(const EulLineData& a) {
			//	_ASSERT_EXPR(0, L"コピーコンストラクタは使えません");
			//};
			~EulLineData() {
				std::vector<EulKey*>::iterator it;
				for (it = key.begin(); it != key.end(); it++) {
					//delete (*it);
					(*it)->InvalidateEulKeys();
				}

				//DestroyEulKeys();

			};

			void SetParent(OWP_EulerGraph* _parent) {
				parent = _parent;
			};

			//class OWP_EulerGraph::EulLineData::EulKey;
			//static OWP_EulerGraph::EulLineData::EulKey* OWP_EulerGraph::EulLineData::GetNewEulKey();
			//static void OWP_EulerGraph::EulLineData::InvalidateEulKeys(OWP_EulerGraph::EulLineData::EulKey* srceul);

			int getMinSelected()
			{
				return minselected;
			}
			void setMinSelected(int srcval)
			{
				minselected = srcval;
			}
			int getMaxSelected()
			{
				return maxselected;
			}
			void setMaxSelected(int srcval)
			{
				maxselected = srcval;
			}


			//キーデータクラス---------------
			class EulKey {
			public:
				EulKey() {
					InitParams();
				};
				EulKey(double _time, int _type = 0, double _value = 0.0, double _length = 1.0, bool _select = false) {
					InitParams();
					SetParams(_time, _type, _value, _length, _select);
				};

				void InitParams() {
					time = 0.0;
					type = 0;
					length = 0.0;
					select = false;
					value = 0.0;

					m_useflag = 0;//0: not use, 1: in use
					m_indexofpool = 0;//index of pool vector
					m_allocheadflag = 0;//1: head pointer at allocated
				};
				void SetParams(double _time, int _type = 0, double _value = 0.0, double _length = 1.0, bool _select = false) {
					time = _time;
					type = _type;
					length = _length;
					select = _select;
					value = _value;
				};
				void* operator new[](size_t sizeInBytes) { 
					//return (void*)malloc(sizeInBytes); 
					size_t newelemnum;
					newelemnum = sizeInBytes / sizeof(OWP_EulerGraph::EulLineData::EulKey);
					if (sizeInBytes != (newelemnum * sizeof(OWP_EulerGraph::EulLineData::EulKey))) {
						_ASSERT(0);
						return 0;
					}

					OWP_EulerGraph::EulLineData::EulKey* neweulblk;
					neweulblk = (OWP_EulerGraph::EulLineData::EulKey*)malloc(sizeof(OWP_EulerGraph::EulLineData::EulKey) * newelemnum);
					if (!neweulblk) {
						_ASSERT(0);
						return 0;
					}
					size_t elemno;
					for (elemno = 0; elemno < newelemnum; elemno++) {
						OWP_EulerGraph::EulLineData::EulKey* newelem;
						newelem = neweulblk + elemno;
						newelem->InitParams();
					}

					return neweulblk;
				};
				
				void InvalidateEulKeys();


				int GetUseFlag()
				{
					return m_useflag;
				};
				void SetUseFlag(int srcflag)
				{
					m_useflag = srcflag;
				};
				int GetIndexOfPool()
				{
					return m_indexofpool;
				};
				void SetIndexOfPool(int srcindex)
				{
					m_indexofpool = srcindex;
				};
				int IsAllocHead()
				{
					return m_allocheadflag;
				};
				void SetIsAllocHead(int srcflag)
				{
					m_allocheadflag = srcflag;
				};



				////////////////////////// MemberVar /////////////////////////////
				double time, length;
				int type;
				bool select;
				double value;
				int m_useflag;//0: not use, 1: in use
				int m_indexofpool;//index of pool vector
				int m_allocheadflag;//1: head pointer at allocated

			};

		public:			
			////////////////////////// MemberVar /////////////////////////////

			int m_nullflag;
			std::basic_string<TCHAR> name;
			std::vector<OWP_EulerGraph::EulLineData::EulKey*> key;
			unsigned int lineIndex;
			int depth;
			int minselected;//2022/09/12 : key[].selected検索のヒント
			int maxselected;//2022/09/12 : key[].selected検索のヒント


			int getValue(double srcframe, double* dstvalue);

			//////////////////////////// Method //////////////////////////////
			//	Method : 描画
			void draw(HDCMaster *hdcM,
				int posX, int posY,
				int width,
				double timeSize,
				double startTime,
				bool highLight = false) {

				if (!parent) {
					return;
				}

				const int AXIS_CURSOR_SIZE = 4;

				unsigned char baseR = parent->baseColor.r;
				unsigned char baseG = parent->baseColor.g;
				unsigned char baseB = parent->baseColor.b;


				int offsetY = -32;//!!!!!!!!!!! 左上が０で上がマイナス、上に16
				posY += offsetY;//!!!!!!!!!!!


				int x0 = posX;
				int x1 = posX + parent->LABEL_SIZE_X;
				int x2 = posX + width;
				int y0 = posY;
				int y1 = posY + parent->GRAPH_SIZE_Y;

				

				if (parent->isseteulminmax == false) {
					return;
				}


				//if (highLight) {
				//	hdcM->setPenAndBrush(NULL, RGB(min(baseR + 20, 255), min(baseG + 20, 255), min(baseB + 20, 255)));
				//	Rectangle(hdcM->hDC, x0, y0, x1, y1);
				//}



				//TextOut(hdcM->hDC,
				//	posX + 2, posY + parent->LABEL_SIZE_Y / 2 - 5,
				//	prname.c_str(), (int)_tcslen(prname.c_str()));

				//TextOut( hdcM->hDC,
				//		 posX+2, posY+parent->LABEL_SIZE_Y/2-5,
				//		 name.c_str(), _tcslen(name.c_str()));

				//枠
				//if (wcscmp(L"X", name.c_str()) == 0) {
					//hdcM->setPenAndBrush(RGB(min(baseR + 20, 255), min(baseG + 20, 255), min(baseB + 20, 255)), NULL);
					//Rectangle(hdcM->hDC, x0, y0, x2, y1);
					//hdcM->setPenAndBrush(RGB(min(baseR + 20, 255), min(baseG + 20, 255), min(baseB + 20, 255)), RGB(baseR, baseG, baseB));
					//Rectangle(hdcM->hDC, x1 - 2, y0, x1 + 1, y1);
					//Rectangle(hdcM->hDC, x1, y0, x2, y1);
					//int x3 = (int)((parent->maxTime - startTime)*timeSize) + x1 + 2;		//maxTime
					//if (x1 <= x3 && x3 <= x2) {
					//	MoveToEx(hdcM->hDC, x3, y0, NULL);
					//	LineTo(hdcM->hDC, x3, y1);
					//}
					//if (x1 <= x3 + 2 && x3 + 2 <= x2) {
					//	MoveToEx(hdcM->hDC, x3 + 2, y0, NULL);
					//	LineTo(hdcM->hDC, x3 + 2, y1);
					//}
					//int x4 = (int)((parent->currentTime - startTime)*timeSize) + x1 + 1;		//currentTime
					//if (x1 <= x4 && x4 < x2) {
					//	MoveToEx(hdcM->hDC, x4, y0, NULL);
					//	LineTo(hdcM->hDC, x4, y1);
					//}
				//}

				//ゴーストキー
				x1++; x2--;
				y0++; y1--;
				//for (int i = 0; i<(int)key.size(); i++) {
				//	int xx0 = (int)((key[i]->time - startTime + parent->ghostShiftTime)*timeSize) + x1;
				//	int xx1 = (int)(key[i]->length*timeSize) + xx0 + 1;

				//	if (x2 <= xx0) {
				//		break;
				//	}
				//	if (x1 <= xx1 && key[i]->select) {

				//		if (x1 <= xx1 - 1 && xx0 + 1 <= x2) {

				//			hdcM->setPenAndBrush(NULL, RGB(min(baseR + 20, 255), min(baseG + 20, 255), min(baseB + 20, 255)));
				//			Rectangle(hdcM->hDC, max(xx0 + 1, x1), y0 + 1, min(xx1 - 1, x2), y1 - 1);

				//		}

				//		if (x1 <= xx0) {
				//			hdcM->setPenAndBrush(RGB(baseR, baseG, baseB), NULL);
				//			MoveToEx(hdcM->hDC, xx0, y0, NULL);
				//			LineTo(hdcM->hDC, xx0, y1);
				//		}

				//	}
				//}

				double eulmargin = 10.0;

				double eulrange;
				int y2;

				bool sgraph = false;

				//キー
				if (wcscmp(L"X", name.c_str()) == 0) {
					sgraph = false;

					//hdcM->setPenAndBrush(NULL, RGB(255, 0, 0));
					hdcM->setPenAndBrush(NULL, RGB(255, 128, 128));
					eulrange = abs(parent->maxeul - parent->mineul) / parent->getDispScale();
					y2 = y0 + (int)parent->getDispOffset();

				}
				else if (wcscmp(L"Y", name.c_str()) == 0) {
					sgraph = false;

					hdcM->setPenAndBrush(NULL, RGB(0, 255, 0));
					eulrange = abs(parent->maxeul - parent->mineul) / parent->getDispScale();
					y2 = y0 + (int)parent->getDispOffset();
				}
				else if (wcscmp(L"Z", name.c_str()) == 0) {
					sgraph = false;

					//hdcM->setPenAndBrush(NULL, RGB(0, 0, 255));
					//hdcM->setPenAndBrush(NULL, RGB(0, 128, 255));
					hdcM->setPenAndBrush(NULL, RGB(150, 200, 255));
					eulrange = abs(parent->maxeul - parent->mineul) / parent->getDispScale();
					y2 = y0 + (int)parent->getDispOffset();
				}
				else if (wcscmp(L"S", name.c_str()) == 0) {
					sgraph = true;

					//2022/10/20 CommentOut : スケールの色で選択範囲の両端に垂直線を引くために　returnしないことに
					//if ((g_previewFlag) != 0 && (g_previewFlag != 5)) {
					//	return;//!!!!!!!!!!!!!!! preview時にはブラシラインは表示しない
					//}

					hdcM->setPenAndBrush(NULL, RGB(255, 255, 255));
					eulrange = abs(parent->maxeul - parent->mineul) * 1.0;//scale 1.0
					y2 = y0;//MotionBrushは初期位置
				}
				else {
					hdcM->setPenAndBrush(NULL, RGB(min(baseR + 20, 255), min(baseG + 20, 255), min(baseB + 20, 255)));
					eulrange = abs(parent->maxeul - parent->mineul) * 1.0;//scale 1.0
					y2 = y0;//MotionBrushは初期位置
				}

				//if (eulrange < 10.0) {
				//	eulrange = 10.0;
				//}
				if (eulrange < 1.0) {
					eulrange = 1.0;
				}


				bool firstdrawflag = true;
				int startindex;// = getKeyIndex(startTime);
				if ((g_previewFlag == 0) || (g_previewFlag == 5) || g_preciseOnPreviewToo) {
					startindex = getKeyIndex(startTime);
				}
				else {
					startindex = max(0, (getKeyIndex(parent->currentTime) - KEYNUM_ONPREVIEW * 2));
				}

				//スケール表示時　再生中はSグラフ非表示　ただし下方コードにて　両端の垂直ラインは描画
				if ((startindex >= 0) && !(sgraph && (g_previewFlag != 0) && (g_previewFlag != 5))) {
					int currentkeynum = (int)key.size();
					int currenttimeindex = getKeyIndex(parent->currentTime);

					//int endkey = min(currentkeynum, (startindex + (int)parent->showPos_width));
					int endkey;// = min(currentkeynum, (startindex + 1));
					if ((g_previewFlag == 0) || (g_previewFlag == 5)) {
						endkey = currentkeynum - 1;
					}
					else {
						//endkey = min(currentkeynum, (startindex + KEYNUM_ONPREVIEW));
						endkey = min((currentkeynum - 1), currenttimeindex);
					}

					//for (int i = startindex; i < currentkeynum; i++) {
					for (int i = startindex; i <= endkey; i++) {
						//for (int i = 0; i < (int)key.size(); i++) {

						double keytime = key[i]->time;

						if ((keytime - startTime) < 0.0) {
							continue;
							//break;
						}

						//valueが増える方向を上方向に。座標系は下方向にプラス。
						//int ey0 = (parent->maxeul - key[i]->value) / (eulrange + 2.0 * eulmargin) * (y1 - y0) + y0;
						int ey0 = (int)((parent->maxeul - key[i]->value) / eulrange * ((double)y1 - (double)y0) + (double)y2);
						int ey1 = ey0 + DOT_SIZE_Y;

						int ex0 = (int)((keytime - startTime) * timeSize) + x1;
						int ex1 = ex0 + DOT_SIZE_X;

						if (ex1 > (x2 + DOT_SIZE_X)) {
							break;
						}

						
						if (ey0 < y0) {
							continue;
						}

						//if (key[i]->select) {
						//}
						//Rectangle(hdcM->hDC, max(ex0 + 2, ex1), ey0 + 2, min(ex1 - 2, x2), ey1 - 2);

						if (ex0 >= x1) {
							if (firstdrawflag == true) {
								MoveToEx(hdcM->hDC, ex0, ey0, NULL);
								firstdrawflag = false;
							}
							else {
								LineTo(hdcM->hDC, ex0, ey0);//2022/09/13 RectangleよりもLineToの方が描画が速い
							}

							//Rectangle(hdcM->hDC, ex0, ey0, ex1, ey1);
						}
						else {
							_ASSERT(0);
						}
					}
				}


			//Ellipse and line at current.
			//カレント位置に丸マーク　カレント位置に垂直ライン
				{
					double keytime = parent->currentTime;
					int currentindex = getKeyIndex(keytime);
					if ((currentindex >= 0) && (currentindex < key.size())) {
						int ey0 = (int)((parent->maxeul - key[currentindex]->value) / eulrange * ((double)y1 - (double)y0) + (double)y2);
						int ey1 = ey0 + DOT_SIZE_Y;
						int ex0 = ((int)keytime - (int)startTime) * (int)timeSize + x1;
						int ex1 = ex0 + DOT_SIZE_X;

						//timeline cursor
						//int xx = (int)((parent->currentTime - parent->getShowPosTime()) * timeSize) + x1 + 1;


						if (!sgraph && ((g_previewFlag != 0) && (g_previewFlag != 5))) {
							//再生中にcurrenttimeにサークル表示
							Ellipse(hdcM->hDC, ex0 - 2, ey0 - 2, ex0 + AXIS_CURSOR_SIZE + 2, ey0 + 2);
						}
						else if (sgraph && ((g_previewFlag == 0)) || (g_previewFlag == 5)) {
							//再生中以外　ブラシ色で　currenttimeに　縦線
							//MoveToEx(hdcM->hDC, ex0, y0, NULL);
							//MoveToEx(hdcM->hDC, ex0, (parent->LABEL_SIZE_Y * 2), NULL);
							if (g_4kresolution) {
								MoveToEx(hdcM->hDC, ex0, (parent->LABEL_SIZE_Y * 4), NULL);
							}
							else {
								MoveToEx(hdcM->hDC, ex0, (parent->LABEL_SIZE_Y * 3), NULL);
							}
							
							LineTo(hdcM->hDC, ex0, y1);
						}
					}
				}

			//Lines at both of edge
			//選択両端に　垂直ライン
				if (sgraph) {

					int edgeindex;
					int ey0;
					int ey1;
					int ex0;
					int ex1;
					double keytime;

					int startedge;
					int endedge;
					if ((g_previewFlag == 0) || (g_previewFlag == 5)){
						startedge = IntTime(g_motionbrush_startframe);
						endedge = IntTime(g_motionbrush_endframe);
					}else{
						startedge = IntTime(g_playingstart);
						endedge = IntTime(g_playingend);
					}

					//vert startedge
					edgeindex = startedge;
					if ((edgeindex >= 0) && (edgeindex < key.size())) {
						keytime = key[edgeindex]->time;
						ey0 = (int)((parent->maxeul - key[edgeindex]->value) / eulrange * ((double)y1 - (double)y0) + (double)y2);
						ey1 = ey0 + DOT_SIZE_Y;
						ex0 = (int)((keytime - startTime) * timeSize) + x1;
						ex1 = ex0 + DOT_SIZE_X;
						if (ex0 >= x1) {
							//MoveToEx(hdcM->hDC, ex0, y0, NULL);
							//MoveToEx(hdcM->hDC, ex0, (parent->LABEL_SIZE_Y * 2), NULL);
							if (g_4kresolution) {
								MoveToEx(hdcM->hDC, ex0, (parent->LABEL_SIZE_Y * 4), NULL);
							}
							else {
								MoveToEx(hdcM->hDC, ex0, (parent->LABEL_SIZE_Y * 3), NULL);
							}
							
							LineTo(hdcM->hDC, ex0, y1);
						}
					}

					//vert endedge
					edgeindex = endedge;
					if (startedge != endedge) {//2023/10/17 1フレーム長選択時にはendedgeの垂直線は描かない
						if ((edgeindex >= 0) && (edgeindex < key.size())) {
							keytime = key[edgeindex]->time;
							ey0 = (int)((parent->maxeul - key[edgeindex]->value) / eulrange * ((double)y1 - (double)y0) + (double)y2);
							ey1 = ey0 + DOT_SIZE_Y;
							ex0 = (int)((keytime - startTime) * timeSize) + x1;
							ex1 = ex0 + DOT_SIZE_X;
							if (ex0 >= x1) {
								//MoveToEx(hdcM->hDC, ex0, y0, NULL);
								//MoveToEx(hdcM->hDC, ex0 + AXIS_CURSOR_SIZE * 2, (parent->LABEL_SIZE_Y * 2), NULL);
								if (g_4kresolution) {
									MoveToEx(hdcM->hDC, ex0 + AXIS_CURSOR_SIZE * 2, (parent->LABEL_SIZE_Y * 4), NULL);
								}
								else {
									MoveToEx(hdcM->hDC, ex0 + AXIS_CURSOR_SIZE * 2, (parent->LABEL_SIZE_Y * 3), NULL);
								}
								LineTo(hdcM->hDC, ex0 + AXIS_CURSOR_SIZE * 2, y1);
							}
						}
					}
				}






				//for (int i = 0; i < (int)key.size(); i++) {
				//	//valueが増える方向を上方向に。座標系は下方向にプラス。
				//	//int ey0 = (parent->maxeul - key[i]->value) / (eulrange + 2.0 * eulmargin) * (y1 - y0) + y0;
				//	int ey0 = (int)((parent->maxeul - key[i]->value) / eulrange * ((double)y1 - (double)y0) + (double)y2);
				//	int ey1 = ey0 + DOT_SIZE_Y;

				//	int ex0 = (int)((key[i]->time - startTime)*timeSize) + x1;
				//	int ex1 = ex0 + DOT_SIZE_X;

				//	if ((key[i]->time - startTime) < 0.0) {
				//		continue;
				//	}
				//	if (ex1 > x2) {
				//		break;
				//	}

				//	if (ey0 < y0) {
				//		continue;
				//	}

				//	//if (key[i]->select) {
				//	//}
				//	//Rectangle(hdcM->hDC, max(ex0 + 2, ex1), ey0 + 2, min(ex1 - 2, x2), ey1 - 2);
				//	Rectangle(hdcM->hDC, ex0, ey0, ex1, ey1);
				//}


				//目盛り 10度単位
				if (wcscmp(L"X", name.c_str()) == 0) {
					int fontsize = 12;
					hdcM->setFont(fontsize, _T("ＭＳ ゴシック"));
					//SetTextColor(hdcM->hDC, RGB(255, 255, 255));

					double mesurestep;
					//if (parent->ikkind == 0) {
					//	mesurestep = 10.0;
					//}
					//else if (parent->ikkind == 1) {
					//	mesurestep = 2.0;
					//}
					//else {
					//	mesurestep = 10.0;
					//}
					if (parent->maxeul != parent->mineul) {
						mesurestep = (parent->maxeul - parent->mineul) / 6.0;
					}
					else {
						if (parent->ikkind == 0) {
							mesurestep = 10.0;
						}
						else if (parent->ikkind == 1) {
							mesurestep = 2.0;
						}
						else {
							mesurestep = 10.0;
						}
					}
					
					//int befey0 = 0;
					//int mindiv = (int)((parent->mineul - 1.0) / (double)mesurestep);
					double mindiv = parent->mineul / mesurestep;
					//int minmeasure = (mindiv - 1) * mesurestep;
					double minmeasure = mindiv * mesurestep;
					//int maxdiv = (int)((parent->maxeul + 1.0) / (double)mesurestep);
					double maxdiv = parent->maxeul / mesurestep;
					//int maxmeasure = (maxdiv + 1) * mesurestep;
					double maxmeasure = maxdiv * mesurestep;


					int prevy0 = y0;

					//min
					//int ey0 = (parent->maxeul - minmeasure) / (eulrange + 2.0 * eulmargin) * (y1 - y0) + y0;
					int ey0 = (int)((parent->maxeul - minmeasure) / eulrange * (y1 - y0) + y2);
					int ex0 = (int)(x0 + parent->LABEL_SIZE_X - 7 * fontsize);
					WCHAR strmeasure[64];
					if (ey0 >= y0) {
						swprintf_s(strmeasure, 64, L"%+3.3lf---", minmeasure);
						strmeasure[64 - 1] = 0L;
						size_t measurelen;
						measurelen = wcslen(strmeasure);
						if ((measurelen > 0) && (measurelen < 64)) {
							SetTextColor(hdcM->hDC, RGB(255, 255, 255));
							TextOut(hdcM->hDC,
								ex0, ey0,
								strmeasure, (int)measurelen);
							//hdcM->setPenAndBrush(RGB(min(baseR + 20, 255), min(baseG + 20, 255), min(baseB + 20, 255)), NULL);
							//MoveToEx(hdcM->hDC, x1, ey0, NULL);
							//LineTo(hdcM->hDC, x2, ey0);
						}
						else {
							_ASSERT(0);
						}
						prevy0 = y0;
					}
					//befey0 = ey0;//!!!!!!!!!!!!!!!


					//ikkind
					bool displabel = false;
					//ey0 = (parent->maxeul - maxmeasure) / (eulrange + 2.0 * eulmargin) * (y1 - y0) + y0;//
					ey0 = (int)((parent->maxeul - maxmeasure) / eulrange * (y1 - y0) + y2);//
					//ex0 = (int)(x0 + parent->LABEL_SIZE_X - 17 * fontsize);//
					ex0 = (int)(x0 + 5);//2023/01/08
					if (ey0 >= y0) {
						if (parent->ikkind == 0) {
							//swprintf_s(strmeasure, 64, L"LocalRot(parjoint)");
							swprintf_s(strmeasure, 64, L"Rot");
							displabel = true;
						}
						else if (parent->ikkind == 1) {
							//swprintf_s(strmeasure, 64, L"LocalPos(curjoint)");
							swprintf_s(strmeasure, 64, L"Tra");
							displabel = true;
						}
						else if (parent->ikkind == 2) {
							//swprintf_s(strmeasure, 64, L"LocalScale(curjoint)");
							swprintf_s(strmeasure, 64, L"Scale");
							displabel = true;
						}
						if (displabel) {
							strmeasure[64 - 1] = 0L;
							size_t measurelen;
							measurelen = wcslen(strmeasure);
							if ((measurelen > 0) && (measurelen < 64)) {

								SetTextColor(hdcM->hDC, RGB(168, 129, 129));
								TextOut(hdcM->hDC,
									ex0, ey0,
									strmeasure, (int)measurelen);
								//hdcM->setPenAndBrush(RGB(min(baseR + 20, 255), min(baseG + 20, 255), min(baseB + 20, 255)), NULL);
								//MoveToEx(hdcM->hDC, x1, ey0, NULL);
								//LineTo(hdcM->hDC, x2, ey0);
							}
							else {
								_ASSERT(0);
							}

						}
					}

					//between min and max
					for (double curmeasure = minmeasure + mesurestep; curmeasure < maxmeasure; curmeasure += mesurestep) {
						//valueが増える方向を上方向に。座標系は下方向にプラス。
						//ey0 = (parent->maxeul - curmeasure) / (eulrange + 2.0 * eulmargin) * (y1 - y0) + y0;
						ey0 = (int)((parent->maxeul - curmeasure) / eulrange * (y1 - y0) + y2);
						ex0 = (int)(x0 + parent->LABEL_SIZE_X - 7 * fontsize);
						if ((ey0 >= y0) && (abs(ey0 - prevy0) > 16)) {//文字フォントサイズを考慮
							//if (abs(ey0 - befey0) > 20) {
							//if (abs(ey0 - befey0) > (mesurestep * 2)) {
								swprintf_s(strmeasure, 64, L"%+3.3lf---", curmeasure);
								strmeasure[64 - 1] = 0L;
								size_t measurelen;
								measurelen = wcslen(strmeasure);
								if ((measurelen > 0) && (measurelen < 64)) {
									SetTextColor(hdcM->hDC, RGB(255, 255, 255));
									TextOut(hdcM->hDC,
										ex0, ey0,
										strmeasure, (int)measurelen);

									//hdcM->setPenAndBrush(RGB(min(baseR + 20, 255), min(baseG + 20, 255), min(baseB + 20, 255)), NULL);
									//MoveToEx(hdcM->hDC, x1, ey0, NULL);
									//LineTo(hdcM->hDC, x2, ey0);

									//befey0 = ey0;
								}
								else {
									_ASSERT(0);
								}


							//}
								prevy0 = ey0;
						}
					}


					//max
					//ey0 = (parent->maxeul - maxmeasure) / (eulrange + 2.0 * eulmargin) * (y1 - y0) + y0;
					ey0 = (int)((parent->maxeul - maxmeasure) / eulrange * (y1 - y0) + y2);
					ex0 = (int)(x0 + parent->LABEL_SIZE_X - 7 * fontsize);
					if ((ey0 >= y0) && (abs(ey0 - prevy0) > 16)) {//文字フォントサイズを考慮
						swprintf_s(strmeasure, 64, L"%+3.3lf---", maxmeasure);
						strmeasure[64 - 1] = 0L;
						size_t measurelen;
						measurelen = wcslen(strmeasure);
						if ((measurelen > 0) && (measurelen < 64)) {
							SetTextColor(hdcM->hDC, RGB(255, 255, 255));
							TextOut(hdcM->hDC,
								ex0, ey0,
								strmeasure, (int)measurelen);
							//hdcM->setPenAndBrush(RGB(min(baseR + 20, 255), min(baseG + 20, 255), min(baseB + 20, 255)), NULL);
							//MoveToEx(hdcM->hDC, x1, ey0, NULL);
							//LineTo(hdcM->hDC, x2, ey0);
						}
						else {
							_ASSERT(0);
						}

						prevy0 = ey0;
					}

				}
			}

			//	Method : キーを作成する
			bool newKey(const double &_time, int _type = 0, double _value = 0.0, double _length = 1.0, bool _select = false) {

				if (parent->allowSameTimeKey == false) {
					int i = getKeyIndex(_time);
					if (i != -1) {
						//上書きリスナーコール
						if (parent->keyDeleteListener != NULL) {
							KeyInfo ki;
							ki.label = name.c_str();
							ki.lineIndex = lineIndex;
							ki.time = key[i]->time;
							ki.timeIndex = -1;
							ki.object = NULL;// key[i]->value;
							(parent->keyDeleteListener)(ki);
						}

						key[i]->time = _time;//2022/10/22

						key[i]->type = _type;
						key[i]->length = _length;
						key[i]->select = _select;
						key[i]->value = _value;
						return true;
					}
				}

				if ((_time >= parent->maxTime) || _time<0.0) {
					/*					//作成不能削除リスナーコール
					if( parent->keyDeleteListener!=NULL ){
					KeyInfo ki;
					ki.label= name.c_str();
					ki.lineIndex= lineIndex;
					ki.time= _time;
					ki.timeIndex= -1;
					ki.object= _object;
					parent->keyDeleteListener(ki);
					}//*/
					return false;
				}
				if ((_time + _length) >= parent->maxTime) {
					_length = parent->maxTime - _time;
				}

				unsigned int currentindex = (unsigned int)(_time + 0.1);

				if(currentindex == key.size()){
					EulKey* newkey = (OWP_EulerGraph::EulLineData::EulKey*)GetNewEulKey();
					if (newkey) {
						//key[currentindex] = newkey;
						key.push_back(newkey);
						key[currentindex]->SetParams(_time, _type, _value, _length, _select);
					}
					else {
						_ASSERT(0);
						return false;
					}
				}
				else if ((currentindex >= 0) && (currentindex <= (key.size() - 1))) {
					EulKey* currentkey = key[currentindex];
					if (currentkey) {
						currentkey->SetParams(_time, _type, _value, _length, _select);
					}
					else {
						_ASSERT(0);
						return false;
					}
				}
				else {
					_ASSERT(0);
					return false;
				}
				return true;


				//if (key.size() != 0) {
				//	int pushPos = (int)key.size();
				//	for (int i = 0; i<(int)key.size(); i++) {
				//		if (_time <= key[i]->time) {
				//			pushPos = i;
				//			break;
				//		}
				//	}
				//	key.push_back(key[(int)key.size() - 1]);
				//	for (int i = (int)key.size() - 2; i>pushPos; i--) {
				//		key[i] = key[i - 1];
				//	}
				//	//key[pushPos] = new EulKey(_time, _type, _value, _length, _select);
				//	key[pushPos] = (OWP_EulerGraph::EulLineData::EulKey*)GetNewEulKey();
				//	key[pushPos]->SetParams(_time, _type, _value, _length, _select);

				//}
				//else {
				//	//key.push_back(new EulKey(_time, _type, _value, _length, _select));
				//	OWP_EulerGraph::EulLineData::EulKey* neweulkey;
				//	neweulkey = (OWP_EulerGraph::EulLineData::EulKey*)GetNewEulKey();
				//	if (neweulkey) {
				//		neweulkey->SetParams(_time, _type, _value, _length, _select);
				//		key.push_back(neweulkey);
				//	}
				//}

				//return true;
			}
			//	Method : キーに値を設定する。
			bool setKey(const double &_time, int _type = 0, double _value = 0.0) {
				int i = getKeyIndex(_time);
				if (i < 0) {
					return false;
				}
				key[i]->value = _value;

				return true;
			}
			//	Method : すべてのキーの選択を解除する
			void selectClear() {
				int currentkeynum = (int)key.size();
				for (int i = 0; i < currentkeynum; i++) {
					if (key[i]) {
						key[i]->select = false;
					}
				}
				setMinSelected(0);
				setMaxSelected(0);
			}
			//	Method : すべてのキーを選択する
			void selectAll() {
				int currentkeynum = (int)key.size();
				for (int i = 0; i < currentkeynum; i++) {
					if (key[i]) {
						key[i]->select = true;
					}
				}
				setMinSelected(0);
				setMaxSelected(max(0, (int)key.size() - 1));

				parent->ghostShiftTime = 0;
			}
			//	Method : 指定された時刻にあるキーをひとつ選択する
			bool selectKey(const double &_time) {

				int currentindex = getKeyIndex(_time);
				if (currentindex >= 0) {
					key[currentindex]->select = true;
					return true;
				}
				setMinSelected(currentindex);
				setMaxSelected(currentindex);


				//for (int i = 0; i<(int)key.size(); i++) {
				//	if (((_time - TIME_ERROR_WIDTH) <= key[i]->time)
				//		&& (key[i]->time <= (_time + TIME_ERROR_WIDTH))) {
				//		key[i]->select = true;
				//		return true;
				//	}
				//}
				parent->ghostShiftTime = 0;
				return false;

			}
			///	Method : 指定された時刻範囲にあるキーをすべて選択する
			///			 厳密モード(startTime<=keyTime<endTime)
			int selectKey(const double &startTime, const double &endTime) {
				int selectCount = 0;
				int startindex, endindex;
				startindex = getKeyIndex(startTime);
				endindex = getKeyIndex(endTime);
				if ((startindex >= 0) && (endindex >= 0)) {
					for (int i = startindex; i <= endindex; i++) {
						if ((startTime <= key[i]->time) && (key[i]->time <= endTime)) {//endTime ==含む　2021/11/09
							key[i]->select = true;
							selectCount++;
						}
					}
				}
				setMinSelected(startindex);
				setMaxSelected(endindex);


				//int selectCount = 0;
				//for (int i = 0; i<(int)key.size(); i++) {
				//	//					if( startTime-TIME_ERROR_WIDTH <= key[i]->time
				//	//					 && key[i]->time <= endTime+TIME_ERROR_WIDTH ){
				//	if ((startTime <= key[i]->time) && key[i]->time<endTime) {
				//		key[i]->select = true;
				//		selectCount++;
				//	}
				//}
				parent->ghostShiftTime = 0;
				return selectCount;

			}
			///	Method : 指定された時刻範囲にあるキーをすべて選択する
			///			 大雑把モード(startTime-TIME_ERROR_WIDTH<=keyTime<=endTime+TIME_ERROR_WIDTH)
			int selectKey2(const double &startTime, const double &endTime) {

				int selectCount = 0;
				int startindex, endindex;
				startindex = getKeyIndex(startTime);
				endindex = getKeyIndex(endTime);
				if ((startindex >= 0) && (endindex >= 0)) {
					for (int i = startindex; i <= endindex; i++) {
						if ((startTime <= key[i]->time) && (key[i]->time <= endTime)) {//endTime ==含む　2021/11/09
							key[i]->select = true;
							selectCount++;
						}
					}
				}

				setMinSelected(startindex);
				setMaxSelected(endindex);

				//int selectCount = 0;
				//for (int i = 0; i<(int)key.size(); i++) {
				//	if (((startTime - TIME_ERROR_WIDTH) <= key[i]->time) && 
				//		(key[i]->time <= (endTime + TIME_ERROR_WIDTH))) {
				//		key[i]->select = true;
				//		selectCount++;
				//	}
				//}
				parent->ghostShiftTime = 0;
				return selectCount;

			}
			//	Method : 指定された時刻にあるキーのインデックスをひとつ取得する
			int getKeyIndex(const double &_time) {
				//int timeindex = (int)(_time + 0.1);
				unsigned int currentindex = (unsigned int)(_time + 0.1);
				if (key.size() <= currentindex) {
					return -1;
				}
				else {
					return currentindex;
				}

				//for (int i = 0; i<(int)key.size(); i++) {
				//	if (((_time - TIME_ERROR_WIDTH) <= key[i]->time) && 
				//		(key[i]->time <= (_time + TIME_ERROR_WIDTH))) {
				//		return i;
				//	}
				//}
				//return -1;

			}
			////	Method : 選択されているキーをすべて削除する
			//unsigned int deleteKey(bool noCallListener = false) {
			//	Method : キーをすべて削除する
			unsigned int deleteKey(bool noCallListener = false) {
				//EulKeyの場合　全てInvalidate
				unsigned int deleteNum;
				deleteNum = (unsigned int)key.size();
				for (unsigned int i = 0; i < deleteNum; i++) {
					if (key[i]) {
						key[i]->InvalidateEulKeys();
					}
				}
				key.clear();

				//unsigned int deleteNum = 0;
				//for (unsigned int i = 0; i<(int)key.size(); i++) {
				//	if (key[i]->select) {
				//		//リスナーコール
				//		if (!noCallListener && parent->keyDeleteListener != NULL) {
				//			KeyInfo ki;
				//			ki.label = name.c_str();
				//			ki.lineIndex = lineIndex;
				//			ki.time = key[i]->time;
				//			ki.timeIndex = -1;
				//			ki.object = NULL;// key[i]->object;
				//			(parent->keyDeleteListener)(ki);
				//		}

				//		//delete key[i];
				//		key[i]->InvalidateEulKeys();
				//		deleteNum++;
				//	}
				//	else {
				//		key[i - deleteNum] = key[i];
				//	}
				//}
				//for (unsigned int i = 0; i<deleteNum; i++) {
				//	key.pop_back();
				//}

				return deleteNum;

			}
			////	Method : 指定されたキーを削除する
			//bool deleteKey(int index, bool noCallListener = false) {
			//	if ((unsigned int)key.size() <= (unsigned int)index) return false;

			//	//リスナーコール
			//	if (!noCallListener && (parent->keyDeleteListener != NULL)) {
			//		KeyInfo ki;
			//		ki.label = name.c_str();
			//		ki.lineIndex = lineIndex;
			//		ki.time = key[index]->time;
			//		ki.timeIndex = -1;
			//		ki.object = NULL;// key[index]->object;
			//		(parent->keyDeleteListener)(ki);
			//	}

			//	//delete key[index];
			//	key[index]->InvalidateEulKeys();

			//	for (int i = index; i<(int)key.size(); i++) {
			//		key[i - 1] = key[i];
			//	}
			//	key.pop_back();
			//	return true;
			//}
			////	Method : 指定されたキーを移動する
			//bool shiftKey(const double &shiftTime, int index) {
			//	if (key.size() <= (unsigned int)index) return false;

			//	double dstTime = key[index]->time + shiftTime;
			//	int type = key[index]->type;
			//	double length = key[index]->length;
			//	bool select = key[index]->select;
			//	void *object = NULL;// key[index]->object;
			//	double value = key[index]->value;
			//	deleteKey(index, true);

			//	//既にキーが存在する場合
			//	if (!newKey(dstTime, type, value, length, select)) {
			//		//置き換えによるキー削除リスナーコール
			//		if (parent->keyDeleteListener != NULL) {
			//			KeyInfo ki;
			//			ki.label = name.c_str();
			//			ki.lineIndex = lineIndex;
			//			ki.time = dstTime;
			//			ki.timeIndex = -1;
			//			ki.object = object;
			//			(parent->keyDeleteListener)(ki);
			//		}

			//		//存在するキーの値を置き換え後のキーの値に変更
			//		int dstIndex = getKeyIndex(dstTime);
			//		key[dstIndex]->type = type;
			//		key[dstIndex]->length = length;
			//		key[dstIndex]->value = value;
			//	}

			//	return true;
			//}
			////	Method : 選択されているキーをすべて移動する
			//unsigned int shiftKey(const double &shiftTime) {

			//	unsigned int shiftNum = 0;
			//	std::list<EulKey> shiftKeyList;
			//	for (int i = 0; i<(int)key.size(); i++) {
			//		if (key[i]->select) {
			//			shiftKeyList.push_back(*(key[i]));
			//			//						shiftKey(shiftTime,i);
			//			shiftNum++;
			//		}
			//	}

			//	deleteKey(true);

			//	std::list<EulKey>::iterator itr;
			//	for (itr = shiftKeyList.begin(); itr != shiftKeyList.end(); itr++) {
			//		if (!newKey(itr->time + shiftTime, itr->type, itr->value, itr->length, itr->select)) {
			//			//作成不能削除リスナーコール
			//			if (parent->keyDeleteListener != NULL) {
			//				KeyInfo ki;
			//				ki.label = name.c_str();
			//				ki.lineIndex = lineIndex;
			//				ki.time = itr->time + shiftTime;
			//				ki.timeIndex = -1;
			//				ki.object = NULL;// itr->object;
			//				(parent->keyDeleteListener)(ki);
			//			}
			//		}
			//	}

			//	return shiftNum;
			//}
			////	Method : 選択されているキーをゴーストキーの位置へすべて移動する
			//unsigned int shiftKey() {
			//	return shiftKey(parent->ghostShiftTime);
			//}

			/////////////////////////// Accessor /////////////////////////////
			int getSelectNum() {

				int selectCount = 0;
				int currentkeynum = (int)key.size();
				for (int i = 0; i < currentkeynum; i++) {
					if (key[i]->select) {
						selectCount++;
					}
				}
				return selectCount;

			}

		private:
			////////////////////////// MemberVar /////////////////////////////
			OWP_EulerGraph* parent;
			//static const double TIME_ERROR_WIDTH;
		};
		std::vector<EulLineData*> lineData;
		double ghostShiftTime;

		//static const int GRAPH_SIZE_Y = 60;
		//static const int GRAPH_SIZE_Y = 120;
		
		//static const int GRAPH_SIZE_Y = 170;
		int GRAPH_SIZE_Y;//constructorでセット
		
		static const int LABEL_SIZE_Y = 20;
		//static const int LABEL_SIZE_X= 75;
		//static const int LABEL_SIZE_X= 250;
		
		
		//static const int LABEL_SIZE_X = 280;
		static const int LABEL_SIZE_X = 135;//2023/01/08 owp_timelineのshortlabelと合わせる


		static const int AXIS_SIZE_Y = 15;
		//static const int SCROLL_BAR_WIDTH = 10;
		static const int SCROLL_BAR_WIDTH = 20;
		static const int MARGIN = 3;
		static const int DOT_SIZE_X = 3;
		static const int DOT_SIZE_Y = 3;
		double timeSize;
		bool allowSameTimeKey;

		double timeSnapSize;

		bool rewriteOnChange;		//キー操作時に再描画を行うか否かのフラグ
		bool canMouseControll;		//マウスでの操作が可能か否かのフラグ

		bool dragLabel;
		bool dragTime;
		bool dragScrollBarLabel;
		bool dragScrollBarTime;

		bool dragSelect;		//ドラッグでの範囲選択
		double dragSelectTime1, dragSelectTime2;
		int dragSelectLine1, dragSelectLine2;

		bool dragShift;			//ドラッグでのキー移動
	};


	///<summary>
	///	ウィンドウ内部品"レイヤーテーブル"クラス
	///</summary>
	class OWP_LayerTable: public OrgWindowParts{
	public:
		//////////////////// Constructor/Destructor //////////////////////
		OWP_LayerTable(const TCHAR *_name ) : OrgWindowParts() {
			name = new TCHAR[256];
			if (_name) {
				size_t tclen = _tcslen(_name);
				size_t cplen;
				if (tclen != 0) {
					if (tclen <= 255) {
						cplen = tclen;
					}
					else {
						cplen = 255;
					}
					_tcsncpy_s(name, 256, _name, cplen);
					name[cplen] = (TCHAR)0;
				}
				else {
					_tcscpy_s(name, 256, TEXT("NoName"));
				}
			}

			//cursorListener = [](){s_dummyfunc();};
			//lineShiftListener = [](int beforIndex, int afterIndex){s_dummyfunc();};
			//changeVisibleListener = [](int targetIndex){s_dummyfunc();};
			//changeLockListener = [](int targetIndex){s_dummyfunc();};
			//callPropertyListener = [](int targetIndex){s_dummyfunc();};
			cursorListener = NULL;
			lineShiftListener = NULL;
			changeVisibleListener = NULL;
			changeLockListener = NULL;
			callPropertyListener = NULL;


			currentLine= 0;
			showPosLine= 0;

			rewriteOnChange= true;
			canMouseControll= true;

			dragLine= false;
			dragScrollBarLine= false;
			dragVisibleButton= false;
			dragLockButton= false;

			mouseRBtnOnIndex= -1;
		}
		~OWP_LayerTable(){
			delete[] name;

			std::vector<LineData*>::iterator itr;
			for(itr = lineData.begin(); itr != lineData.end(); itr++){
				delete *itr;
			}
		}

		//////////////////////////// Method //////////////////////////////
		/// Method : 自動サイズ設定
		virtual void autoResize(){
			size.y-= (size.y-MARGIN*2)%(LABEL_SIZE_Y-1)-1;
		}
		//	Method : 描画
		virtual void draw(){
			if (!hdcM) {
				return;
			}

			drawEdge();

			int showLineNum= (size.y-MARGIN*2)/(LABEL_SIZE_Y-1);

			//行データ
			for(int i=showPosLine,j=0; i<(int)lineData.size() && j<showLineNum; i++,j++){
				bool highLight=false;
				if( i == currentLine ) highLight=true;

				lineData[i]->draw(	hdcM,
									pos.x+MARGIN,
									pos.y+MARGIN+j*(LABEL_SIZE_Y-1),
									size.x-SCROLL_BAR_WIDTH-MARGIN*2,highLight );
			}

			//ドラッグ移動の目印
			if( dragLine && (shiftIndex != 0) ){
				int markPos= currentLine+shiftIndex-showPosLine;
				if( 0 <= shiftIndex ) markPos+=1;

				markPos= pos.y+MARGIN+ markPos*(LABEL_SIZE_Y-1);

				hdcM->setPenAndBrush(RGB(255,255,255),NULL,0,2);
				MoveToEx(hdcM->hDC, pos.x+MARGIN,markPos, NULL);
				LineTo(hdcM->hDC,   size.x-SCROLL_BAR_WIDTH-MARGIN*2,markPos);
			}

			{//ラベルスクロールバー
				int x0= pos.x+size.x-MARGIN-SCROLL_BAR_WIDTH-1;
				int x1= x0+SCROLL_BAR_WIDTH+1;
				int y0= pos.y+MARGIN;
				int y1= pos.y+size.y-MARGIN;

				//枠
				hdcM->setPenAndBrush(RGB(min(baseColor.r+20,255),min(baseColor.g+20,255),min(baseColor.b+20,255)),NULL);
				Rectangle(hdcM->hDC,x0,y0,x1,y1);

				//中身
				if (lineData.size() > 0) {
					int barSize = (y1 - y0 - 4) * showLineNum / (int)lineData.size();
					int barStart = (y1 - y0 - 4) * showPosLine / (int)lineData.size();
					if (showLineNum < (int)lineData.size()) {
						//hdcM->setPenAndBrush(NULL, RGB(min(baseColor.r + 20, 255), min(baseColor.g + 20, 255), min(baseColor.b + 20, 255)));
						hdcM->setPenAndBrush(NULL, RGB(240, 240, 240));//2023/03/23
						Rectangle(hdcM->hDC, x0 + 2, y0 + 2 + barStart, x1 - 2, y0 + 2 + barStart + barSize + 1);
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
		//	Method : 行を追加	(既に同名のキーがある場合はFalseを返す)
		bool newLine(const std::basic_string<TCHAR> &_name, const void *object=NULL){
			for(int i=0; i<(int)lineData.size(); i++){
				if(lineData[i]->name==_name){
					return false;
				}
			}
			lineData.push_back(new LineData(_name,this,object));

			//再描画要求
			if( rewriteOnChange ){
				callRewrite();
			}
			return true;
		}
		//	Method : 行を削除
		void deleteLine(){
			for(int i=0; i<(int)lineData.size(); i++){
				delete lineData[i];
			}
			lineData.clear();

			//再描画要求
			if( rewriteOnChange ){
				callRewrite();
			}
		}
		bool deleteLine(const std::basic_string<TCHAR> &_name){
			int popPos=-1;
			for(int i=0; i<(int)lineData.size(); i++){
				if(lineData[i]->name==_name){
					delete lineData[i];
					popPos=i;
				}
			}
			if(popPos == -1) return false;
			for(int i=popPos+1; i<(int)lineData.size(); i++){
				lineData[i-1]=lineData[i];
			}
			lineData.pop_back();

			//再描画要求
			if( rewriteOnChange ){
				callRewrite();
			}
			return true;
		}
		bool deleteLine(int index){
			if((unsigned int)lineData.size() <= (unsigned int)index) return false;

			bool ret= deleteLine(lineData[index]->name);

			//再描画要求
			if( ret && rewriteOnChange ){
				callRewrite();
			}
			return ret;
		}
		///	Method : マウス左ボタンダウンイベント受信
		virtual void onLButtonDown(const MouseEvent& e){
			if( !canMouseControll ) return;

			int x0= MARGIN;
			int x1= MARGIN+ LABEL_SIZE_Y*2;
			int x2= size.x-SCROLL_BAR_WIDTH-MARGIN-1;
			int x3= size.x-MARGIN;
			int y0= MARGIN;
			int y1= size.y-MARGIN;

			//ボタン
			if( (x0 <= e.localX) && (e.localX < x1) && 
				(y0 <= e.localY) && (e.localY < y1) ){
				unsigned int targetIndex= showPosLine+ (e.localY-y0)/(LABEL_SIZE_Y-1);
				if( targetIndex <= ((unsigned int)lineData.size() - 1) ){
					//可視・不可視ボタン
					if( e.localX < (x0 + LABEL_SIZE_Y) ){
						lineData[targetIndex]->visible= !lineData[targetIndex]->visible;
						//リスナーコール
						if( this->changeVisibleListener != NULL ){
							(this->changeVisibleListener)(targetIndex);
						}
						dragVisibleButton= true;
						dragVisibleButtonValue= lineData[targetIndex]->visible;

					//ロックボタン
					}else{
						lineData[targetIndex]->lock= !lineData[targetIndex]->lock;
						//リスナーコール
						if( this->changeLockListener != NULL ){
							(this->changeLockListener)(targetIndex);
						}
						dragLockButton= true;
						dragLockButtonValue= lineData[targetIndex]->lock;
					}

					//再描画要求
					if( rewriteOnChange ){
						callRewrite();
					}
				}
			}

			//ラベルドラッグ
			if( (x1 <= e.localX) && (e.localX < x2) && 
				(y0 <= e.localY) && (e.localY < y1) ){
				setCurrentLine( showPosLine+ (e.localY-y0)/(LABEL_SIZE_Y-1) );

				dragLine= true;
				shiftIndex= 0;
			}

			//ラインスクロールバー
			if( (x2 <= e.localX) && (e.localX < x3) && 
				(y0 <= e.localY) && (e.localY < y1) ){
				int showLineNum= (y1-y0)/(LABEL_SIZE_Y-1);
				if( showLineNum < (int)lineData.size() ){
					int barSize= (y1-y0-4)*showLineNum/(int)lineData.size();

					int movableY= y1-y0-barSize;
					int movableYStart= y0+barSize/2;

					setShowPosLine( (e.localY-movableYStart)*((int)lineData.size()-showLineNum)/movableY );

					dragScrollBarLine=true;
				}
			}

		}
		//	Method : 左マウスボタン ダブルクリックイベント受信
		virtual void onLButtonDBLCLK(const MouseEvent& e) {//2023/10/04
		}
		//	Method : 右マウスボタン ダブルクリックイベント受信
		virtual void onRButtonDBLCLK(const MouseEvent& e) {//2023/10/04
			int dbgflag1 = 1;
		}

		///	Method : 左マウスボタンアップイベント受信
		virtual void onLButtonUp(const MouseEvent& e){
			if( !canMouseControll ) return;

			//ドラッグによるキー移動
			if( dragLine ){
				int beforIndex= currentLine;
				int afterIndex= max(0, min(currentLine+shiftIndex, ((int)lineData.size()-1)));
				if( beforIndex != afterIndex ){

					//リスナーコール
					if(this->lineShiftListener != NULL){
						(this->lineShiftListener)(beforIndex,afterIndex);
					}

					//置き換え
					if( beforIndex<afterIndex ){
						LineData* tmp= lineData[beforIndex];
						for(int i=beforIndex; i<afterIndex; i++){
							lineData[i]= lineData[i+1];
						}
						lineData[afterIndex]= tmp;
					}else{
						LineData* tmp= lineData[beforIndex];
						for(int i=beforIndex; afterIndex<i; i--){
							lineData[i]= lineData[i-1];
						}
						lineData[afterIndex]= tmp;
					}

					//カレントラインを修正
					currentLine= afterIndex;

				}
			}

			//ドラッグフラグを初期化
			dragLine= false;
			dragScrollBarLine= false;
			dragVisibleButton= false;
			dragLockButton= false;

			//再描画要求
			if( rewriteOnChange ){
				callRewrite();
			}
		}
		///	Method : マウス移動イベント受信
		virtual void onMouseMove(const MouseEvent& e){
			if( !canMouseControll ) return;

			int x0= MARGIN;
			int x1= size.x-SCROLL_BAR_WIDTH-MARGIN-1;
			int x2= size.x-MARGIN;
			int y0= MARGIN;
			int y1= size.y-MARGIN;

			//ライン
			if( dragLine || dragVisibleButton || dragLockButton ){
				int showLineNum= (size.y-MARGIN*2)/(LABEL_SIZE_Y-1);

				int newCposLine= showPosLine+ (e.localY-y0)/(LABEL_SIZE_Y-1);
				newCposLine= min(max(newCposLine,0),(int)lineData.size()-1);

				if( newCposLine <= showPosLine ){
					showPosLine= newCposLine;
				}
				if( (showPosLine + showLineNum - 1) <= newCposLine ){
					showPosLine= newCposLine-showLineNum+1;
				}

				//ラベルドラッグ
				if( dragLine ){
					shiftIndex= newCposLine-currentLine;

				//可視状態ボタンドラッグ
				}else if( dragVisibleButton ){
					setVisible(newCposLine,dragVisibleButtonValue);

				//ロック状態ボタンドラッグ
				}else{
					setLock(newCposLine,dragLockButtonValue);

				}

				//再描画要求
				if( rewriteOnChange ){
					callRewrite();
				}
			}

			//ラベルスクロールバー
			if( dragScrollBarLine ){
				int showLineNum= (y1-y0)/(LABEL_SIZE_Y-1);
				int barSize= (y1-y0-4)*showLineNum/(int)lineData.size();

				int movableY= y1-y0-barSize;
				int movableYStart= y0+barSize/2;

				setShowPosLine( (e.localY-movableYStart)*((int)lineData.size()-showLineNum)/movableY );
			}
		}
		///	Method : 右マウスボタンダウンイベント受信
		virtual void onRButtonDown(const MouseEvent& e){
			if( !canMouseControll ) return;

			int x0= MARGIN;
			int x1= MARGIN+ LABEL_SIZE_Y*2;
			int x2= size.x-SCROLL_BAR_WIDTH-MARGIN-1;
			int x3= size.x-MARGIN;
			int y0= MARGIN;
			int y1= size.y-MARGIN;

			//ラベル右クリック
			if( (x1 <= e.localX) && (e.localX < x2) && 
				(y0 <= e.localY) && (e.localY < y1) ){
				mouseRBtnOnIndex= showPosLine+ (e.localY-y0)/(LABEL_SIZE_Y-1);

				setCurrentLine( mouseRBtnOnIndex );
			}

		}
		///	Method : 右マウスボタンアップイベント受信
		virtual void onRButtonUp(const MouseEvent& e){
			if( !canMouseControll ) return;

			int x0= MARGIN;
			int x1= MARGIN+ LABEL_SIZE_Y*2;
			int x2= size.x-SCROLL_BAR_WIDTH-MARGIN-1;
			int x3= size.x-MARGIN;
			int y0= MARGIN;
			int y1= size.y-MARGIN;

			//プロパティコール
			if( (mouseRBtnOnIndex != -1) && 
				(mouseRBtnOnIndex == ((showPosLine + (e.localY - y0)) / (LABEL_SIZE_Y-1))) && 
				(x1 <= e.localX) && (e.localX < x2) && 
				(y0 <= e.localY) && (e.localY < y1) ){

				//リスナーコール
				if( this->callPropertyListener!=NULL ){
					(this->callPropertyListener)(mouseRBtnOnIndex);
				}
			}

			mouseRBtnOnIndex= -1;
		}

		//////////////////////////// Accessor //////////////////////////////
		/// Accessor : name
		const TCHAR* getName() const{
			return name;
		}
		void setName(const TCHAR *value){
			if (name && value) {
				size_t tclen = _tcslen(value);
				size_t cplen;
				if (tclen != 0) {
					if (tclen <= 255) {
						cplen = tclen;
					}
					else {
						cplen = 255;
					}
					_tcsncpy_s(name, 256, value, cplen);
					name[cplen] = (TCHAR)0;
				}
				else {
					_tcscpy_s(name, 256, TEXT("NoName"));
				}
			}

			//再描画要求
			if( rewriteOnChange ){
				callRewrite();
			}
		}
		std::basic_string<TCHAR> getName(int index) const{
			index= min(max(index,0),(int)lineData.size()-1);
			return lineData[index]->name;
		}
		bool setName(int index, const std::basic_string<TCHAR>& value ){
			if( (0 <= index) && (index < (int)lineData.size()) ){

				lineData[index]->name= value;

				//再描画要求
				if( rewriteOnChange ){
					callRewrite();
				}
				
				return true;
			}
			return false;
		}
		/// Accessor : rewriteOnChange
		bool getRewriteOnChangeFlag() const{
			return rewriteOnChange;
		}
		void setRewriteOnChangeFlag(bool value){
			rewriteOnChange= value;
		}
		//	Accessor : currentLine
		int getCurrentLine() const{
			return currentLine;
		}
		void setCurrentLine(int _currentLine, bool noCallListener=false){
			int showLineNum= (size.y-MARGIN*2)/(LABEL_SIZE_Y-1);

			currentLine= min(max(_currentLine,0),(int)lineData.size()-1);

			if( currentLine <= showPosLine ){
				showPosLine= currentLine;
			}
			if( (showPosLine + showLineNum - 1) <= currentLine ){
				showPosLine= currentLine-showLineNum+1;
			}

			//リスナーコール
			if( !noCallListener && (this->cursorListener != NULL) ){
				(this->cursorListener)();
			}

			//再描画要求
			if( rewriteOnChange ){
				callRewrite();
			}
		}
		std::basic_string<TCHAR> getCurrentLineName() const{
			return lineData[currentLine]->name;
		}
		void setCurrentLineName(const std::basic_string<TCHAR> &value){
			for(int i=0; i<(int)lineData.size(); i++){
				if( lineData[i]->name==value ){
					setCurrentLine(i);
				}
			}
		}
		//	Accessor : showPosLine
		int getShowPosLine() const{
			return showPosLine;
		}
		void setShowPosLine(int _showPosLine){
			int y0= MARGIN;
			int y1= size.y-MARGIN+1;

			int showLineNum= (size.y-MARGIN*2)/(LABEL_SIZE_Y-1);
			if( showLineNum < (int)lineData.size() ){
				showPosLine= max(0,min( _showPosLine ,(int)lineData.size()-showLineNum));
			}else{
				showPosLine= 0;
			}

			//再描画要求
			if( rewriteOnChange ){
				callRewrite();
			}
		}
		/// Accessor: 可視・不可視フラグ
		bool getVisible(const std::basic_string<TCHAR> &_name) const{
			std::vector<LineData*>::const_iterator itr;
			for(itr = lineData.begin(); itr != lineData.end(); itr++){
				if( *itr && ((*itr)->name==_name) ){
					return (*itr)->visible;
				}
			}

			return false;
		}
		bool getVisible(int index){
			if((unsigned int)lineData.size() <= (unsigned int)index) return false;

			return lineData[index]->visible;
		}
		bool setVisible(const std::basic_string<TCHAR> &_name, bool value, bool noCallListener=false){
			for(unsigned int i=0; i<(unsigned int)lineData.size(); i++){
				if( lineData[i]->name==_name ){

					//セットする値が現在の値と異なるときのみ変更する
					if( lineData[i]->visible != value ){
						lineData[i]->visible= value;

						//リスナーコール
						if( !noCallListener && (this->changeVisibleListener != NULL) ){
							(this->changeVisibleListener)((int)i);
						}

						//再描画要求
						if( rewriteOnChange ){
							callRewrite();
						}
					}
					return true;
				}
			}

			return false;
		}
		bool setVisible(int index, bool value, bool noCallListener=false){
			if(lineData.size() <= (unsigned int)index) return false;

			//セットする値が現在の値と異なるときのみ変更する
			if( lineData[index]->visible != value ){
				lineData[index]->visible= value;

				//リスナーコール
				if( !noCallListener && (this->changeVisibleListener!=NULL) ){
					(this->changeVisibleListener)(index);
				}

				//再描画要求
				if( rewriteOnChange ){
					callRewrite();
				}
			}
			return true;
		}
		/// Accessor: ロックフラグ
		bool getLock(const std::basic_string<TCHAR> &_name) const{
			std::vector<LineData*>::const_iterator itr;
			for(itr = lineData.begin(); itr != lineData.end(); itr++){
				if( *itr && ((*itr)->name==_name) ){
					return (*itr)->lock;
				}
			}

			return false;
		}
		bool getLock(int index){
			if(lineData.size() <= (unsigned int)index) return false;

			return lineData[index]->lock;
		}
		bool setLock(const std::basic_string<TCHAR> &_name, bool value, bool noCallListener=false){
			for(unsigned int i=0; i<(unsigned int)lineData.size(); i++){
				if( lineData[i]->name==_name ){

					//セットする値が現在の値と異なるときのみ変更する
					if( lineData[i]->lock != value ){
						lineData[i]->lock= value;

						//リスナーコール
						if( !noCallListener && (this->changeLockListener != NULL) ){
							(this->changeLockListener)(i);
						}

						//再描画要求
						if( rewriteOnChange ){
							callRewrite();
						}
					}
					return true;
				}
			}

			return false;
		}
		bool setLock(int index, bool value, bool noCallListener=false){
			if(lineData.size() <= (unsigned int)index) return false;

			//セットする値が現在の値と異なるときのみ変更する
			if( lineData[index]->lock != value ){
				lineData[index]->lock= value;

				//リスナーコール
				if( !noCallListener && (this->changeLockListener != NULL) ){
					(this->changeLockListener)(index);
				}

				//再描画要求
				if( rewriteOnChange ){
					callRewrite();
				}
			}
			return true;
		}

		const void* getObj(int index){
			if(lineData.size() <= (unsigned int)index) return 0;

			return lineData[index]->object;
		}

		///	Accessor : cursorListener
		void setCursorListener(std::function<void()> listener){
			this->cursorListener= listener;
		}
		///	Accessor : lineShiftListener
		void setLineShiftListener(std::function<void(int beforIndex, int afterIndex)> listener){
			this->lineShiftListener= listener;
		}
		///	Accessor : changeVisibleListener
		void setChangeVisibleListener(std::function<void(int targetIndex)> listener){
			this->changeVisibleListener= listener;
		}
		///	Accessor : changeLockListener
		void setChangeLockListener(std::function<void(int targetIndex)> listener){
			this->changeLockListener= listener;
		}
		///	Accessor : callPropertyListener
		void setCallPropertyListener(std::function<void(int targetIndex)> listener){
			this->callPropertyListener= listener;
		}

	private:
		////////////////////////// MemberVar /////////////////////////////
		int currentLine,showPosLine;
		TCHAR *name;
		std::function<void()> cursorListener;										//カーソル位置が変更された直後に呼ばれる
		std::function<void(int beforIndex, int afterIndex)> lineShiftListener;		//移動が行われる直前に呼ばれる
		std::function<void(int targetIndex)> changeVisibleListener;				//可視状態が変更された直後に呼ばれる
		std::function<void(int targetIndex)> changeLockListener;					//ロック状態が変更された直後に呼ばれる
		std::function<void(int targetIndex)> callPropertyListener;					//レイヤーのプロパティを呼ぶ時に呼ばれる

		class LineData{
		public:
			LineData(std::basic_string<TCHAR> _name, OWP_LayerTable *_parent, const void* _object){
				name= _name;
				parent= _parent;
				object= _object;
				visible= true;
				lock= false;
				select= false;
			}
			~LineData(){
			}

			std::basic_string<TCHAR> name;
			const void* object;
			bool visible;
			bool lock;
			bool select;

			//	Method : 描画
			void draw(	HDCMaster *hdcM,
						int posX, int posY,
						int width,
						bool highLight=false){
				unsigned char baseR= parent->baseColor.r;
				unsigned char baseG= parent->baseColor.g;
				unsigned char baseB= parent->baseColor.b;

				//highLight
				int x0= posX;
				int x1= posX+width;
				int y0= posY;
				int y1= posY+parent->LABEL_SIZE_Y;
				int x2= x0+parent->LABEL_SIZE_Y-1;
				int x3= x2+parent->LABEL_SIZE_Y-1;
				if( highLight ){
					//hdcM->setPenAndBrush(NULL,RGB(min(baseR+20,255),min(baseG+20,255),min(baseB+20,255)));
					hdcM->setPenAndBrush(NULL, RGB(255, 128, 0));
					Rectangle(hdcM->hDC,x3,y0,x1,y1);
				}

				//ラベル
				hdcM->setFont(12,_T("ＭＳ ゴシック"));
				SetTextColor(hdcM->hDC,RGB(240,240,240));
				TextOut( hdcM->hDC,
						 x3+2, posY+parent->LABEL_SIZE_Y/2-5,
						 name.c_str(), (int)_tcslen(name.c_str()));

				{//可視・不可視
					if( visible ){
						hdcM->setPenAndBrush( RGB(255,255,255), NULL );
					}else{
						hdcM->setPenAndBrush( RGB(min(baseR+20,255),min(baseG+20,255),min(baseB+20,255)), NULL );
					}

					double cx= (double)(x0+x2)/2.0;
					double cy= (double)(y0+y1)/2.0;
					double rx= (double)(x2-x0)/2.0*0.75;
					double ry= rx/1.7320508*0.8;
					Arc(hdcM->hDC, (int)(cx-ry*2+1.0), (int)(cy-ry+1.0),
								   (int)(cx+ry*2+1.0), (int)(cy+ry*3+1.0),
								   (int)(cx+rx+1.0), (int)(cy+1.0),
								   (int)(cx-rx+1.0), (int)(cy+1.0) );
					Arc(hdcM->hDC, (int)(cx-ry*2+1.0), (int)(cy-ry*3+1.0),
								   (int)(cx+ry*2+1.0), (int)(cy+ry+1.0),
								   (int)(cx-rx+1.0), (int)(cy+1.0),
								   (int)(cx+rx+1.0), (int)(cy+1.0) );
					Ellipse(hdcM->hDC, (int)(cx-ry/2+1.0), (int)(cy-ry+1.0),
									   (int)(cx+ry/2+1.0), (int)(cy+ry+1.0) );
				}
				
				{//ロック状態
					if( lock ){
						hdcM->setPenAndBrush( RGB(255,255,255), NULL );
					}else{
						hdcM->setPenAndBrush( RGB(min(baseR+20,255),min(baseG+20,255),min(baseB+20,255)), NULL );
					}

					double cx= (double)(x2+x3)/2.0;
					double cy= (double)(y0+y1)/2.0;
					double r= (double)(x3-x2)/2.0;
					Ellipse(hdcM->hDC, (int)(cx-r*0.45+0.5), (int)(cy-r*0.65+0.5),
									   (int)(cx+r*0.45+0.5)+1, (int)(cy-r*0.1+0.5) );

					MoveToEx(hdcM->hDC, (int)(cx+0.5),(int)(cy-r*0.1+0.5), NULL);
					LineTo(hdcM->hDC,   (int)(cx+0.5),(int)(cy+r*0.8+0.5) );
					MoveToEx(hdcM->hDC, (int)(cx+0.5),         (int)(cy+r*0.15+0.5), NULL);
					LineTo(hdcM->hDC,   (int)(cx+r*0.35+0.5)+1,(int)(cy+r*0.15+0.5) );
					MoveToEx(hdcM->hDC, (int)(cx+0.5),         (int)(cy+r*0.35+0.5), NULL);
					LineTo(hdcM->hDC,   (int)(cx+r*0.35+0.5)+1,(int)(cy+r*0.35+0.5) );
				}

				//枠
				hdcM->setPenAndBrush(RGB(min(baseR+20,255),min(baseG+20,255),min(baseB+20,255)),NULL);
				Rectangle(hdcM->hDC,x0,y0,x1,y1);
				MoveToEx(hdcM->hDC, x2,y0, NULL);
				LineTo(hdcM->hDC,   x2,y1);
				MoveToEx(hdcM->hDC, x3,y0, NULL);
				LineTo(hdcM->hDC,   x3,y1);
				
			}

		private:
			OWP_LayerTable *parent;
		};
		std::vector<LineData*> lineData;

		static const int LABEL_SIZE_Y= 15;
		static const int SCROLL_BAR_WIDTH= 10;
		static const int MARGIN= 3;
		static const int NAME_POS_X= 5;

		bool rewriteOnChange;		//キー操作時に再描画を行うか否かのフラグ
		bool canMouseControll;		//マウスでの操作が可能か否かのフラグ

		bool dragLine,dragScrollBarLine;
		bool dragVisibleButton,dragVisibleButtonValue;
		bool dragLockButton,dragLockButtonValue;

		int shiftIndex;				//選択行のドラッグでの移動量

		int mouseRBtnOnIndex;		//右クリックが押されたときの行インデックス
	};


	///<summary>
	///	ウィンドウ内部品"スクロールウインドウ"クラス
	///</summary>
	class OWP_ScrollWnd : public OrgWindowParts{
	public:
		//////////////////// Constructor/Destructor //////////////////////
		OWP_ScrollWnd(const TCHAR *_name, bool _allareamousewheel) : OrgWindowParts() {
			name = new TCHAR[256];
			if (_name) {
				size_t tclen = _tcslen(_name);
				size_t cplen;
				if (tclen != 0) {
					if (tclen <= 255) {
						cplen = tclen;
					}
					else {
						cplen = 255;
					}
					_tcsncpy_s(name, 256, _name, cplen);
					name[cplen] = (TCHAR)0;
				}
				else {
					_tcscpy_s(name, 256, TEXT("NoName"));
				}
			}

			//cursorListener = [](){s_dummyfunc(); };
			//lineShiftListener = [](int beforIndex, int afterIndex){s_dummyfunc(); };
			//changeVisibleListener = [](int targetIndex){s_dummyfunc(); };
			//changeLockListener = [](int targetIndex){s_dummyfunc(); };
			//callPropertyListener = [](int targetIndex){s_dummyfunc(); };
			cursorListener = NULL;
			lineShiftListener = NULL;
			changeVisibleListener = NULL;
			changeLockListener = NULL;
			callPropertyListener = NULL;


			currentLine = 0;
			showPosLine = 0;
			lineDatasize = 0;

			rewriteOnChange = true;
			canMouseControll = true;

			dragLine = false;
			dragScrollBarLine = false;
			dragVisibleButton = false;
			dragLockButton = false;

			mouseRBtnOnIndex = -1;

			currentPartsSizeY = 0;
			open = true;
			canClose = true;


			allareamousewheel = _allareamousewheel;//子供にsliderを持たない場合　全クライアント領域でマウスホイール処理を行う

		}
		~OWP_ScrollWnd(){
			delete[] name;
		}

		//////////////////////////// Method //////////////////////////////
		///	Method : 親ウィンドウに登録
		//void regist(OrgWindow *_parentWindow,
		//	WindowPos _pos, WindowSize _size,
		//	HDCMaster* _hdcM,
		//	unsigned char _baseR = 50, unsigned char _baseG = 70, unsigned char _baseB = 70){
		//	_regist(_parentWindow, _pos, _size, _hdcM, _baseR, _baseG, _baseB);

		//	////全てのグループ内部品を同じウィンドウに登録
		//	//for (std::list<OrgWindowParts*>::iterator itr = partsList.begin();
		//	//	itr != partsList.end(); itr++){
		//	//	(*itr)->regist(parentWindow,
		//	//		_pos, _size,
		//	//		hdcM,
		//	//		baseColor.r, baseColor.g, baseColor.b);
		//	//}

		//	//グループボックスと内部要素の位置とサイズを自動設定
		//	autoResize();
		//}
		///	Method : グループ内部品を追加
		void addParts(OrgWindowParts& a){
			partsList.push_back(&a);

			// グループボックスがウィンドウに登録されている場合は
			// グループ内部品も同じウィンドウに登録する
			if (parentWindow != NULL){
				a.registmember(parentWindow,
					pos, size,
					hdcM,
					baseColor.r, baseColor.g, baseColor.b);

				// グループボックスがオープン状態の時は
				// ウィンドウ内の全パーツの位置・サイズを自動調整
				if (open){
					parentWindow->autoResizeAllParts();
				}
			}
		}
		
		/// Method : 自動サイズ設定
		virtual void autoResize(){
			if (!parentWindow) {
				return;
			}

			//size = parentWindow->getSize();//2023/10/08コメントアウト
			////pos = WindowPos(0, 0);//2023/02/17 setPosを反映させるためコメントアウト

			//2023/10/08
			// OrgWindowの子供にとってのOrgWindowのサイズはWindowSize(partsAreaSize.x, partsAreaSize.y)
			// getSize()で計算すると　ウインドウ１クリック後に　スライダー位置が右に６ピクセル(partsAreaPos.x * 2)だけずれる
			size = parentWindow->getClientSize();


			int showLineNum = (size.y) / (LABEL_SIZE_Y);

			int x0 = pos.x + size.x - SCROLL_BAR_WIDTH - 1;
			int x1 = x0 + SCROLL_BAR_WIDTH + 1;
			int y0 = pos.y;
			int y1 = pos.y + size.y;

			//if (open){
				//パーツエリアの位置とサイズを設定
				partsAreaPos = pos;
				partsAreaSize = WindowSize(size.x - SCROLL_BAR_WIDTH - 1, size.y);
				currentPartsSizeY = 0;

				//全ての内部パーツの位置とサイズを自動設定
				int starty = showPosLine * (LABEL_SIZE_Y);
				std::list<OrgWindowParts*>::iterator itr;
				for (itr = partsList.begin(); itr != partsList.end(); itr++){
					if (*itr) {
						(*itr)->setPos(WindowPos(partsAreaPos.x, partsAreaPos.y - starty));
						(*itr)->setSize(WindowSize(partsAreaSize.x, partsAreaSize.y));
						(*itr)->autoResize();

						currentPartsSizeY += (*itr)->getSize().y;
					}
				}

				//グループボックスのサイズを内部要素に合わせてトリミング
				//partsAreaSize.y = currentPartsSizeY;
				//size.y = partsAreaPos.y - pos.y + partsAreaSize.y + 3;

			//}
			//else{
			//	size.y = SIZE_CLOSE_Y;
			//}
		}
		
		//	Method : 描画
		virtual void draw(){
			if (!hdcM) {
				return;
			}

			drawEdge();

			//全ての内部パーツを描画
			if (open){
				std::list<OrgWindowParts*>::iterator itr;
				for (itr = partsList.begin(); itr != partsList.end(); itr++){
					if (*itr) {
						(*itr)->draw();
					}
				}
			}

			int showLineNum = (size.y) / (LABEL_SIZE_Y);

			{//ラベルスクロールバー
				int x0 = pos.x + size.x - SCROLL_BAR_WIDTH - 1;
				int x1 = x0 + SCROLL_BAR_WIDTH + 1;
				int y0 = pos.y;
				int y1 = pos.y + size.y;

				//枠
				//hdcM->setPenAndBrush(RGB(min(baseColor.r + 20, 255), min(baseColor.g + 20, 255), min(baseColor.b + 20, 255)), NULL);
				hdcM->setPenAndBrush(RGB(240, 240, 240), NULL);
				Rectangle(hdcM->hDC, x0, y0, x1, y1);

				//中身
				int barSize = (y1 - y0 - 4)*showLineNum / lineDatasize;
				int barStart = (y1 - y0 - 4)*showPosLine / lineDatasize;
				if (showLineNum < lineDatasize){
					//hdcM->setPenAndBrush(NULL, RGB(min(baseColor.r + 20, 255), min(baseColor.g + 20, 255), min(baseColor.b + 20, 255)));
					hdcM->setPenAndBrush(NULL, RGB(240, 240, 240));
					Rectangle(hdcM->hDC, x0 + 2, y0 + 2 + barStart, x1 - 2, y0 + 2 + barStart + barSize + 1);
				}
			}
		}
		void onLButtonDown(const MouseEvent& e){
			if (!canMouseControll) return;

			int x0 = 0;
			int x1 = LABEL_SIZE_Y * 2;
			int x2 = size.x - SCROLL_BAR_WIDTH - 1;
			int x3 = size.x;
			int y0 = 0;
			int y1 = size.y;

			int showLineNum = (size.y) / (LABEL_SIZE_Y);

			//ラインスクロールバー
			if ((x2 <= e.localX) && (e.localX < x3) && 
				(y0 <= e.localY) && (e.localY < y1)){
				showLineNum = (y1 - y0) / (LABEL_SIZE_Y);
				if (showLineNum < lineDatasize){
					int barSize = (y1 - y0) * showLineNum / lineDatasize;

					int movableY = y1 - y0 - barSize;
					int movableYStart = y0 + barSize / 2;

					setShowPosLine((e.localY - movableYStart)*(lineDatasize - showLineNum) / movableY);

					dragScrollBarLine = true;
				}
			}
			autoResize();

			//内部パーツ
			std::list<OrgWindowParts*>::iterator plItr;
			for (plItr = partsList.begin(); plItr != partsList.end(); plItr++){
				if (*plItr) {
					WindowSize partsSize = (*plItr)->getSize();
					int tmpPosX = e.localX + pos.x - (*plItr)->getPos().x;
					int tmpPosY = e.localY + pos.y - (*plItr)->getPos().y;

					MouseEvent mouseEvent;
					mouseEvent.globalX = e.globalX;
					mouseEvent.globalY = e.globalY;
					mouseEvent.localX = tmpPosX;
					mouseEvent.localY = tmpPosY;
					mouseEvent.altKey = e.altKey;
					mouseEvent.shiftKey = e.shiftKey;
					mouseEvent.ctrlKey = e.ctrlKey;

					(*plItr)->onLButtonDown(mouseEvent);
				}
			}

		}
		//	Method : 左マウスボタン ダブルクリックイベント受信
		virtual void onLButtonDBLCLK(const MouseEvent& e) {//2023/10/04
		}
		//	Method : 右マウスボタン ダブルクリックイベント受信
		virtual void onRButtonDBLCLK(const MouseEvent& e) {//2023/10/04
			int dbgflag1 = 1;
		}


		///	Method : 左マウスボタンアップイベント受信
		void onLButtonUp(const MouseEvent& e){
			if (!canMouseControll) return;

			//ドラッグフラグを初期化
			dragLine = false;
			dragScrollBarLine = false;
			dragVisibleButton = false;
			dragLockButton = false;

			autoResize();

			//内部パーツ
			std::list<OrgWindowParts*>::iterator plItr;
			for (plItr = partsList.begin(); plItr != partsList.end(); plItr++){
				if (*plItr) {
					WindowSize partsSize = (*plItr)->getSize();
					int tmpPosX = e.localX + pos.x - (*plItr)->getPos().x;
					int tmpPosY = e.localY + pos.y - (*plItr)->getPos().y;

					MouseEvent mouseEvent;
					mouseEvent.globalX = e.globalX;
					mouseEvent.globalY = e.globalY;
					mouseEvent.localX = tmpPosX;
					mouseEvent.localY = tmpPosY;
					mouseEvent.altKey = e.altKey;
					mouseEvent.shiftKey = e.shiftKey;
					mouseEvent.ctrlKey = e.ctrlKey;

					(*plItr)->onLButtonUp(mouseEvent);
				}
			}

			//再描画要求
			if (rewriteOnChange){
				callRewrite();
			}
		}
		///	Method : マウス移動イベント受信
		void onMouseMove(const MouseEvent& e){
			if (!canMouseControll) return;

			int x0 = 0;
			int x1 = size.x - SCROLL_BAR_WIDTH - 1;
			int x2 = size.x;
			int y0 = 0;
			int y1 = size.y;

			//ラベルスクロールバー
			if (dragScrollBarLine){
				int showLineNum = (y1 - y0) / (LABEL_SIZE_Y);
				int barSize = (y1 - y0) * showLineNum / lineDatasize;

				int movableY = y1 - y0 - barSize;
				int movableYStart = y0 + barSize / 2;

				setShowPosLine((e.localY - movableYStart)*(lineDatasize - showLineNum) / movableY);

			}


			autoResize();

		}
		///	Method : 右マウスボタンダウンイベント受信
		void onRButtonDown(const MouseEvent& e){
			if (!canMouseControll) return;

			//内部パーツ
			std::list<OrgWindowParts*>::iterator plItr;
			for (plItr = partsList.begin(); plItr != partsList.end(); plItr++) {
				if (*plItr) {
					WindowSize partsSize = (*plItr)->getSize();
					int tmpPosX = e.localX + pos.x - (*plItr)->getPos().x;
					int tmpPosY = e.localY + pos.y - (*plItr)->getPos().y;

					MouseEvent mouseEvent;
					mouseEvent.globalX = e.globalX;
					mouseEvent.globalY = e.globalY;
					mouseEvent.localX = tmpPosX;
					mouseEvent.localY = tmpPosY;
					mouseEvent.altKey = e.altKey;
					mouseEvent.shiftKey = e.shiftKey;
					mouseEvent.ctrlKey = e.ctrlKey;

					(*plItr)->onRButtonDown(mouseEvent);
				}
			}
		}
		///	Method : 右マウスボタンアップイベント受信
		void onRButtonUp(const MouseEvent& e){
			if (!canMouseControll) return;

			mouseRBtnOnIndex = -1;
		}

		virtual void onMouseWheel(const MouseEvent& e) {
			int x0;
			int x1 = size.x;
			int y0 = 0;
			int y1 = size.y;


			//2023/10/08
			if (allareamousewheel) {//allareamousewheelはコンストラクタで設定
				x0 = 0;//全クライアントエリア
			}
			else {
				x0 = size.x - SCROLL_BAR_WIDTH - 1;//スクロールバー上だけ
			}


			if ((e.localX >= x0) && (e.localX <= x1) && 
				(e.localY >= y0) && (e.localY <= y1)) {

				int wheeldelta = e.wheeldelta;
				int linedelta= 0;
				if (wheeldelta > 0) {
					linedelta = -1;
				}
				else if (wheeldelta < 0) {
					linedelta = 1;
				}
				else {
					linedelta = 0;
				}

				//int showLineNum = (y1 - y0) / (LABEL_SIZE_Y);
				//int barSize = (y1 - y0) * showLineNum / lineDatasize;
				//int movableY = y1 - y0 - barSize;
				//int movableYStart = y0 + barSize / 2;

				int curline = getShowPosLine();

				setShowPosLine(curline + linedelta);

				autoResize();
			}
		}

		//////////////////////////// Accessor //////////////////////////////
		void setLineDataSize(int srcsize){
			lineDatasize = srcsize;
		}
		/// Accessor : name
		const TCHAR* getName() const{
			return name;
		}
		void setName(const TCHAR *value){
			if (name && value) {
				size_t tclen = _tcslen(value);
				size_t cplen;
				if (tclen != 0) {
					if (tclen <= 255) {
						cplen = tclen;
					}
					else {
						cplen = 255;
					}
					_tcsncpy_s(name, 256, value, cplen);
					name[cplen] = (TCHAR)0;
				}
				else {
					_tcscpy_s(name, 256, TEXT("NoName"));
				}
			}

			//再描画要求
			if (rewriteOnChange){
				callRewrite();
			}
		}
		/// Accessor : rewriteOnChange
		bool getRewriteOnChangeFlag() const{
			return rewriteOnChange;
		}
		void setRewriteOnChangeFlag(bool value){
			rewriteOnChange = value;
		}
		//	Accessor : currentLine
		int getCurrentLine() const{
			return currentLine;
		}
		//	Accessor : showPosLine
		int getShowPosLine() const{
			return showPosLine;
		}
		void setShowPosLine(int _showPosLine){
			int y0 = 0;
			int y1 = size.y;

			int showLineNum = (size.y) / (LABEL_SIZE_Y);
			if (showLineNum < lineDatasize){
				showPosLine = max(0, min(_showPosLine, lineDatasize - showLineNum));
			}
			else{
				showPosLine = 0;
			}

			autoResize();//2023/02/17

			//再描画要求
			if (rewriteOnChange){
				callRewrite();
			}
		}
		void inView(int srcline)
		{
			int showLineNum = (size.y) / (LABEL_SIZE_Y);
			//int currentline = getCurrentLine();

			if ((srcline >= showPosLine) && (srcline <= (showPosLine + showLineNum))) {
				//2023/02/17
				//currentlineが　既に表示範囲に入っている場合には　変更しないで再描画
				autoResize();
				if (rewriteOnChange) {
					callRewrite();
				}
			}
			else {
				setShowPosLine(srcline - showLineNum + 3);
			}
		}

		///	Accessor : cursorListener
		void setCursorListener(std::function<void()> listener){
			this->cursorListener = listener;
		}
		///	Accessor : lineShiftListener
		void setLineShiftListener(std::function<void(int beforIndex, int afterIndex)> listener){
			this->lineShiftListener = listener;
		}
		///	Accessor : changeVisibleListener
		void setChangeVisibleListener(std::function<void(int targetIndex)> listener){
			this->changeVisibleListener = listener;
		}
		///	Accessor : changeLockListener
		void setChangeLockListener(std::function<void(int targetIndex)> listener){
			this->changeLockListener = listener;
		}
		///	Accessor : callPropertyListener
		void setCallPropertyListener(std::function<void(int targetIndex)> listener){
			this->callPropertyListener = listener;
		}


		WindowPos getPartsAreaPos() {
			return partsAreaPos;
		}


	private:
		////////////////////////// MemberVar /////////////////////////////
		int currentLine, showPosLine;
		TCHAR *name;
		std::function<void()> cursorListener;										//カーソル位置が変更された直後に呼ばれる
		std::function<void(int beforIndex, int afterIndex)> lineShiftListener;		//移動が行われる直前に呼ばれる
		std::function<void(int targetIndex)> changeVisibleListener;				//可視状態が変更された直後に呼ばれる
		std::function<void(int targetIndex)> changeLockListener;					//ロック状態が変更された直後に呼ばれる
		std::function<void(int targetIndex)> callPropertyListener;					//レイヤーのプロパティを呼ぶ時に呼ばれる

		int lineDatasize;
		
		static const int LABEL_SIZE_Y = 15;
		//static const int SCROLL_BAR_WIDTH = 10;
		static const int SCROLL_BAR_WIDTH = 20;
		static const int MARGIN = 3;
		static const int NAME_POS_X = 5;

		bool rewriteOnChange;		//キー操作時に再描画を行うか否かのフラグ
		bool canMouseControll;		//マウスでの操作が可能か否かのフラグ

		bool dragLine, dragScrollBarLine;
		bool dragVisibleButton, dragVisibleButtonValue;
		bool dragLockButton, dragLockButtonValue;

		int shiftIndex;				//選択行のドラッグでの移動量

		int mouseRBtnOnIndex;		//右クリックが押されたときの行インデックス


		int currentPartsSizeY;
		WindowPos  partsAreaPos;
		WindowSize partsAreaSize;
		std::list<OrgWindowParts*> partsList;

		bool open;
		bool canClose;
		std::function<void()> openListener;

		static const int SIZE_CLOSE_Y = 15;
		static const int BOX_POS_X = 3;
		//static const int BOX_WIDTH = 10;
		static const int BOX_WIDTH = 20;
		static const int NAME_POS_X1 = 5;
		static const int NAME_POS_X2 = 3;

		bool allareamousewheel;//子供にsliderを持たない場合　全クライアント領域でマウスホイール処理を行う
	};


//void s_dummyfuncKey( KeyboardEvent& keye )
//{
//	static int dummycnt = 0;
//	dummycnt++;
//}

}

#endif
