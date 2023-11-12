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


#include <d3dx9.h>


struct KeyInfo{
	const TCHAR *label;
	double time;
	int timeIndex,lineIndex;
	//ChaVector3 eul, tra;
	ChaMatrix wmat;
	void *object;
};

namespace OrgWinGUI{

static void s_dummyfunc();

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
		int localX,localY;
		int globalX,globalY;
		bool shiftKey,ctrlKey,altKey;
	};

	class KeyboardEvent{
	public:
		int keyCode;
		bool shiftKey,ctrlKey,altKey;
		bool repeat;
		bool onDown;
	};

	static void s_dummyfuncKey( KeyboardEvent& keye );


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
		}
		HDCMaster( const HDCMaster& a ){
			_ASSERT_EXPR( 0, L"コピーコンストラクタは使えません" );
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
			parentWindow=NULL;
		}
		OrgWindowParts( const OrgWindowParts& a ){
			operator=(a);
		}
		virtual ~OrgWindowParts(){
		}

		////////////////////////// MemberVar /////////////////////////////
		struct color_tag{
			unsigned char r,g,b;
		};

		//////////////////////////// Method //////////////////////////////
		//	Method : 親ウィンドウに登録
		virtual void regist( OrgWindow *_parentWindow,
							 WindowPos _pos, WindowSize _size,
							 HDCMaster* _hdcM,
							 unsigned char _baseR=50, unsigned char _baseG=70, unsigned char _baseB=70 ){
			_regist(_parentWindow,_pos,_size,_hdcM,_baseR,_baseG,_baseB);
			autoResize();
		}
		/// Method : 自動サイズ設定
		virtual void autoResize(){

		}
		//	Method : 描画
		virtual void draw(){
			drawEdge();
		}
		/// Method : 再描画要求を送る
		virtual void callRewrite();
		//	Method : 左マウスボタンダウンイベント受信
		virtual void onLButtonDown(const MouseEvent& e){
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
		//	Method : キーダウンイベント受信
		virtual void onKeyDown(const KeyboardEvent& e){
		}
		//	Method : キーアップイベント受信
		virtual void onKeyUp(const KeyboardEvent& e){
		}

		/////////////////////////// Operator /////////////////////////////
		void operator=(const OrgWindow& a){
			_ASSERT_EXPR( 0, L"コピーコンストラクタは使えません" );
		}

		/////////////////////////// Accessor /////////////////////////////
		//	Accessor : pos
		virtual WindowPos getPos() const{
			return WindowPos(pos.x,pos.y);
		}
		virtual void setPos(const WindowPos& _pos){
			pos= _pos;
	//		draw()
		}
		//	Accessor : size
		virtual WindowSize getSize() const{
			return WindowSize(size.x,size.y);
		}
		virtual void setSize(const WindowSize& _size){
			size= _size;
	//		draw();
		}
		//	Accessor : color
		color_tag getBaseColor() const{
			return baseColor;
		}
		void setColor(color_tag _color){
			baseColor= _color;
	//		draw();
		}

	protected:
		////////////////////////// MemberVar /////////////////////////////
		OrgWindow *parentWindow;
		WindowPos  pos;
		WindowSize size;
		color_tag baseColor;

		HDCMaster *hdcM;

		//////////////////////////// Method //////////////////////////////
		//	Method : 親ウィンドウに登録
		void _regist( OrgWindow *_parentWindow,
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
		}
		//	Method : 枠を描画
		void drawEdge(bool fill=true){
			if(fill){
				hdcM->setPenAndBrush(RGB(min(baseColor.r+20,255),min(baseColor.g+20,255),min(baseColor.b+20,255)),RGB(baseColor.r,baseColor.g,baseColor.b));
			}else{
				hdcM->setPenAndBrush(RGB(min(baseColor.r+20,255),min(baseColor.g+20,255),min(baseColor.b+20,255)),NULL);
			}
			Rectangle(hdcM->hDC,pos.x,pos.y,pos.x+size.x,pos.y+size.y);
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
				   unsigned char _baseR=50, unsigned char _baseG=70, unsigned char _baseB=70,
				   bool _canQuit=true, bool _canChangeSize=true ){

			istopmost = srcistopmost;

			//イベントリスナー
			closeListener = [](){s_dummyfunc();};
			keyboardListener = [](const KeyboardEvent &e){s_dummyfunc();};
			lupListener = [](){s_dummyfunc();};

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

			partsList.clear();

			//ウィンドウクラスを登録
			registerWindowClass();

			//ウィンドウ作成
			create();

			//ウィンドウ表示
			ShowWindow(hWnd, SW_SHOW);

			beginPaint();
				paintTitleBar();
			endPaint();

			UpdateWindow(hWnd);

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

			delete[] szclassName;
			delete[] title;
		}

		//////////////////////////// Method //////////////////////////////
		//	Method : ウィンドウ内部品を追加
		void addParts(OrgWindowParts& a){
			a.regist( this,
					  WindowPos(partsAreaPos.x,partsAreaPos.y+currentPartsSizeY),
					  WindowSize(partsAreaSize.x,partsAreaSize.y-currentPartsSizeY),
					  &hdcM,
					  baseR,baseG,baseB);
			currentPartsSizeY+= a.getSize().y+1;
			partsList.push_back(&a);

			if( partsAreaPos.y+currentPartsSizeY+2 >= size.y ){		//ウィンドウからはみ出る場合はサイズを調整
				setSize( WindowSize(size.x, partsAreaPos.y+currentPartsSizeY+2) );
			}

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

			//再描画領域
			RECT tmpRect;
			tmpRect.left=   0;
			tmpRect.top=    0;
			tmpRect.right=  size.x;
			tmpRect.bottom= size.y;
			InvalidateRect( hWnd, &tmpRect, false );
		}
		/// Method : ウィンドウ内部品を全て追加しなおす
		void reAddAllParts(){
			std::list<OrgWindowParts*> tmpPartsList= partsList;
			partsList.clear();
			currentPartsSizeY= 0;
			for(std::list<OrgWindowParts*>::iterator itr= tmpPartsList.begin();
				itr!=tmpPartsList.end(); itr++){
				addParts(**itr);
			}
		}
		/// Method : ウィンドウ内部品の位置・サイズを全て自動設定する
		void autoResizeAllParts(){
			currentPartsSizeY= 0;

			//全ての内部要素に対して位置・サイズを自動設定
			for(std::list<OrgWindowParts*>::iterator itr=partsList.begin();
				itr!=partsList.end(); itr++){

				(*itr)->setPos(  WindowPos(  partsAreaPos.x,  partsAreaPos.y+currentPartsSizeY  ) );
				(*itr)->setSize( WindowSize( partsAreaSize.x, partsAreaSize.y-currentPartsSizeY ) );
				(*itr)->autoResize();

				currentPartsSizeY+= (*itr)->getSize().y+1;
			}

			//再描画要求を送る
			callRewrite();
		}

		/////////////////////////// Operator /////////////////////////////
		void operator=(const OrgWindow& a){
			_ASSERT_EXPR( 0, L"コピーコンストラクタは使えません" );
		}

		/////////////////////////// Accessor /////////////////////////////
		//	Accessor : pos
		WindowPos getPos(){
			if( hWnd==NULL ) _ASSERT_EXPR( 0, L"hWnd = NULL" );

			refreshPosAndSize();
			return WindowPos(pos.x,pos.y);
		}
		void setPos(const WindowPos& _pos){
			if( hWnd==NULL ) _ASSERT_EXPR( 0, L"hWnd = NULL" );

			pos= _pos;
			MoveWindow(hWnd, pos.x,pos.y, size.x,size.y, true);
		}
		//	Accessor : size
		WindowSize getSize(){
			if( hWnd==NULL ) _ASSERT_EXPR( 0, L"hWnd = NULL" );

			refreshPosAndSize();
			return size;
		}
		void setSize(const WindowSize& _size){
			if( hWnd==NULL ) _ASSERT_EXPR( 0, L"hWnd = NULL" );

			if( _size.x<sizeMin.x ) size.x= sizeMin.x;
			else					size.x= _size.x;
			if( _size.y<sizeMin.y ) size.y= sizeMin.y;
			else					size.y= _size.y;

			MoveWindow(hWnd, pos.x,pos.y, size.x,size.y, true);
		}
		//	Accessor : sizeMin
		WindowSize getSizeMin(){
			if( hWnd==NULL ) _ASSERT_EXPR( 0, L"hWnd = NULL" );

			return sizeMin;
		}
		void setSizeMin(const WindowSize& _sizeMin){
			if( hWnd==NULL ) _ASSERT_EXPR( 0, L"hWnd = NULL" );

			sizeMin= _sizeMin;
			if( size.x<sizeMin.x || size.y<sizeMin.y ){
				if( size.x<sizeMin.x ) size.x= sizeMin.x;
				if( size.y<sizeMin.y ) size.y= sizeMin.y;

				MoveWindow(hWnd, pos.x,pos.y, size.x,size.y, true);
			}
		}
		//	Accessor : hWnd
		HWND getHWnd(){
			return hWnd;
		}
		//	Accessor : closeListener
		void setCloseListener(std::tr1::function<void()> listener){
			this->closeListener= listener;
		}
		void setLUpListener(std::tr1::function<void()> listener){
			this->lupListener= listener;
		}
		/// Accessor : keyboardListener
		void setKeyboardEventListener(std::tr1::function<void(const KeyboardEvent&)> listener){
			this->keyboardListener= listener;
		}
		//	Accessor : visible
		bool getVisible(){
			return visible;
		}
		void setVisible(bool value){
			if( value ){
				ShowWindow(hWnd, SW_SHOWNA);
			}else{
				ShowWindow(hWnd, SW_HIDE);
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

		HDCMaster hdcM;

		TCHAR *title;
		unsigned char baseR,baseG,baseB;
		bool canQuit;

		std::list<OrgWindowParts*> partsList;

		static std::map<HWND,OrgWindow*> hWndAndClassMap;

		//ユーザーイベントリスナー
		std::tr1::function<void()> closeListener;
		std::tr1::function<void()> lupListener;
		std::tr1::function<void(const KeyboardEvent&)> keyboardListener;

		//マウスキャプチャ用のフラグ
		bool mouseCaptureFlagL,mouseCaptureFlagR;

		//////////////////////////// Method //////////////////////////////
		//	Method : ウィンドウクラスを登録
		void registerWindowClass(){
			ZeroMemory((LPVOID)&wcex, sizeof(WNDCLASSEX));

			wcex.cbSize			= sizeof(WNDCLASSEX);
			wcex.style			= 0;
			wcex.lpfnWndProc	= wndProc;
			wcex.cbClsExtra		= 0;
			wcex.cbWndExtra		= 0;
			wcex.hInstance		= hInstance;
			wcex.hIcon			= NULL;
			wcex.hCursor		= LoadCursor(NULL,IDC_ARROW);
			wcex.hbrBackground	= ( HBRUSH)( COLOR_WINDOW+1);
			wcex.lpszMenuName	= NULL;
			wcex.lpszClassName	= szclassName;
			wcex.hIconSm		= NULL;
			RegisterClassEx(&wcex);
		}
		//	Method : ウィンドウ作成
		void create(){
			if( istopmost ){
				hWnd=CreateWindowEx(WS_EX_TOOLWINDOW|WS_EX_TOPMOST,szclassName,title,WS_POPUP,
					pos.x,  pos.y,
					size.x, size.y,
					hWndParent,NULL,hInstance,NULL);
			}else{
				hWnd=CreateWindowEx(WS_EX_TOOLWINDOW,szclassName,title,WS_POPUP,
					pos.x,  pos.y,
					size.x, size.y,
					hWndParent,NULL,hInstance,NULL);
			}
			hdcM.setHWnd(hWnd);

			hWndAndClassMap[hWnd]= this;	//hWndとこのクラスのインスタンスポインタの対応表更新
		}
		//	Method : ウィンドウプロシージャ
		static LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
		//	Method : ウィンドウ位置とサイズの更新
		void refreshPosAndSize(){
			RECT tmpRect;
			GetWindowRect(hWnd, &tmpRect);
			pos.x= tmpRect.left;
			pos.y= tmpRect.top;
			size.x= tmpRect.right-  tmpRect.left;
			size.y= tmpRect.bottom- tmpRect.top;

			//最小ウィンドウサイズ未満になって居ないかどうか確認
			if( size.x<sizeMin.x || size.y<sizeMin.y ){
				if( size.x<sizeMin.x ) size.x= sizeMin.x;
				if( size.y<sizeMin.y ) size.y= sizeMin.y;
				setSize(size);
			}

			partsAreaSize= WindowSize( size.x-partsAreaPos.x-3, size.y-partsAreaPos.y-3 );
		}
		///	Method : 左右マウスボタンダウンイベント受信
		void onLButtonDown(const MouseEvent& e){
			onLRButtonDown(e,true);
		}
		void onRButtonDown(const MouseEvent& e){
			onLRButtonDown(e,false);
		}
		void onLRButtonDown(const MouseEvent& e, bool lButton){
			int xButtonX1=size.x-1-2-9;
			int xButtonY1=1+2;
			int xButtonX2=xButtonX1+9;
			int xButtonY2=xButtonY1+9;
			refreshPosAndSize();

			if( lButton ){
				//タイトルバーのXボタン
				if( xButtonX1<=e.localX && e.localX<=xButtonX2-1
				 && xButtonY1<=e.localY && e.localY<=xButtonY2-1
				 && canQuit ){
					if(this->closeListener!=NULL){
						(this->closeListener)();
					}
					return;
				}

				//タイトルバー
				if( 1<=e.localX && e.localX<=size.x-2
				 && 1<=e.localY && e.localY<=1+2+9+1 ){
					SendMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
					return;
				}

				//右下の隅
				if( canChangeSize &&
					size.x-4<=e.localX && size.y-4<=e.localY ){
					SendMessage(hWnd, WM_NCLBUTTONDOWN, HTBOTTOMRIGHT, 0);
					return;
				}
			}

			//マウスキャプチャ
			if( !mouseCaptureFlagL && !mouseCaptureFlagR ) SetCapture(hWnd);
			if( lButton ) mouseCaptureFlagL=true;
			else		  mouseCaptureFlagR=true;

			//内部パーツ
			for( std::list<OrgWindowParts*>::iterator plItr=partsList.begin();
				 plItr!=partsList.end();
				 plItr++ ){
				WindowSize partsSize= (*plItr)->getSize();
				int tmpPosX= e.localX- (*plItr)->getPos().x;
				int tmpPosY= e.localY- (*plItr)->getPos().y;
				if( 0<=tmpPosX && tmpPosX<partsSize.x &&
					0<=tmpPosY && tmpPosY<partsSize.y ){

					MouseEvent mouseEvent;
					mouseEvent.globalX= e.globalX;
					mouseEvent.globalY= e.globalY;
					mouseEvent.localX= tmpPosX;
					mouseEvent.localY= tmpPosY;
					mouseEvent.altKey= e.altKey;
					mouseEvent.shiftKey= e.shiftKey;
					mouseEvent.ctrlKey= e.ctrlKey;

					if( lButton ){
						(*plItr)->onLButtonDown(mouseEvent);
					}else{
						(*plItr)->onRButtonDown(mouseEvent);
					}
					return;
				}
			}
		}
		//	Method : 左右マウスボタンアップイベント受信
		void onLButtonUp(const MouseEvent& e){
			if( this->lupListener!=NULL ){
				(this->lupListener)();
			}
			onLRButtonUp(e,true);
		}
		void onRButtonUp(const MouseEvent& e){
			onLRButtonUp(e,false);
		}
		void onLRButtonUp(const MouseEvent& e, bool lButton){

			//マウスキャプチャリリース
			if( lButton ) mouseCaptureFlagL=false;
			else		  mouseCaptureFlagR=false;
			if( !mouseCaptureFlagL && !mouseCaptureFlagR ) ReleaseCapture();

			//内部パーツ
			for( std::list<OrgWindowParts*>::iterator plItr=partsList.begin();
				 plItr!=partsList.end();
				 plItr++ ){

				MouseEvent mouseEvent;
				mouseEvent.globalX= e.globalX;
				mouseEvent.globalY= e.globalY;
				mouseEvent.localX= e.localX- (*plItr)->getPos().x;
				mouseEvent.localY= e.localY- (*plItr)->getPos().y;
				mouseEvent.altKey= e.altKey;
				mouseEvent.shiftKey= e.shiftKey;
				mouseEvent.ctrlKey= e.ctrlKey;

				if( lButton ){
					(*plItr)->onLButtonUp(mouseEvent);
				}else{
					(*plItr)->onRButtonUp(mouseEvent);
				}
			}
		}
		//	Method : マウス移動イベント受信
		void onMouseMove(const MouseEvent& e){

			//右下の隅
			if( canChangeSize && 
				size.x-4<=e.localX && size.y-4<=e.localY ){
				SetCursor((HCURSOR)LoadImage(NULL, IDC_SIZENWSE,IMAGE_CURSOR,
											 NULL, NULL,LR_DEFAULTCOLOR | LR_SHARED));
			}

			//内部パーツ
			for( std::list<OrgWindowParts*>::iterator plItr=partsList.begin();
				 plItr!=partsList.end();
				 plItr++ ){

				MouseEvent mouseEvent;
				mouseEvent.globalX= e.globalX;
				mouseEvent.globalY= e.globalY;
				mouseEvent.localX= e.localX- (*plItr)->getPos().x;
				mouseEvent.localY= e.localY- (*plItr)->getPos().y;
				mouseEvent.altKey= e.altKey;
				mouseEvent.shiftKey= e.shiftKey;
				mouseEvent.ctrlKey= e.ctrlKey;

				(*plItr)->onMouseMove(mouseEvent);
			}
		}
		//	Method : キーボードイベント受信
		void onKeyboard(const KeyboardEvent& e){
			if( this->keyboardListener!=NULL ){
				(this->keyboardListener)(e);
			}
		}
		//	Method : 全描画
		void allPaint(){
			beginPaint();
				paintTitleBar();
				for( std::list<OrgWindowParts*>::iterator itr=partsList.begin();
						itr!= partsList.end();
						itr++ ){
					(*itr)->draw();
				}
			endPaint();
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
				TextOut(hdcM.hDC, 10, 2, title, _tcslen(title));			//タイトル名
			}
		}
	};

	////////////////----------------------------------------////////////////
	////////////////										////////////////
	////////////////		ウィンドウ内部品クラス			////////////////
	////////////////										////////////////
	////////////////----------------------------------------////////////////

	///<summary>
	///	ウィンドウ内部品"セパレータ"クラス
	///</summary>
	class OWP_Separator : public OrgWindowParts{
	public:
		//////////////////// Constructor/Destructor //////////////////////
		OWP_Separator(bool _divideSide=true){
			currentPartsSizeY1= 0;
			currentPartsSizeY2= 0;

			canShift= true;
			centerRate= 0.5;
			divideSide= _divideSide;
			shiftDrag= false;
		}
		~OWP_Separator(){
		}

		//////////////////////////// Method //////////////////////////////
		///	Method : 親ウィンドウに登録
		void regist( OrgWindow *_parentWindow,
							 WindowPos _pos, WindowSize _size,
							 HDCMaster* _hdcM,
							 unsigned char _baseR=50, unsigned char _baseG=70, unsigned char _baseB=70 ){
			_regist(_parentWindow,_pos,_size,_hdcM,_baseR,_baseG,_baseB);

			//全てのグループ内部品を同じウィンドウに登録
			for(std::list<OrgWindowParts*>::iterator itr=partsList1.begin();
				itr!=partsList1.end(); itr++){
				(*itr)->regist( parentWindow,
								_pos,_size,
								hdcM,
								baseColor.r,baseColor.g,baseColor.b);
			}
			for(std::list<OrgWindowParts*>::iterator itr=partsList2.begin();
				itr!=partsList2.end(); itr++){
				(*itr)->regist( parentWindow,
								_pos,_size,
								hdcM,
								baseColor.r,baseColor.g,baseColor.b);
			}

			//グループボックスと内部要素の位置とサイズを自動設定
			autoResize();
		}
		///	Method : グループ内部品を追加
		void addParts1(OrgWindowParts& a){
			partsList1.push_back(&a);

			// グループボックスがウィンドウに登録されている場合は
			// グループ内部品も同じウィンドウに登録する
			if( parentWindow!=NULL ){
				a.regist( parentWindow,
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
				a.regist( parentWindow,
						  pos, size,
						  hdcM,
						  baseColor.r,baseColor.g,baseColor.b);

				//ウィンドウ内の全パーツの位置・サイズを自動調整
				parentWindow->autoResizeAllParts();
			}
		}
		/// Method : 自動サイズ設定
		void autoResize(){
			//パーツエリアの位置とサイズを設定
			int centerPos= getCenterLinePos();
			partsAreaPos1=  pos;
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
			for(std::list<OrgWindowParts*>::iterator itr=partsList1.begin();
				itr!=partsList1.end(); itr++){

				(*itr)->setPos(  WindowPos(  partsAreaPos1.x,  partsAreaPos1.y+currentPartsSizeY1  ) );
				(*itr)->setSize( WindowSize( partsAreaSize1.x, partsAreaSize1.y-currentPartsSizeY1 ) );
				(*itr)->autoResize();

				currentPartsSizeY1+= (*itr)->getSize().y+1;
			}
			for(std::list<OrgWindowParts*>::iterator itr=partsList2.begin();
				itr!=partsList2.end(); itr++){

				(*itr)->setPos(  WindowPos(  partsAreaPos2.x,  partsAreaPos2.y+currentPartsSizeY2  ) );
				(*itr)->setSize( WindowSize( partsAreaSize2.x, partsAreaSize2.y-currentPartsSizeY2 ) );
				(*itr)->autoResize();

				currentPartsSizeY2+= (*itr)->getSize().y+1;
			}
		}
		///	Method : 描画
		void draw(){
			//枠を書く
			int centerPos= getCenterLinePos();
			hdcM->setPenAndBrush(RGB(min(baseColor.r+20,255),min(baseColor.g+20,255),min(baseColor.b+20,255)),NULL);
			if( divideSide ){
				MoveToEx(hdcM->hDC, pos.x+centerPos,pos.y+1, NULL);
				LineTo(hdcM->hDC,   pos.x+centerPos,pos.y+size.y-1);
				if( canShift ){
					hdcM->setPenAndBrush(RGB(240,240,240),NULL);
					int centerPos2= pos.y+size.y/2;
					MoveToEx(hdcM->hDC, pos.x+centerPos,centerPos2-HANDLE_MARK_SIZE/2, NULL);
					LineTo(hdcM->hDC,   pos.x+centerPos,centerPos2+HANDLE_MARK_SIZE/2);
				}
			}else{
				MoveToEx(hdcM->hDC, pos.x+1,pos.y+centerPos, NULL);
				LineTo(hdcM->hDC,   pos.x+size.x-1,pos.y+centerPos);
				if( canShift ){
					hdcM->setPenAndBrush(RGB(240,240,240),NULL);
					int centerPos2= pos.x+size.x/2;
					MoveToEx(hdcM->hDC, centerPos2-HANDLE_MARK_SIZE/2,pos.y+centerPos, NULL);
					LineTo(hdcM->hDC,   centerPos2+HANDLE_MARK_SIZE/2,pos.y+centerPos);
				}
			}

			//全ての内部パーツを描画
			for(std::list<OrgWindowParts*>::iterator itr=partsList1.begin();
				itr!=partsList1.end(); itr++){
					(*itr)->draw();
			}
			for(std::list<OrgWindowParts*>::iterator itr=partsList2.begin();
				itr!=partsList2.end(); itr++){
					(*itr)->draw();
			}
		}
		///	Method : 左右マウスボタンダウンイベント受信
		void onLButtonDown(const MouseEvent& e){
			onLRButtonDown(e,true);
		}
		void onRButtonDown(const MouseEvent& e){
			onLRButtonDown(e,false);
		}
		///	Method : 左右マウスボタンアップイベント受信
		void onLButtonUp(const MouseEvent& e){
			onLRButtonUp(e,true);
		}
		void onRButtonUp(const MouseEvent& e){
			onLRButtonUp(e,false);
		}
		///	Method : マウス移動イベント受信
		void onMouseMove(const MouseEvent& e){

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
			for( std::list<OrgWindowParts*>::iterator plItr=partsList1.begin();
				 plItr!=partsList1.end();
				 plItr++ ){

				MouseEvent mouseEvent;
				mouseEvent.globalX= e.globalX;
				mouseEvent.globalY= e.globalY;
				mouseEvent.localX= e.localX+pos.x- (*plItr)->getPos().x;
				mouseEvent.localY= e.localY+pos.y- (*plItr)->getPos().y;
				mouseEvent.altKey= e.altKey;
				mouseEvent.shiftKey= e.shiftKey;
				mouseEvent.ctrlKey= e.ctrlKey;

				(*plItr)->onMouseMove(mouseEvent);
			}
			for( std::list<OrgWindowParts*>::iterator plItr=partsList2.begin();
				 plItr!=partsList2.end();
				 plItr++ ){

				MouseEvent mouseEvent;
				mouseEvent.globalX= e.globalX;
				mouseEvent.globalY= e.globalY;
				mouseEvent.localX= e.localX+pos.x- (*plItr)->getPos().x;
				mouseEvent.localY= e.localY+pos.y- (*plItr)->getPos().y;
				mouseEvent.altKey= e.altKey;
				mouseEvent.shiftKey= e.shiftKey;
				mouseEvent.ctrlKey= e.ctrlKey;

				(*plItr)->onMouseMove(mouseEvent);
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

		static const int LINE_MARGIN= 1;
		static const int HANDLE_SIZE= 12;
		static const int HANDLE_MARK_SIZE= 6;

		/// Method : 仕切り線の位置を取得
		int getCenterLinePos() const{
			int centerPos= (int)(centerRate*(double)(divideSide?size.x:size.y));
			centerPos= max(1, min(centerPos, (divideSide?size.x:size.y)-2));
			return centerPos;
		}
		/// Method : 仕切り線に触れているかどうか取得
		bool isMouseOnHandle(const MouseEvent& e) const{
			int centerPos= getCenterLinePos();
			if( centerPos-LINE_MARGIN <= (divideSide?e.localX:e.localY)
			 && (divideSide?e.localX:e.localY) <= centerPos+LINE_MARGIN ){

				int centerPos2= (divideSide?size.y:size.x)/2;
				if( centerPos2-HANDLE_SIZE/2 <= (divideSide?e.localY:e.localX)
				 && (divideSide?e.localY:e.localX) <= centerPos2+HANDLE_SIZE/2 ){
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
			}else if( (divideSide?e.localX:e.localY) <= getCenterLinePos() ){

				for(std::list<OrgWindowParts*>::iterator plItr=partsList1.begin();
					plItr!=partsList1.end(); plItr++ ){

					WindowSize partsSize= (*plItr)->getSize();
					int tmpPosX= e.localX+pos.x- (*plItr)->getPos().x;
					int tmpPosY= e.localY+pos.y- (*plItr)->getPos().y;
					if( 0<=tmpPosX && tmpPosX<partsSize.x &&
						0<=tmpPosY && tmpPosY<partsSize.y ){

						MouseEvent mouseEvent;
						mouseEvent.globalX= e.globalX;
						mouseEvent.globalY= e.globalY;
						mouseEvent.localX= tmpPosX;
						mouseEvent.localY= tmpPosY;
						mouseEvent.altKey= e.altKey;
						mouseEvent.shiftKey= e.shiftKey;
						mouseEvent.ctrlKey= e.ctrlKey;

						if( lButton ){
							(*plItr)->onLButtonDown(mouseEvent);
						}else{
							(*plItr)->onRButtonDown(mouseEvent);
						}
						return;
					}
				}

			//内部パーツ2
			}else{

				for(std::list<OrgWindowParts*>::iterator plItr=partsList2.begin();
					plItr!=partsList2.end(); plItr++ ){

					WindowSize partsSize= (*plItr)->getSize();
					int tmpPosX= e.localX+pos.x- (*plItr)->getPos().x;
					int tmpPosY= e.localY+pos.y- (*plItr)->getPos().y;
					if( 0<=tmpPosX && tmpPosX<partsSize.x &&
						0<=tmpPosY && tmpPosY<partsSize.y ){

						MouseEvent mouseEvent;
						mouseEvent.globalX= e.globalX;
						mouseEvent.globalY= e.globalY;
						mouseEvent.localX= tmpPosX;
						mouseEvent.localY= tmpPosY;
						mouseEvent.altKey= e.altKey;
						mouseEvent.shiftKey= e.shiftKey;
						mouseEvent.ctrlKey= e.ctrlKey;

						if( lButton ){
							(*plItr)->onLButtonDown(mouseEvent);
						}else{
							(*plItr)->onRButtonDown(mouseEvent);
						}
						return;
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
			for( std::list<OrgWindowParts*>::iterator plItr=partsList1.begin();
				 plItr!=partsList1.end();
				 plItr++ ){

				MouseEvent mouseEvent;
				mouseEvent.globalX= e.globalX;
				mouseEvent.globalY= e.globalY;
				mouseEvent.localX= e.localX+pos.x- (*plItr)->getPos().x;
				mouseEvent.localY= e.localY+pos.y- (*plItr)->getPos().y;
				mouseEvent.altKey= e.altKey;
				mouseEvent.shiftKey= e.shiftKey;
				mouseEvent.ctrlKey= e.ctrlKey;

				if( lButton ){
					(*plItr)->onLButtonUp(mouseEvent);
				}else{
					(*plItr)->onRButtonUp(mouseEvent);
				}
			}
			for( std::list<OrgWindowParts*>::iterator plItr=partsList2.begin();
				 plItr!=partsList2.end();
				 plItr++ ){

				MouseEvent mouseEvent;
				mouseEvent.globalX= e.globalX;
				mouseEvent.globalY= e.globalY;
				mouseEvent.localX= e.localX+pos.x- (*plItr)->getPos().x;
				mouseEvent.localY= e.localY+pos.y- (*plItr)->getPos().y;
				mouseEvent.altKey= e.altKey;
				mouseEvent.shiftKey= e.shiftKey;
				mouseEvent.ctrlKey= e.ctrlKey;

				if( lButton ){
					(*plItr)->onLButtonUp(mouseEvent);
				}else{
					(*plItr)->onRButtonUp(mouseEvent);
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
		OWP_GroupBox(const TCHAR *_name ){
			name= new TCHAR[256];
			_tcscpy_s(name,256,_name);

			openListener = [](){s_dummyfunc();};

			currentPartsSizeY= 0;

			open= true;
			canClose= true;
		}
		~OWP_GroupBox(){
			delete[] name;
		}

		//////////////////////////// Method //////////////////////////////
		///	Method : 親ウィンドウに登録
		void regist( OrgWindow *_parentWindow,
							 WindowPos _pos, WindowSize _size,
							 HDCMaster* _hdcM,
							 unsigned char _baseR=50, unsigned char _baseG=70, unsigned char _baseB=70 ){
			_regist(_parentWindow,_pos,_size,_hdcM,_baseR,_baseG,_baseB);

			//全てのグループ内部品を同じウィンドウに登録
			for(std::list<OrgWindowParts*>::iterator itr=partsList.begin();
				itr!=partsList.end(); itr++){
				(*itr)->regist( parentWindow,
								_pos,_size,
								hdcM,
								baseColor.r,baseColor.g,baseColor.b);
			}

			//グループボックスと内部要素の位置とサイズを自動設定
			autoResize();
		}
		///	Method : グループ内部品を追加
		void addParts(OrgWindowParts& a){
			partsList.push_back(&a);

			// グループボックスがウィンドウに登録されている場合は
			// グループ内部品も同じウィンドウに登録する
			if( parentWindow!=NULL ){
				a.regist( parentWindow,
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
		void autoResize(){
			if( open ){
				//パーツエリアの位置とサイズを設定
				partsAreaPos=  pos+ WindowPos( 3, SIZE_CLOSE_Y+2 );
				partsAreaSize= WindowSize( size.x-3-3, size.y-(SIZE_CLOSE_Y+3)-3 );
				currentPartsSizeY= 0;

				//全ての内部パーツの位置とサイズを自動設定
				for(std::list<OrgWindowParts*>::iterator itr=partsList.begin();
					itr!=partsList.end(); itr++){

					(*itr)->setPos(  WindowPos(  partsAreaPos.x,  partsAreaPos.y+currentPartsSizeY  ) );
					(*itr)->setSize( WindowSize( partsAreaSize.x, partsAreaSize.y-currentPartsSizeY ) );
					(*itr)->autoResize();

					currentPartsSizeY+= (*itr)->getSize().y+1;
				}

				//グループボックスのサイズを内部要素に合わせてトリミング
				partsAreaSize.y= currentPartsSizeY;
				size.y= partsAreaPos.y-pos.y+partsAreaSize.y+3;

			}else{
				size.y= SIZE_CLOSE_Y;
			}
		}
		///	Method : 描画
		void draw(){
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
							name, _tcslen(name), NULL);
				SetBkMode(hdcM->hDC,TRANSPARENT);
			}

			//全ての内部パーツを描画
			if( open ){
				for(std::list<OrgWindowParts*>::iterator itr=partsList.begin();
					itr!=partsList.end(); itr++){
						(*itr)->draw();
				}
			}
		}
		///	Method : 左右マウスボタンダウンイベント受信
		void onLButtonDown(const MouseEvent& e){
			onLRButtonDown(e,true);
		}
		void onRButtonDown(const MouseEvent& e){
			onLRButtonDown(e,false);
		}
		///	Method : 左右マウスボタンアップイベント受信
		void onLButtonUp(const MouseEvent& e){
			onLRButtonUp(e,true);
		}
		void onRButtonUp(const MouseEvent& e){
			onLRButtonUp(e,false);
		}
		///	Method : マウス移動イベント受信
		void onMouseMove(const MouseEvent& e){
			//内部パーツ
			for( std::list<OrgWindowParts*>::iterator plItr=partsList.begin();
				 plItr!=partsList.end();
				 plItr++ ){

				MouseEvent mouseEvent;
				mouseEvent.globalX= e.globalX;
				mouseEvent.globalY= e.globalY;
				mouseEvent.localX= e.localX+pos.x- (*plItr)->getPos().x;
				mouseEvent.localY= e.localY+pos.y- (*plItr)->getPos().y;
				mouseEvent.altKey= e.altKey;
				mouseEvent.shiftKey= e.shiftKey;
				mouseEvent.ctrlKey= e.ctrlKey;

				(*plItr)->onMouseMove(mouseEvent);
			}
		}

		/////////////////////////// Accessor /////////////////////////////
		/// Accessor : name
		const TCHAR* getName() const{
			return name;
		}
		void setName( TCHAR *value ){
			_tcscpy_s(name,256,value);

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
		std::tr1::function<void()> openListener;

		static const int SIZE_CLOSE_Y= 13;
		static const int BOX_POS_X= 3;
		static const int BOX_WIDTH= 10;
		static const int NAME_POS_X1= 5;
		static const int NAME_POS_X2= 3;

		///	Method : 左右マウスボタンダウンイベント受信
		void onLRButtonDown(const MouseEvent& e, bool lButton){

			if( lButton ){
				//ラベルクリックで開閉
				if( e.localY<SIZE_CLOSE_Y && canClose ){
					setOpenStatus(!open);
				}
			}

			//内部パーツ
			for( std::list<OrgWindowParts*>::iterator plItr=partsList.begin();
				 plItr!=partsList.end();
				 plItr++ ){
				WindowSize partsSize= (*plItr)->getSize();
				int tmpPosX= e.localX+pos.x- (*plItr)->getPos().x;
				int tmpPosY= e.localY+pos.y- (*plItr)->getPos().y;
				if( 0<=tmpPosX && tmpPosX<partsSize.x &&
					0<=tmpPosY && tmpPosY<partsSize.y ){

					MouseEvent mouseEvent;
					mouseEvent.globalX= e.globalX;
					mouseEvent.globalY= e.globalY;
					mouseEvent.localX= tmpPosX;
					mouseEvent.localY= tmpPosY;
					mouseEvent.altKey= e.altKey;
					mouseEvent.shiftKey= e.shiftKey;
					mouseEvent.ctrlKey= e.ctrlKey;

					if( lButton ){
						(*plItr)->onLButtonDown(mouseEvent);
					}else{
						(*plItr)->onRButtonDown(mouseEvent);
					}
					return;
				}
			}

		}
		///	Method : 左右マウスボタンアップイベント受信
		void onLRButtonUp(const MouseEvent& e, bool lButton){
			//内部パーツ
			for( std::list<OrgWindowParts*>::iterator plItr=partsList.begin();
				 plItr!=partsList.end();
				 plItr++ ){

				MouseEvent mouseEvent;
				mouseEvent.globalX= e.globalX;
				mouseEvent.globalY= e.globalY;
				mouseEvent.localX= e.localX+pos.x- (*plItr)->getPos().x;
				mouseEvent.localY= e.localY+pos.y- (*plItr)->getPos().y;
				mouseEvent.altKey= e.altKey;
				mouseEvent.shiftKey= e.shiftKey;
				mouseEvent.ctrlKey= e.ctrlKey;

				if( lButton ){
					(*plItr)->onLButtonUp(mouseEvent);
				}else{
					(*plItr)->onRButtonUp(mouseEvent);
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
		OWP_Label(const TCHAR *_name ){
			name= new TCHAR[256];
			_tcscpy_s(name,256,_name);
		}
		~OWP_Label(){
			delete[] name;
		}

		//////////////////////////// Method //////////////////////////////
		/// Method : 自動サイズ設定
		void autoResize(){
			size.y= SIZE_Y;
		}
		//	Method : 描画
		void draw(){
			//名前
			int pos1x= pos.x+NAME_POS_X;
			int pos1y= pos.y+size.y/2-5;
			hdcM->setFont(12,_T("ＭＳ ゴシック"));
			SetTextColor(hdcM->hDC,RGB(240,240,240));
			TextOut( hdcM->hDC,
					 pos1x, pos1y,
					 name, _tcslen(name));
		}
		/// Method : 内容変更
		void setName( TCHAR *value ){
			_tcscpy_s(name,256,value);

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
		OWP_Button( const TCHAR *_name=_T("") ){
			name= new TCHAR[256];
			_tcscpy_s(name,256,_name);


			buttonPush=false;
			buttonListener = [](){s_dummyfunc();};
		}
		~OWP_Button(){
			delete[] name;
		}

		//////////////////////////// Method //////////////////////////////
		/// Method : 自動サイズ設定
		void autoResize(){
			size.y= SIZE_Y;
		}
		//	Method : 描画
		void draw(){
			drawEdge();

			//ボックス
			int pos1x= pos.x+BOX_POS_X;
			int pos1y= pos.y+size.y/2-BOX_WIDTH/2;
			int pos2x= pos.x+BOX_POS_X+BOX_WIDTH-1;
			int pos2y= pos.y+size.y/2+BOX_WIDTH/2-1;
			if(buttonPush){		hdcM->setPenAndBrush(RGB(min(baseColor.r+20,255),min(baseColor.g+20,255),min(baseColor.b+20,255)),NULL);
			}else{				hdcM->setPenAndBrush(RGB(240,240,240),NULL); }
			Rectangle(hdcM->hDC,pos1x,pos1y,pos2x+1,pos2y+1);
			if(!buttonPush){	hdcM->setPenAndBrush(RGB(min(baseColor.r+20,255),min(baseColor.g+20,255),min(baseColor.b+20,255)),NULL);
			}else{				hdcM->setPenAndBrush(RGB(240,240,240),NULL); }
			MoveToEx(hdcM->hDC, pos1x,pos2y, NULL);
			LineTo(hdcM->hDC,   pos2x,pos2y);
			LineTo(hdcM->hDC,   pos2x,pos1y);

			//名前
			pos1x= pos.x+BOX_POS_X+BOX_WIDTH+3;
			pos1y= pos.y+size.y/2-5;
			hdcM->setFont(12,_T("ＭＳ ゴシック"));
			SetTextColor(hdcM->hDC,RGB(240,240,240));
			TextOut( hdcM->hDC,
					 pos1x, pos1y,
					 name, _tcslen(name));
		}
		//	Method : マウスダウンイベント受信
		void onLButtonDown(const MouseEvent& e){
			if(this->buttonListener!=NULL){
				(this->buttonListener)();
			}

			buttonPush=true;

			RECT tmpRect;
			tmpRect.left=   pos.x+1;
			tmpRect.top=    pos.y+1;
			tmpRect.right=  pos.x+size.x-1;
			tmpRect.bottom= pos.y+size.y-1;
			InvalidateRect( parentWindow->getHWnd(), &tmpRect, false );


			//ボタンアップアニメーションのためのスレッド作成
			_beginthread(drawButtonUpThread,0,(void *)this);
		}

		/////////////////////////// Accessor /////////////////////////////
		//	Accessor : buttonListener
		void setButtonListener(std::tr1::function<void()> listener){
			this->buttonListener= listener;
		}
		void setName(TCHAR *value){
			_tcscpy_s(name, 256, value);

			callRewrite();
		}
	private:
		////////////////////////// MemberVar /////////////////////////////
		TCHAR *name;

		bool buttonPush;
		std::tr1::function<void()> buttonListener;

		static const int SIZE_Y= 15;
		static const int BOX_POS_X= 3;
		static const int BOX_WIDTH= 10;

		//////////////////////////// Method //////////////////////////////
		//	Method : ボタンアップのスレッド
		static void drawButtonUpThread(LPVOID	pParam){
			Sleep(100);

			OWP_Button *thisClass= (OWP_Button*)pParam;

			thisClass->buttonPush=false;

			RECT tmpRect;
			tmpRect.left=   thisClass->pos.x+1;
			tmpRect.top=    thisClass->pos.y+1;
			tmpRect.right=  thisClass->pos.x+ thisClass->size.x-1;
			tmpRect.bottom= thisClass->pos.y+ thisClass->size.y-1;
			InvalidateRect( thisClass->parentWindow->getHWnd(), &tmpRect, false );
		}
	};

	///<summary>
	///	ウィンドウ内部品"プレイヤーボタン"クラス
	///</summary>
	class OWP_PlayerButton : public OrgWindowParts{
	public:
		//////////////////// Constructor/Destructor //////////////////////
		OWP_PlayerButton(){
			BOX_WIDTH= 11;
			SIZE_Y= 15;
		}
		~OWP_PlayerButton(){
		}

		//////////////////////////// Method //////////////////////////////
		/// Method : 自動サイズ設定
		void autoResize(){
			size.y= SIZE_Y;
		}
		//	Method : 描画
		void draw(){
			drawEdge();

			//全てのボタンについて繰り返す
			for(int i=0; i<6; i++){

				//ボタンの四隅になる座標を求める
				int pos1x= pos.x+BOX_POS_X+BOX_WIDTH*i;
				int pos1y= pos.y+size.y/2-BOX_WIDTH/2;
				int pos2x= pos.x+BOX_POS_X+BOX_WIDTH*(i+1)-1;
				int pos2y= pos1y+BOX_WIDTH-1;

				//ボタンパラメータのインスタンスへのポインタを作成
				OneButtonParam *btnPrm;
				switch(i){
				case 0: btnPrm= &reset; break;
				case 1: btnPrm= &backStep; break;
				case 2: btnPrm= &backPlay; break;
				case 3: btnPrm= &stop; break;
				case 4: btnPrm= &frontPlay; break;
				case 5: btnPrm= &frontStep; break;
				}

				//枠組み描画
				if(btnPrm->buttonPush){	hdcM->setPenAndBrush(RGB(min(baseColor.r+20,255),min(baseColor.g+20,255),min(baseColor.b+20,255)),NULL);
				}else{					hdcM->setPenAndBrush(RGB(240,240,240),NULL); }
				Rectangle(hdcM->hDC,pos1x,pos1y,pos2x+1,pos2y+1);
				if(!btnPrm->buttonPush){hdcM->setPenAndBrush(RGB(min(baseColor.r+20,255),min(baseColor.g+20,255),min(baseColor.b+20,255)),NULL);
				}else{					hdcM->setPenAndBrush(RGB(240,240,240),NULL); }
				MoveToEx(hdcM->hDC, pos1x,pos2y, NULL);
				LineTo(hdcM->hDC,   pos2x,pos2y);
				LineTo(hdcM->hDC,   pos2x,pos1y);

				//中身描画
				int shiftDot= btnPrm->buttonPush?1:0;
				hdcM->setPenAndBrush(RGB(240,240,240),RGB(240,240,240));
				switch(i){
				case 0:		//リセットボタン
					Rectangle( hdcM->hDC,
						pos1x+4+shiftDot, pos1y+4+shiftDot,
						pos2x-3+shiftDot, pos2y-3+shiftDot );
					break;
				case 1:		//一つ戻るボタン
					{
						int x1= (pos1x+pos2x)/2+2;
						int x2= pos2x-2;
						Rectangle( hdcM->hDC, x1+shiftDot,pos1y+3+shiftDot, x2+shiftDot,pos2y-2+shiftDot );

						int y1= pos1y+3;
						int y2= pos2y-2;
						int x4= (pos1x+pos2x)/2;
						int x3= x4-(y2-y1)/2;
						for(int j=0; j<=x4-x3; j++){
							MoveToEx(hdcM->hDC, x4-j+shiftDot,y1+j+shiftDot, NULL);
							LineTo(hdcM->hDC,   x4-j+shiftDot,y2-j+shiftDot);
						}
					}break;
				case 2:		//逆再生ボタン
					{
						int y1= pos1y+3;
						int y2= pos2y-2;
						int x1= (pos1x+pos2x)/2-(y2-y1)/4;
						int x2= x1+(y2-y1)/2;
						for(int j=0; j<=x2-x1; j++){
							MoveToEx(hdcM->hDC, x2-j+shiftDot,y1+j+shiftDot, NULL);
							LineTo(hdcM->hDC,   x2-j+shiftDot,y2-j+shiftDot);
						}
					}break;
				case 3:		//停止ボタン
					{
						int x1= pos1x+3;
						int x2= (pos1x+pos2x)/2;
						int x4= pos2x-2;
						int x3= x4-(x2-x1);
						Rectangle( hdcM->hDC, x1+shiftDot,pos1y+4+shiftDot, x2+shiftDot,pos2y-3+shiftDot );
						Rectangle( hdcM->hDC, x3+shiftDot,pos1y+4+shiftDot, x4+shiftDot,pos2y-3+shiftDot );
					}break;
				case 4:		//再生ボタン
					{
						int y1= pos1y+3;
						int y2= pos2y-2;
						int x1= (pos1x+pos2x)/2-(y2-y1)/4;
						int x2= x1+(y2-y1)/2;
						for(int j=0; j<=x2-x1; j++){
							MoveToEx(hdcM->hDC, x1+j+shiftDot,y1+j+shiftDot, NULL);
							LineTo(hdcM->hDC,   x1+j+shiftDot,y2-j+shiftDot);
						}
					}break;
				case 5:		//一つ進むボタン
					{
						int x1= pos1x+3;
						int x2= (pos1x+pos2x)/2-1;
						Rectangle( hdcM->hDC, x1+shiftDot,pos1y+3+shiftDot, x2+shiftDot,pos2y-2+shiftDot );

						int y1= pos1y+3;
						int y2= pos2y-2;
						int x3= (pos1x+pos2x)/2;
						int x4= x3+(y2-y1)/2;
						for(int j=0; j<=x4-x3; j++){
							MoveToEx(hdcM->hDC, x3+j+shiftDot,y1+j+shiftDot, NULL);
							LineTo(hdcM->hDC,   x3+j+shiftDot,y2-j+shiftDot);
						}
					}break;
				}

			}

		}
		//	Method : マウスダウンイベント受信
		void onLButtonDown(const MouseEvent& e){

			//全てのボタンについて繰り返す
			for(int i=0; i<6; i++){

				//まずボタンが押されたかを確認
				if( BOX_POS_X+BOX_WIDTH*i<=e.localX && e.localX<BOX_POS_X+BOX_WIDTH*(i+1) ){
				}else{
					continue;
				}

				//ボタンパラメータのインスタンスへのポインタを作成
				OneButtonParam *btnPrm;
				switch(i){
				case 0: btnPrm= &reset; break;
				case 1: btnPrm= &backStep; break;
				case 2: btnPrm= &backPlay; break;
				case 3: btnPrm= &stop; break;
				case 4: btnPrm= &frontPlay; break;
				case 5: btnPrm= &frontStep; break;
				}

				//ボタンリスナーを呼ぶ
				if( btnPrm->buttonListener!=NULL){
					btnPrm->buttonListener();
				}
				
				//ボタン押下状態をONにする
				btnPrm->buttonPush=true;

				//再描画通知
				callRewrite();

				//ボタンアップアニメーションのためのスレッド作成
				switch(i){
				case 0: _beginthread(drawResetButtonUpThread,0,(void *)this); break;
				case 1: _beginthread(drawBackStepButtonUpThread,0,(void *)this); break;
				case 2: _beginthread(drawBackPlayButtonUpThread,0,(void *)this); break;
				case 3: _beginthread(drawStopButtonUpThread,0,(void *)this); break;
				case 4: _beginthread(drawFrontPlayButtonUpThread,0,(void *)this); break;
				case 5: _beginthread(drawFrontStepButtonUpThread,0,(void *)this); break;
				}

				return;
			}
		}

		/////////////////////////// Accessor /////////////////////////////
		//	Accessor : buttonListener
		void setFrontPlayButtonListener(std::tr1::function<void()> listener){
			frontPlay.buttonListener= listener;
		}
		void setBackPlayButtonListener(std::tr1::function<void()> listener){
			backPlay.buttonListener= listener;
		}
		void setStopButtonListener(std::tr1::function<void()> listener){
			stop.buttonListener= listener;
		}
		void setResetButtonListener(std::tr1::function<void()> listener){
			reset.buttonListener= listener;
		}
		void setFrontStepButtonListener(std::tr1::function<void()> listener){
			frontStep.buttonListener= listener;
		}
		void setBackStepButtonListener(std::tr1::function<void()> listener){
			backStep.buttonListener= listener;
		}

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
				buttonListener = [](){s_dummyfunc();};
			}

			bool buttonPush;
			std::tr1::function<void()> buttonListener;
		}frontPlay,backPlay,stop,reset,frontStep,backStep;
		

		int SIZE_Y;
		static const int BOX_POS_X= 3;
		int BOX_WIDTH;

		//////////////////////////// Method //////////////////////////////
		//	Method : ボタンアップのスレッド
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
		static void drawStopButtonUpThread(LPVOID	pParam){
			Sleep(100);

			OWP_PlayerButton *thisClass= (OWP_PlayerButton*)pParam;
			thisClass->stop.buttonPush=false;
			thisClass->callRewrite();
		}
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
	};

	///<summary>
	///	ウィンドウ内部品"チェックボックス"クラス
	///</summary>
	class OWP_CheckBox : public OrgWindowParts{
	public:
		//////////////////// Constructor/Destructor //////////////////////
		OWP_CheckBox( const TCHAR *_name=_T(""), bool _value=false ){
			name= new TCHAR[256];
			_tcscpy_s(name,256,_name);

			value= _value;

			buttonListener = [](){s_dummyfunc();};
		}
		~OWP_CheckBox(){
			delete[] name;
		}

		//////////////////////////// Method //////////////////////////////
		/// Method : 自動サイズ設定
		void autoResize(){
			size.y= SIZE_Y;
		}
		//	Method : 描画
		void draw(){
			drawEdge();

			//チェックボックス
			int pos1x= pos.x+BOX_POS_X;
			int pos1y= pos.y+size.y/2-BOX_WIDTH/2;
			int pos2x= pos.x+BOX_POS_X+BOX_WIDTH-1;
			int pos2y= pos.y+size.y/2+BOX_WIDTH/2-1;
			hdcM->setPenAndBrush(RGB(240,240,240),NULL);
			Rectangle(hdcM->hDC,pos1x,pos1y,pos2x+1,pos2y+1);
			if( value ){
				MoveToEx(hdcM->hDC, pos1x+2,pos1y+2, NULL);
				LineTo(hdcM->hDC,   pos2x-1,pos2y-1);
				MoveToEx(hdcM->hDC, pos2x-2,pos1y+2, NULL);
				LineTo(hdcM->hDC,   pos1x+1,pos2y-1);
			}
			hdcM->setPenAndBrush(RGB(min(baseColor.r+20,255),min(baseColor.g+20,255),min(baseColor.b+20,255)),NULL);
			MoveToEx(hdcM->hDC, pos1x,pos1y+2, NULL);
			LineTo(hdcM->hDC,   pos1x,pos2y-1);
			MoveToEx(hdcM->hDC, pos2x,pos1y+2, NULL);
			LineTo(hdcM->hDC,   pos2x,pos2y-1);
			MoveToEx(hdcM->hDC, pos1x+2,pos1y, NULL);
			LineTo(hdcM->hDC,   pos2x-1,pos1y);
			MoveToEx(hdcM->hDC, pos1x+2,pos2y, NULL);
			LineTo(hdcM->hDC,   pos2x-1,pos2y);

			//名前
			pos1x= pos.x+BOX_POS_X+BOX_WIDTH+3;
			pos1y= pos.y+size.y/2-5;
			hdcM->setFont(12,_T("ＭＳ ゴシック"));
			SetTextColor(hdcM->hDC,RGB(240,240,240));
			TextOut( hdcM->hDC,
					 pos1x, pos1y,
					 name, _tcslen(name));
		}
		//	Method : マウスダウンイベント受信
		void onLButtonDown(const MouseEvent& e){
			setValue(value^true);
		}

		/////////////////////////// Accessor /////////////////////////////
		//	Accessor : value
		void setValue(bool _value){
			value= _value;

			RECT tmpRect;
			tmpRect.left=   pos.x+1;
			tmpRect.top=    pos.y+1;
			tmpRect.right=  pos.x+size.x-1;
			tmpRect.bottom= pos.y+size.y-1;
			InvalidateRect( parentWindow->getHWnd(), &tmpRect, false );

			//リスナーコール
			if(this->buttonListener!=NULL){
				(this->buttonListener)();
			}
		}
		bool getValue() const{
			return value;
		}
		//	Accessor : buttonListener
		void setButtonListener(std::tr1::function<void()> listener){
			this->buttonListener= listener;
		}

	private:
		////////////////////////// MemberVar /////////////////////////////
		TCHAR *name;
		bool value;
		std::tr1::function<void()> buttonListener;

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
		OWP_RadioButton( const TCHAR *name ){
			nameList.push_back(name);
			selectIndex= 0;

			selectListener = [](){s_dummyfunc();};
		}
		~OWP_RadioButton(){
		}

		//////////////////////////// Method //////////////////////////////
		/// Method : 自動サイズ設定
		void autoResize(){
			size.y= SIZE_Y*nameList.size()+4;
		}
		///	Method : 描画
		void draw(){
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
						 nameList[i].c_str(), _tcslen(nameList[i].c_str()));
			}
		}
		///	Method : マウスダウンイベント受信
		void onLButtonDown(const MouseEvent& e){
			int targetIndex= (e.localY-2)/SIZE_Y;
			if( 0<=targetIndex && targetIndex<(int)nameList.size() ){
				setSelectIndex(targetIndex);
			}
		}
		/// Method : 項目の追加
		void addLine( const TCHAR *name ){
			nameList.push_back(name);

			//ウィンドウ内の全パーツの位置とサイズを自動設定
			if( parentWindow!=NULL ){
				parentWindow->autoResizeAllParts();
			}
		}
		/// Method : 項目の削除
		bool deleteLine(){

			//2つ以上の項目がある場合は最後の項目を削除
			if( nameList.size()!=1 ){
				nameList.pop_back();

				//消した項目を選択していた場合は最後の項目を選択
				if( selectIndex==nameList.size() ){
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
			if( nameList.size()!=1
			 && 0<=index && index<(int)nameList.size() ){
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
			if( nameList.size()!=1 ){
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
		void setSelectListener(std::tr1::function<void()> listener){
			selectListener= listener;
		}

	private:
		////////////////////////// MemberVar /////////////////////////////
		TCHAR *name;
		int selectIndex;
		std::tr1::function<void()> selectListener;

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
		OWP_Slider( double _value=0.0, double _maxValue=1.0, double _minValue=0.0 ){
			maxValue= max(_minValue,_maxValue);
			minValue= min(_minValue,_maxValue);
			value= max(min(_value,maxValue),minValue);

			drag=false;
		}

		//////////////////////////// Method //////////////////////////////
		/// Method : 自動サイズ設定
		void autoResize(){
			size.y= SIZE_Y;
		}
		//	Method : 描画
		void draw(){
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

			//つまみ
			int tumamiPosX= pos1x+ (int)( (value-minValue)*(float)(pos2x-pos1x)/(maxValue-minValue)+ 0.5f );
			pos1x= tumamiPosX-1;
			pos1y= pos1y-AXIS_SIZE_Y/2;
			pos2x= tumamiPosX+1;
			pos2y= pos1y+AXIS_SIZE_Y;
			hdcM->setPenAndBrush(RGB(240,240,240),RGB(baseColor.r,baseColor.g,baseColor.b));
			Rectangle(hdcM->hDC,pos1x,pos1y,pos2x+1,pos2y+1);
			hdcM->setPenAndBrush(RGB(min(baseColor.r+20,255),min(baseColor.g+20,255),min(baseColor.b+20,255)),NULL);
			MoveToEx(hdcM->hDC, pos1x,pos1y+2, NULL);
			LineTo(hdcM->hDC,   pos1x,pos2y-1);
			MoveToEx(hdcM->hDC, pos2x,pos1y+2, NULL);
			LineTo(hdcM->hDC,   pos2x,pos2y-1);

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
					 tmpChar, _tcslen(tmpChar));
		}
		//	Method : マウスダウンイベント受信
		void onLButtonDown(const MouseEvent& e){
			WindowPos tmpPos= WindowPos(e.localX,e.localY)- WindowPos(AXIS_POS_X, size.y/2);
			const int EDGE_WIDTH= 4;
			if( -EDGE_WIDTH<=tmpPos.x && tmpPos.x<=size.x-LABEL_SIZE_X+EDGE_WIDTH &&
				-EDGE_WIDTH-AXIS_SIZE_Y/2<=tmpPos.y && tmpPos.y<=size.y+EDGE_WIDTH+AXIS_SIZE_Y/2 ){
				setValue( minValue+ (maxValue-minValue)* (float)tmpPos.x/(float)(size.x-AXIS_POS_X-LABEL_SIZE_X) );

				drag=true;

				RECT tmpRect;
				tmpRect.left=   pos.x+1;
				tmpRect.top=    pos.y+1;
				tmpRect.right=  pos.x+size.x-1;
				tmpRect.bottom= pos.y+size.y-1;
				InvalidateRect( parentWindow->getHWnd(), &tmpRect, false );

			}
		}
		//	Method : 左マウスボタンアップイベント受信
		void onLButtonUp(const MouseEvent& e){
			drag=false;
		}
		//	Method : マウス移動イベント受信
		void onMouseMove(const MouseEvent& e){
			WindowPos tmpPos= WindowPos(e.localX,e.localY)- WindowPos(AXIS_POS_X, size.y/2);
			if( drag ){
				setValue( minValue+ (maxValue-minValue)* (float)tmpPos.x/(float)(size.x-AXIS_POS_X-LABEL_SIZE_X) );

				RECT tmpRect;
				tmpRect.left=   pos.x+1;
				tmpRect.top=    pos.y+1;
				tmpRect.right=  pos.x+size.x-1;
				tmpRect.bottom= pos.y+size.y-1;
				InvalidateRect( parentWindow->getHWnd(), &tmpRect, false );

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
		void setValue(double _value){
			value= min(max(_value,minValue),maxValue);

			//リスナーコール
			if(this->cursorListener!=NULL){
				(this->cursorListener)();
			}
		}
		//	Accessor : cursorListener
		void setCursorListener(std::tr1::function<void()> listener){
			this->cursorListener= listener;
		}

	private:
		////////////////////////// MemberVar /////////////////////////////
		double maxValue,minValue,value;
		std::tr1::function<void()> cursorListener;

		bool drag;

		static const int SIZE_Y= 20;
		static const int LABEL_SIZE_X= 65;
		static const int AXIS_POS_X= 5;
		static const int AXIS_SIZE_Y= 10;
	};

	///<summary>
	///	ウィンドウ内部品"タイムラインテーブル"クラス
	///</summary>
	class OWP_Timeline : public OrgWindowParts{
	public:
		//////////////////// Constructor/Destructor //////////////////////
		OWP_Timeline(const std::basic_string<TCHAR> &_name=_T(""), const double &_maxTime=1.0, const double &_timeSize=8.0 ){
			lineData.push_back(new LineData(0,_name,this,0));
			maxTime= _maxTime;
			timeSize= _timeSize;
			cursorListener = [](){s_dummyfunc();};
			selectListener = [](){s_dummyfunc();};
			keyShiftListener= [this](){
				shiftKeyTime(getShiftKeyTime());
			};
			keyDeleteListener = [](const KeyInfo& dummy){s_dummyfunc();};

			showPos_time=0;
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
		}
		~OWP_Timeline(){
			selectAll(true);
			deleteKey();

			for(std::vector<LineData*>::iterator it=lineData.begin();
				it!=lineData.end();
				it++){
				delete (*it);
			}
		}

		//////////////////////////// Method //////////////////////////////
		/// Method : 自動サイズ設定
		void autoResize(){
			size.y-= (size.y-SCROLL_BAR_WIDTH-AXIS_SIZE_Y-MARGIN*2)%(LABEL_SIZE_Y-1);
		}
		//	Method : 描画
		void draw(){
			drawEdge();

			//時間軸目盛り
			{
				const int AXIS_CURSOR_SIZE=4;
				const int AXIS_LABEL_SIDE_MARGIN=7;

				//目盛り線 & ラベル
				int x0= pos.x+MARGIN+LABEL_SIZE_X;
				int x1= pos.x+size.x-MARGIN-SCROLL_BAR_WIDTH;
				int y0= pos.y+MARGIN;
				int y1= y0+AXIS_SIZE_Y+1;
				for(int i=(int)showPos_time; i<=(int)maxTime; i++){
					int xx= (int)(((double)i-showPos_time)*timeSize) + x0 + 1;

					if( x1+AXIS_LABEL_SIDE_MARGIN <= xx ) break;
					if( x0-AXIS_LABEL_SIDE_MARGIN <= xx ){
						hdcM->setPenAndBrush(RGB(min(baseColor.r+20,255),min(baseColor.g+20,255),min(baseColor.b+20,255)),NULL);
						MoveToEx(hdcM->hDC, xx,y1-5, NULL);
						LineTo(hdcM->hDC,   xx,y1);

						if( i%5==0 ){
							TCHAR tmpChar[20];
							_stprintf_s(tmpChar,20,_T("%.3G"),(double)i);
							hdcM->setFont(12,_T("ＭＳ ゴシック"));
							SetTextColor(hdcM->hDC,RGB(240,240,240));
							TextOut( hdcM->hDC,
									 xx-(int)((double)_tcslen(tmpChar)*2.0), y0,
									 tmpChar, _tcslen(tmpChar));
						}
					}
				}

				//カーソル
				int xx= (int)((currentTime-showPos_time)*timeSize) + x0 + 1;
				hdcM->setPenAndBrush(RGB(240,240,240),NULL);
				if( x0-AXIS_CURSOR_SIZE <= xx && xx <= x1+AXIS_CURSOR_SIZE ){
					for(int i=0; i<AXIS_CURSOR_SIZE; i++){
						MoveToEx(hdcM->hDC, xx-i,y1-i-2, NULL);
						LineTo(hdcM->hDC,   xx+i+1,y1-i-2);
					}
				}

				//枠
				hdcM->setPenAndBrush(NULL,RGB(baseColor.r,baseColor.g,baseColor.b));
				Rectangle(hdcM->hDC,pos.x,y0,x0,y1);
				Rectangle(hdcM->hDC,x1,y0,pos.x+size.x,y1);
				hdcM->setPenAndBrush(RGB(min(baseColor.r+20,255),min(baseColor.g+20,255),min(baseColor.b+20,255)),NULL);
				Rectangle(hdcM->hDC,x0,y0,x1,y1);
			}
			
			drawEdge(false);

			//行データ
			int showLineNum= (size.y-SCROLL_BAR_WIDTH-AXIS_SIZE_Y-MARGIN*2)/(LABEL_SIZE_Y-1);
			for(int i=showPos_line,j=0; i<(int)lineData.size() && j<showLineNum; i++,j++){
				bool highLight=false;
				if( i==currentLine ) highLight=true;
				if( i >= 0 ){
					lineData[i]->draw(	hdcM,
										pos.x+MARGIN,
										pos.y+MARGIN+AXIS_SIZE_Y+j*(LABEL_SIZE_Y-1),
										size.x-SCROLL_BAR_WIDTH-MARGIN*2,
										timeSize,showPos_time,highLight );
				}
			}

			//ドラッグによる選択範囲
			if( dragSelect && dragSelectTime1!=dragSelectTime2 ){
				int xx0= pos.x+MARGIN+LABEL_SIZE_X+ 1;
				int yy0= pos.y+MARGIN+AXIS_SIZE_Y;
				int xx1= pos.x+size.x-MARGIN-SCROLL_BAR_WIDTH- 1;
				int yy1= pos.y+size.y-MARGIN-SCROLL_BAR_WIDTH;
				int x0= xx0+ (int)((min(dragSelectTime1,dragSelectTime2)- showPos_time)* timeSize);
				int x1= xx0+ (int)((max(dragSelectTime1,dragSelectTime2)- showPos_time)* timeSize);
				int y0= yy0+ (min(dragSelectLine1,dragSelectLine2)- showPos_line)* (LABEL_SIZE_Y-1)+ 1;
				int y1= yy0+ (max(dragSelectLine1,dragSelectLine2)- showPos_line+ 1)* (LABEL_SIZE_Y-1)- 1;

				{//枠描画
					hdcM->setPenAndBrush(RGB(min(baseColor.r+20,255),min(baseColor.g+20,255),min(baseColor.b+20,255)),NULL);
					if( xx0<=x0 ){		//左枠
						MoveToEx(hdcM->hDC, x0, max(yy0,y0+1), NULL);
						LineTo(hdcM->hDC,   x0, min(y1,yy1));
					}
					if( x1<=xx1 ){		//右枠
						MoveToEx(hdcM->hDC, x1, max(yy0,y0+1), NULL);
						LineTo(hdcM->hDC,   x1, min(y1,yy1));
					}
					if( yy0<=y0 ){		//上枠
						MoveToEx(hdcM->hDC, max(xx0,x0+1), y0, NULL);
						LineTo(hdcM->hDC,   min(x1,xx1), y0);
					}
					if( y1<=yy1 ){		//下枠
						MoveToEx(hdcM->hDC, max(xx0,x0+1), y1, NULL);
						LineTo(hdcM->hDC,   min(x1,xx1), y1);
					}
				}
			}

			//時間軸スクロールバー
			{
				int x0= pos.x+MARGIN+LABEL_SIZE_X;
				int x1= pos.x+size.x-MARGIN-SCROLL_BAR_WIDTH;
				int y0= pos.y+size.y-MARGIN-SCROLL_BAR_WIDTH;
				int y1= y0+SCROLL_BAR_WIDTH;

				//枠
				hdcM->setPenAndBrush(RGB(min(baseColor.r+20,255),min(baseColor.g+20,255),min(baseColor.b+20,255)),NULL);
				Rectangle(hdcM->hDC,x0,y0,x1,y1);

				//中身
				double showTimeLength= ((double)(x1-x0-3))/timeSize;
				double barSize= ((double)(x1-x0-4))*showTimeLength/maxTime;
				double barStart= ((double)(x1-x0-4))*showPos_time/maxTime;
				if( showTimeLength<maxTime ){
					hdcM->setPenAndBrush(NULL,RGB(min(baseColor.r+20,255),min(baseColor.g+20,255),min(baseColor.b+20,255)));
					Rectangle(hdcM->hDC,x0+2+(int)barStart,y0+2,x0+2+(int)(barStart+barSize),y1-2);
				}
			}

			//ラベルスクロールバー
			{
				int x0= pos.x+size.x-MARGIN-SCROLL_BAR_WIDTH-1;
				int x1= x0+SCROLL_BAR_WIDTH+1;
				int y0= pos.y+MARGIN+AXIS_SIZE_Y;
				int y1= pos.y+size.y-MARGIN-SCROLL_BAR_WIDTH+1;

				//枠
				hdcM->setPenAndBrush(RGB(min(baseColor.r+20,255),min(baseColor.g+20,255),min(baseColor.b+20,255)),NULL);
				Rectangle(hdcM->hDC,x0,y0,x1,y1);

				//中身
				int barSize= (y1-y0-4)*showLineNum/(int)lineData.size();
				int barStart= (y1-y0-4)*showPos_line/(int)lineData.size();
				if( showLineNum<(int)lineData.size() ){
					hdcM->setPenAndBrush(NULL,RGB(min(baseColor.r+20,255),min(baseColor.g+20,255),min(baseColor.b+20,255)));
					Rectangle(hdcM->hDC,x0+2,y0+2+barStart,x1-2,y0+2+barStart+barSize+1);
				}
			}
		}
		//	Method : 行を追加	(既に同名のキーがある場合はFalseを返す)
		bool newLine(int nullflag, const std::basic_string<TCHAR>& _name ){
			for(int i=0; i<(int)lineData.size(); i++){
				if(lineData[i]->name==_name){
					return false;
				}
			}
			lineData.push_back(new LineData(nullflag, _name,this,lineData.size()));

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
			if(lineData.size()<=(unsigned int)index) return false;

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
		//	Method : キーを削除
		bool deleteKey(const std::basic_string<TCHAR>& _name, double time){
			for(int i=0; i<(int)lineData.size(); i++){
				if(lineData[i]->name==_name){
					selectClear();
					bool ret= lineData[i]->selectKey(time);
					lineData[i]->deleteKey();

					//再描画要求
					if( ret && rewriteOnChange ){
						callRewrite();
					}
					return ret;
				}
			}
			return false;
		}
		bool deleteKey(const std::basic_string<TCHAR>& _name, int index){
			for(int i=0; i<(int)lineData.size(); i++){
				if(lineData[i]->name==_name){
					bool ret= lineData[i]->deleteKey(index);

					//再描画要求
					if( ret && rewriteOnChange ){
						callRewrite();
					}
					return ret;
				}
			}
			return false;
		}
		bool deleteKey(int lineIndex, double time){
			if(lineData.size()<=(unsigned int)lineIndex) return false;

			selectClear();
			bool ret= lineData[lineIndex]->selectKey(time);
			lineData[lineIndex]->deleteKey();

			//再描画要求
			if( ret && rewriteOnChange ){
				callRewrite();
			}

			return ret;
		}
		bool deleteKey(int lineIndex, int keyIndex){
			if(lineData.size()<=(unsigned int)lineIndex) return false;

			bool ret= lineData[lineIndex]->deleteKey(keyIndex);

			//再描画要求
			if( ret && rewriteOnChange ){
				callRewrite();
			}

			return ret;
		}
		unsigned int deleteKey(){
			unsigned int deleteNum= 0;

			for(int i=0; i<(int)lineData.size(); i++){
				deleteNum+= lineData[i]->deleteKey();
			}

			//再描画要求
			if( deleteNum!=0 && rewriteOnChange ){
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
			if(!noCallListener && this->selectListener!=NULL){
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
			if(!noCallListener && this->selectListener!=NULL){
				(this->selectListener)();
			}

			//再描画要求
			if( rewriteOnChange ){
				callRewrite();
			}
		}
		/// Method : すべての選択されているキーを取得する
		std::list<KeyInfo> getSelectedKey() const{
			std::list<KeyInfo> ret;

			for(int i=0; i<(int)lineData.size(); i++){
				LineData *curLineData= lineData[i];

				for(int j=0; j<(int)curLineData->key.size(); j++){
					LineData::Key *curKey= lineData[i]->key[j];

					if( curKey->select ){
						KeyInfo tmp;
						tmp.label= curLineData->name.c_str();
						tmp.time= curKey->time;
						tmp.lineIndex= i;
						tmp.timeIndex= j;
						tmp.object= curKey->object;
						ret.push_back(tmp);
					}
				}
			}

			return ret;
		}
		/// Method : 全ての選択されているキーを移動する
		void shiftKeyTime(const double &shiftTime){
			for(int i=0; i<(int)lineData.size(); i++){
				lineData[i]->shiftKey(shiftTime);
			}

			//再描画要求
			if( rewriteOnChange ){
				callRewrite();
			}
		}
		///	Method : マウスダウンイベント受信
		void onLButtonDown(const MouseEvent& e){
			if( !canMouseControll ) return;

			int x0= MARGIN;
			int x1= x0+LABEL_SIZE_X;
			int x2= size.x-MARGIN-SCROLL_BAR_WIDTH;
			int x3= size.x-MARGIN;
			int y0= MARGIN;
			int y1= y0+AXIS_SIZE_Y+1;
			int y2= size.y-MARGIN-SCROLL_BAR_WIDTH;
			int y3= size.y-MARGIN;

			//ラベル
			if( x0<=e.localX && e.localX<x2
			 && y1<=e.localY && e.localY<y2 ){
				setCurrentLine( showPos_line+ (e.localY-y1)/(LABEL_SIZE_Y-1) );

				dragLabel=true;
			}

			//時間軸目盛り
			if( x1-2<=e.localX && e.localX<x2
			 && y0<=e.localY && e.localY<y2 ){
				setCurrentTime( showPos_time+ (double)(e.localX-x1)/timeSize );

				dragTime=true;
			}

			{//ドラッグでの範囲選択
				dragSelectTime1= currentTime;
				dragSelectLine1= currentLine;
			}

			//時間軸スクロールバー
			if( x1<=e.localX && e.localX<x2
			 && y2<=e.localY && e.localY<y3 ){
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
			if( x2<=e.localX && e.localX<x3
			 && y1<=e.localY && e.localY<y2 ){
				int yy0= MARGIN+AXIS_SIZE_Y;
				int yy1= size.y-MARGIN-SCROLL_BAR_WIDTH+1;

				int showLineNum= (size.y-SCROLL_BAR_WIDTH-AXIS_SIZE_Y-MARGIN*2)/(LABEL_SIZE_Y-1);
				if( showLineNum<(int)lineData.size() ){
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
		///	Method : 左マウスボタンアップイベント受信
		void onLButtonUp(const MouseEvent& e){
			if( !canMouseControll ) return;

			//ドラッグ選択範囲内のキーを選択状態にする
			if( !dragShift && !dragScrollBarLabel && !dragScrollBarTime ){
				selectClear(true);
				if( dragSelect ){
					for(int i=min(dragSelectLine1,dragSelectLine2);
						i<=max(dragSelectLine1,dragSelectLine2) && i<(signed int)lineData.size(); i++){
						lineData[i]->selectKey( min(dragSelectTime1,dragSelectTime2),
												max(dragSelectTime1,dragSelectTime2) );
					}
				}
				//リスナーコール
				if(this->selectListener!=NULL){
					(this->selectListener)();
				}
			}

			//Ctrl+ドラッグによるキー移動
			if( dragShift ){
				//リスナーコール
				if(this->keyShiftListener!=NULL){
					(this->keyShiftListener)();
				}
				ghostShiftTime= 0.0;
			}

			//ドラッグフラグを初期化
			dragLabel= false;
			dragTime= false;
			dragScrollBarLabel= false;
			dragScrollBarTime= false;
			dragSelect= false;
			dragShift= false;

			//再描画領域
			RECT tmpRect;
			tmpRect.left=   pos.x+1;
			tmpRect.top=    pos.y+1;
			tmpRect.right=  pos.x+size.x-1;
			tmpRect.bottom= pos.y+size.y-1;
			InvalidateRect( parentWindow->getHWnd(), &tmpRect, false );

		}
		///	Method : マウス移動イベント受信
		void onMouseMove(const MouseEvent& e){
			if( !canMouseControll ) return;

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
		void setCurrentTime(double _currentTime, bool noCallListener=false){
			int x1= MARGIN+LABEL_SIZE_X;
			int x2= size.x-MARGIN-SCROLL_BAR_WIDTH;

			if( timeSnapSize!=0 ){
				_currentTime= floor(_currentTime/timeSnapSize+0.5)*timeSnapSize;
			}

			currentTime= min(max(_currentTime,0),maxTime);

			if( currentTime <= showPos_time ){
				showPos_time= currentTime;
			}
			if( showPos_time+((double)(x2-3-x1))/timeSize <= currentTime ){
				showPos_time= currentTime-((double)(x2-3-x1))/timeSize;
			}

			//リスナーコール
			if( !noCallListener && this->cursorListener!=NULL ){
				(this->cursorListener)();
			}

			//再描画要求
			if( rewriteOnChange ){
				callRewrite();
			}
		}
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
			if( showPos_line+showLineNum-1 <= currentLine ){
				showPos_line= currentLine-showLineNum+1;
			}

			//リスナーコール
			if( !noCallListener && this->cursorListener!=NULL ){
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
		void setShowPosTime(const double &_showPosTime){
			int x0= MARGIN+LABEL_SIZE_X;
			int x1= size.x-MARGIN-SCROLL_BAR_WIDTH;

			double showTimeLength= ((double)(x1-x0-3))/timeSize;
			if( showTimeLength<maxTime ){
				showPos_time= max(0,min( _showPosTime, maxTime-showTimeLength));
			}else{
				showPos_time= 0;
			}

			//再描画要求
			if( rewriteOnChange ){
				callRewrite();
			}
		}
		//	Accessor : showPos_line
		int getShowPosLine() const{
			return showPos_line;
		}
		void setShowPosLine(int _showPosLine){
			int y0= MARGIN+AXIS_SIZE_Y;
			int y1= size.y-MARGIN-SCROLL_BAR_WIDTH+1;

			int showLineNum= (size.y-SCROLL_BAR_WIDTH-AXIS_SIZE_Y-MARGIN*2)/(LABEL_SIZE_Y-1);
			if( showLineNum<(int)lineData.size() ){
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
		void setCursorListener(std::tr1::function<void()> listener){
			this->cursorListener= listener;
		}
		///	Accessor : selectListener
		void setSelectListener(std::tr1::function<void()> listener){
			this->selectListener= listener;
		}
		///	Accessor : keyShiftListener
		void setKeyShiftListener(std::tr1::function<void()> listener){
			this->keyShiftListener= listener;
		}
		///	Accessor : keyShiftListener
		void setKeyDeleteListener(std::tr1::function<void(const KeyInfo&)> listener){
			this->keyDeleteListener= listener;
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
		double maxTime,currentTime,showPos_time;
		int currentLine,showPos_line;
		std::tr1::function<void()> cursorListener;
		std::tr1::function<void()> selectListener;
		std::tr1::function<void()> keyShiftListener;
		std::tr1::function<void(const KeyInfo&)> keyDeleteListener;

		//行データクラス-------------
		class LineData{
		public:
			LineData(int nullflag, const std::basic_string<TCHAR>& _name, OWP_Timeline *_parent, unsigned int _lineIndex){
				m_nullflag = nullflag;
				name= _name;
				parent= _parent;
				lineIndex= _lineIndex;
			}
			LineData( const LineData& a ){
				_ASSERT_EXPR( 0, L"コピーコンストラクタは使えません" );
			}
			~LineData(){
				for(std::vector<Key*>::iterator it=key.begin();
					it!=key.end();
					it++){
					delete (*it);
				}
			}

			//キーデータクラス---------------
			class Key{
			public:
				Key(double _time, int _type=0, void *_object=NULL, double _length=1.0, bool _select=false ){
					time= _time;
					type= _type;
					length= _length;
					select= _select;
					object= _object;
				}

				////////////////////////// MemberVar /////////////////////////////
				double time,length;
				int type;
				bool select;
				void *object;
			};

			////////////////////////// MemberVar /////////////////////////////
			int m_nullflag;
			std::basic_string<TCHAR> name;
			std::vector<Key*> key;
			unsigned int lineIndex;

			//////////////////////////// Method //////////////////////////////
			//	Method : 描画
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
					hdcM->setPenAndBrush(NULL,RGB(min(baseR+20,255),min(baseG+20,255),min(baseB+20,255)));
					Rectangle(hdcM->hDC,x0,y0,x1,y1);
				}

				//ラベル
				hdcM->setFont(12,_T("ＭＳ ゴシック"));
				if( m_nullflag == 0 ){
					SetTextColor(hdcM->hDC,RGB(240,240,240));
				}else{
					SetTextColor(hdcM->hDC,RGB(0,240,240));
				}
				TextOut( hdcM->hDC,
						 posX+2, posY+parent->LABEL_SIZE_Y/2-5,
						 name.c_str(), _tcslen(name.c_str()));

				//枠
				hdcM->setPenAndBrush(RGB(min(baseR+20,255),min(baseG+20,255),min(baseB+20,255)),NULL);
				Rectangle(hdcM->hDC,x0,y0,x2,y1);
				hdcM->setPenAndBrush(RGB(min(baseR+20,255),min(baseG+20,255),min(baseB+20,255)),RGB(baseR,baseG,baseB));
				Rectangle(hdcM->hDC,x1-2,y0,x1+1,y1);
				Rectangle(hdcM->hDC,x1,y0,x2,y1);
				int x3= (int)((parent->maxTime-startTime)*timeSize) + x1+2;		//maxTime
				if( x1<=x3 && x3<=x2 ){
					MoveToEx(hdcM->hDC, x3,y0, NULL);
					LineTo(hdcM->hDC,   x3,y1);
				}
				if( x1<=x3+2 && x3+2<=x2 ){
					MoveToEx(hdcM->hDC, x3+2,y0, NULL);
					LineTo(hdcM->hDC,   x3+2,y1);
				}
				int x4= (int)((parent->currentTime-startTime)*timeSize) + x1+1;		//currentTime
				if( x1<=x4 && x4<x2 ){
					MoveToEx(hdcM->hDC, x4,y0, NULL);
					LineTo(hdcM->hDC,   x4,y1);
				}
				
				//ゴーストキー
				x1++; x2--;
				y0++; y1--;
				for(int i=0; i<(int)key.size(); i++){
					int xx0= (int)((key[i]->time-startTime+parent->ghostShiftTime)*timeSize) + x1;
					int xx1= (int)(key[i]->length*timeSize) + xx0 + 1;

					if( x2 <= xx0 ){
						break;
					}
					if( x1<=xx1 && key[i]->select ){

						if( x1<=xx1-1 && xx0+1<=x2 ){

							hdcM->setPenAndBrush(NULL,RGB(min(baseR+20,255),min(baseG+20,255),min(baseB+20,255)));
							Rectangle(hdcM->hDC,max(xx0+1,x1),y0+1,min(xx1-1,x2),y1-1);

						}

						if( x1<=xx0 ){
							hdcM->setPenAndBrush(RGB(baseR,baseG,baseB),NULL);
							MoveToEx(hdcM->hDC, xx0,y0, NULL);
							LineTo(hdcM->hDC,   xx0,y1);
						}

					}
				}

				//キー
				for(int i=0; i<(int)key.size(); i++){
					int xx0= (int)((key[i]->time-startTime)*timeSize) + x1;
					int xx1= (int)(key[i]->length*timeSize) + xx0 + 1;

					if( x2 <= xx0 ){
						break;
					}
					if( x1<=xx1 ){

						if( x1<=xx1-1 && xx0+1<=x2 ){

							if( key[i]->select ){
								hdcM->setPenAndBrush(NULL,RGB(240,240,240));
								Rectangle(hdcM->hDC,max(xx0+1,x1),y0+1,min(xx1-1,x2),y1-1);
								hdcM->setPenAndBrush(NULL,RGB(min(baseR+20,255),min(baseG+20,255),min(baseB+20,255)));
								Rectangle(hdcM->hDC,max(xx0+2,x1),y0+2,min(xx1-2,x2),y1-2);

							}else{
								hdcM->setPenAndBrush(NULL,RGB(240,240,240));
								Rectangle(hdcM->hDC,max(xx0+1,x1),y0+1,min(xx1-1,x2),y1-1);
							}

						}

						if( x1<=xx0 ){
							hdcM->setPenAndBrush(RGB(baseR,baseG,baseB),NULL);
							MoveToEx(hdcM->hDC, xx0,y0, NULL);
							LineTo(hdcM->hDC,   xx0,y1);
						}

					}
				}

			}

			//	Method : キーを作成する
			bool newKey(const double &_time, int _type=0, void* _object=NULL, double _length=1.0, bool _select=false){

				if(parent->allowSameTimeKey==false){
					int i= getKeyIndex(_time);
					if(i!=-1){
						//上書きリスナーコール
						if( parent->keyDeleteListener!=NULL ){
							KeyInfo ki;
							ki.label= name.c_str();
							ki.lineIndex= lineIndex;
							ki.time= key[i]->time;
							ki.timeIndex= -1;
							ki.object= key[i]->object;
							parent->keyDeleteListener(ki);
						}

						key[i]->type=   _type;
						key[i]->length= _length;
						key[i]->select= _select;
						key[i]->object= _object;
						return true;
					}
				}
				
				if( _time>=parent->maxTime || _time<0.0 ){
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
				if(_time+_length>=parent->maxTime){
					_length= parent->maxTime-_time;
				}

				if(key.size()!=0){
					int pushPos=key.size();
					for(int i=0; i<(int)key.size(); i++){
						if(_time <= key[i]->time){
							pushPos=i;
							break;
						}
					}
					key.push_back(key[key.size()-1]);
					for(int i=key.size()-2; i>pushPos; i--){
						key[i]=key[i-1];
					}
					key[pushPos]= new Key(_time,_type,_object,_length,_select);
				}else{
					key.push_back(new Key(_time,_type,_object,_length,_select));
				}
				
				return true;
			}
			//	Method : すべてのキーの選択を解除する
			void selectClear(){
				for(int i=0; i<(int)key.size(); i++){
					key[i]->select=false;
				}
			}
			//	Method : すべてのキーを選択する
			void selectAll(){
				for(int i=0; i<(int)key.size(); i++){
					key[i]->select=true;
				}
				parent->ghostShiftTime=0;
			}
			//	Method : 指定された時刻にあるキーをひとつ選択する
			bool selectKey(const double &_time){

				for(int i=0; i<(int)key.size(); i++){
					if( (_time-TIME_ERROR_WIDTH <= key[i]->time)
					 && (key[i]->time <= _time+TIME_ERROR_WIDTH) ){
						key[i]->select=true;
						return true;
					}
				}
				parent->ghostShiftTime=0;
				return false;

			}
			///	Method : 指定された時刻範囲にあるキーをすべて選択する
			///			 厳密モード(startTime<=keyTime<endTime)
			int selectKey(const double &startTime, const double &endTime){

				int selectCount=0;
				for(int i=0; i<(int)key.size(); i++){
//					if( startTime-TIME_ERROR_WIDTH <= key[i]->time
//					 && key[i]->time <= endTime+TIME_ERROR_WIDTH ){
					if( startTime<=key[i]->time && key[i]->time<endTime ){
						key[i]->select=true;
						selectCount++;
					}
				}
				parent->ghostShiftTime=0;
				return selectCount;

			}
			///	Method : 指定された時刻範囲にあるキーをすべて選択する
			///			 大雑把モード(startTime-TIME_ERROR_WIDTH<=keyTime<=endTime+TIME_ERROR_WIDTH)
			int selectKey2(const double &startTime, const double &endTime){

				int selectCount=0;
				for(int i=0; i<(int)key.size(); i++){
					if( startTime-TIME_ERROR_WIDTH <= key[i]->time
					 && key[i]->time <= endTime+TIME_ERROR_WIDTH ){
						key[i]->select=true;
						selectCount++;
					}
				}
				parent->ghostShiftTime=0;
				return selectCount;

			}
			//	Method : 指定された時刻にあるキーのインデックスをひとつ取得する
			int getKeyIndex(const double &_time){

				for(int i=0; i<(int)key.size(); i++){
					if( _time-TIME_ERROR_WIDTH <= key[i]->time
					 && key[i]->time <= _time+TIME_ERROR_WIDTH ){
						return i;
					}
				}
				return -1;

			}
			//	Method : 選択されているキーをすべて削除する
			unsigned int deleteKey(bool noCallListener=false){

				unsigned int deleteNum=0;
				for(unsigned int i=0; i<key.size(); i++){
					if( key[i]->select ){
						//リスナーコール
						if( !noCallListener && parent->keyDeleteListener!=NULL ){
							KeyInfo ki;
							ki.label= name.c_str();
							ki.lineIndex= lineIndex;
							ki.time= key[i]->time;
							ki.timeIndex= -1;
							ki.object= key[i]->object;
							parent->keyDeleteListener(ki);
						}

						delete key[i];
						deleteNum++;
					}else{
						key[i-deleteNum]=key[i];
					}
				}
				for(unsigned int i=0; i<deleteNum; i++){
					key.pop_back();
				}
				return deleteNum;

			}
			//	Method : 指定されたキーを削除する
			bool deleteKey(int index, bool noCallListener=false){
				if( key.size()<=(unsigned int)index ) return false;

				//リスナーコール
				if( !noCallListener && parent->keyDeleteListener!=NULL ){
					KeyInfo ki;
					ki.label= name.c_str();
					ki.lineIndex= lineIndex;
					ki.time= key[index]->time;
					ki.timeIndex= -1;
					ki.object= key[index]->object;
					parent->keyDeleteListener(ki);
				}

				delete key[index];
				for(int i=index; i<(int)key.size(); i++){
					key[i-1]=key[i];
				}
				key.pop_back();
				return true;
			}
			//	Method : 指定されたキーを移動する
			bool shiftKey(const double &shiftTime, int index){
				if( key.size()<=(unsigned int)index ) return false;

				double dstTime= key[index]->time+shiftTime;
				int type= key[index]->type;
				double length= key[index]->length;
				bool select= key[index]->select;
				void *object= key[index]->object;
				deleteKey(index,true);

				//既にキーが存在する場合
				if( !newKey(dstTime,type,object,length,select) ){
					//置き換えによるキー削除リスナーコール
					if( parent->keyDeleteListener!=NULL ){
						KeyInfo ki;
						ki.label= name.c_str();
						ki.lineIndex= lineIndex;
						ki.time= dstTime;
						ki.timeIndex= -1;
						ki.object= object;
						parent->keyDeleteListener(ki);
					}

					//存在するキーの値を置き換え後のキーの値に変更
					int dstIndex= getKeyIndex(dstTime);
					key[dstIndex]->type= type;
					key[dstIndex]->length= length;
					key[dstIndex]->object= object;
				}

				return true;
			}
			//	Method : 選択されているキーをすべて移動する
			unsigned int shiftKey(const double &shiftTime){

				unsigned int shiftNum=0;
				std::list<Key> shiftKeyList;
				for(int i=0; i<(int)key.size(); i++){
					if( key[i]->select ){
						shiftKeyList.push_back(*(key[i]));
//						shiftKey(shiftTime,i);
						shiftNum++;
					}
				}

				deleteKey(true);

				for(std::list<Key>::iterator itr=shiftKeyList.begin();
					itr!=shiftKeyList.end(); itr++){
					if( !newKey( itr->time+shiftTime, itr->type, itr->object, itr->length, itr->select ) ){
						//作成不能削除リスナーコール
						if( parent->keyDeleteListener!=NULL ){
							KeyInfo ki;
							ki.label= name.c_str();
							ki.lineIndex= lineIndex;
							ki.time= itr->time+shiftTime;
							ki.timeIndex= -1;
							ki.object= itr->object;
							parent->keyDeleteListener(ki);
						}
					}
				}

				return shiftNum;
			}
			//	Method : 選択されているキーをゴーストキーの位置へすべて移動する
			unsigned int shiftKey(){
				return shiftKey(parent->ghostShiftTime);
			}

			/////////////////////////// Accessor /////////////////////////////
			int getSelectNum(){

				int selectCount=0;
				for(int i=0; i<(int)key.size(); i++){
					if( key[i]->select ){
						selectCount++;
					}
				}
				return selectCount;

			}

		private:
			////////////////////////// MemberVar /////////////////////////////
			OWP_Timeline *parent;
			static const double TIME_ERROR_WIDTH;
		};
		std::vector<LineData*> lineData;
		double ghostShiftTime;


		static const int LABEL_SIZE_Y= 15;
		//static const int LABEL_SIZE_X= 75;
				static const int LABEL_SIZE_X= 155;
		static const int AXIS_SIZE_Y= 15;
		static const int SCROLL_BAR_WIDTH= 10;
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
	///	ウィンドウ内部品"レイヤーテーブル"クラス
	///</summary>
	class OWP_LayerTable: public OrgWindowParts{
	public:
		//////////////////// Constructor/Destructor //////////////////////
		OWP_LayerTable(const TCHAR *_name ){
			name= new TCHAR[256];
			_tcscpy_s(name,256,_name);

			cursorListener = [](){s_dummyfunc();};
			lineShiftListener = [](int beforIndex, int afterIndex){s_dummyfunc();};
			changeVisibleListener = [](int targetIndex){s_dummyfunc();};
			changeLockListener = [](int targetIndex){s_dummyfunc();};
			callPropertyListener = [](int targetIndex){s_dummyfunc();};

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

			for(std::vector<LineData*>::iterator itr=lineData.begin();
				itr!=lineData.end(); itr++){
				delete *itr;
			}
		}

		//////////////////////////// Method //////////////////////////////
		/// Method : 自動サイズ設定
		void autoResize(){
			size.y-= (size.y-MARGIN*2)%(LABEL_SIZE_Y-1)-1;
		}
		//	Method : 描画
		void draw(){
			drawEdge();

			int showLineNum= (size.y-MARGIN*2)/(LABEL_SIZE_Y-1);

			//行データ
			for(int i=showPosLine,j=0; i<(int)lineData.size() && j<showLineNum; i++,j++){
				bool highLight=false;
				if( i==currentLine ) highLight=true;

				lineData[i]->draw(	hdcM,
									pos.x+MARGIN,
									pos.y+MARGIN+j*(LABEL_SIZE_Y-1),
									size.x-SCROLL_BAR_WIDTH-MARGIN*2,highLight );
			}

			//ドラッグ移動の目印
			if( dragLine && shiftIndex!=0 ){
				int markPos= currentLine+shiftIndex-showPosLine;
				if( 0<=shiftIndex ) markPos+=1;

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
				int barSize= (y1-y0-4)*showLineNum/(int)lineData.size();
				int barStart= (y1-y0-4)*showPosLine/(int)lineData.size();
				if( showLineNum<(int)lineData.size() ){
					hdcM->setPenAndBrush(NULL,RGB(min(baseColor.r+20,255),min(baseColor.g+20,255),min(baseColor.b+20,255)));
					Rectangle(hdcM->hDC,x0+2,y0+2+barStart,x1-2,y0+2+barStart+barSize+1);
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
			if(popPos==-1) return false;
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
			if(lineData.size()<=(unsigned int)index) return false;

			bool ret= deleteLine(lineData[index]->name);

			//再描画要求
			if( ret && rewriteOnChange ){
				callRewrite();
			}
			return ret;
		}
		///	Method : マウス左ボタンダウンイベント受信
		void onLButtonDown(const MouseEvent& e){
			if( !canMouseControll ) return;

			int x0= MARGIN;
			int x1= MARGIN+ LABEL_SIZE_Y*2;
			int x2= size.x-SCROLL_BAR_WIDTH-MARGIN-1;
			int x3= size.x-MARGIN;
			int y0= MARGIN;
			int y1= size.y-MARGIN;

			//ボタン
			if( x0<=e.localX && e.localX<x1
			 && y0<=e.localY && e.localY<y1 ){
				unsigned int targetIndex= showPosLine+ (e.localY-y0)/(LABEL_SIZE_Y-1);
				if( targetIndex<=lineData.size()-1 ){
					//可視・不可視ボタン
					if( e.localX<x0+LABEL_SIZE_Y ){
						lineData[targetIndex]->visible= !lineData[targetIndex]->visible;
						//リスナーコール
						if( this->changeVisibleListener!=NULL ){
							(this->changeVisibleListener)(targetIndex);
						}
						dragVisibleButton= true;
						dragVisibleButtonValue= lineData[targetIndex]->visible;

					//ロックボタン
					}else{
						lineData[targetIndex]->lock= !lineData[targetIndex]->lock;
						//リスナーコール
						if( this->changeLockListener!=NULL ){
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
			if( x1<=e.localX && e.localX<x2
			 && y0<=e.localY && e.localY<y1 ){
				setCurrentLine( showPosLine+ (e.localY-y0)/(LABEL_SIZE_Y-1) );

				dragLine= true;
				shiftIndex= 0;
			}

			//ラインスクロールバー
			if( x2<=e.localX && e.localX<x3
			 && y0<=e.localY && e.localY<y1 ){
				int showLineNum= (y1-y0)/(LABEL_SIZE_Y-1);
				if( showLineNum<(int)lineData.size() ){
					int barSize= (y1-y0-4)*showLineNum/(int)lineData.size();

					int movableY= y1-y0-barSize;
					int movableYStart= y0+barSize/2;

					setShowPosLine( (e.localY-movableYStart)*((int)lineData.size()-showLineNum)/movableY );

					dragScrollBarLine=true;
				}
			}

		}
		///	Method : 左マウスボタンアップイベント受信
		void onLButtonUp(const MouseEvent& e){
			if( !canMouseControll ) return;

			//ドラッグによるキー移動
			if( dragLine ){
				int beforIndex= currentLine;
				int afterIndex= max(0, min(currentLine+shiftIndex, (int)lineData.size()-1));
				if( beforIndex!=afterIndex ){

					//リスナーコール
					if(this->lineShiftListener!=NULL){
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
		void onMouseMove(const MouseEvent& e){
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
				if( showPosLine+showLineNum-1 <= newCposLine ){
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
		void onRButtonDown(const MouseEvent& e){
			if( !canMouseControll ) return;

			int x0= MARGIN;
			int x1= MARGIN+ LABEL_SIZE_Y*2;
			int x2= size.x-SCROLL_BAR_WIDTH-MARGIN-1;
			int x3= size.x-MARGIN;
			int y0= MARGIN;
			int y1= size.y-MARGIN;

			//ラベル右クリック
			if( x1<=e.localX && e.localX<x2
			 && y0<=e.localY && e.localY<y1 ){
				mouseRBtnOnIndex= showPosLine+ (e.localY-y0)/(LABEL_SIZE_Y-1);

				setCurrentLine( mouseRBtnOnIndex );
			}

		}
		///	Method : 右マウスボタンアップイベント受信
		void onRButtonUp(const MouseEvent& e){
			if( !canMouseControll ) return;

			int x0= MARGIN;
			int x1= MARGIN+ LABEL_SIZE_Y*2;
			int x2= size.x-SCROLL_BAR_WIDTH-MARGIN-1;
			int x3= size.x-MARGIN;
			int y0= MARGIN;
			int y1= size.y-MARGIN;

			//プロパティコール
			if( mouseRBtnOnIndex != -1
			 && mouseRBtnOnIndex == showPosLine+(e.localY-y0)/(LABEL_SIZE_Y-1)
			 && x1<=e.localX && e.localX<x2
			 && y0<=e.localY && e.localY<y1 ){

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
			_tcscpy_s(name,256,value);

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
			if( 0<=index && index<(int)lineData.size() ){

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
			if( showPosLine+showLineNum-1 <= currentLine ){
				showPosLine= currentLine-showLineNum+1;
			}

			//リスナーコール
			if( !noCallListener && this->cursorListener!=NULL ){
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
			if( showLineNum<(int)lineData.size() ){
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
			for(std::vector<LineData*>::const_iterator itr=lineData.begin();
				itr!=lineData.end(); itr++){
				if( (*itr)->name==_name ){
					return (*itr)->visible;
				}
			}

			return false;
		}
		bool getVisible(int index){
			if(lineData.size()<=(unsigned int)index) return false;

			return lineData[index]->visible;
		}
		bool setVisible(const std::basic_string<TCHAR> &_name, bool value, bool noCallListener=false){
			for(unsigned int i=0; i<lineData.size(); i++){
				if( lineData[i]->name==_name ){

					//セットする値が現在の値と異なるときのみ変更する
					if( lineData[i]->visible!=value ){
						lineData[i]->visible= value;

						//リスナーコール
						if( !noCallListener && this->changeVisibleListener!=NULL ){
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
			if(lineData.size()<=(unsigned int)index) return false;

			//セットする値が現在の値と異なるときのみ変更する
			if( lineData[index]->visible!=value ){
				lineData[index]->visible= value;

				//リスナーコール
				if( !noCallListener && this->changeVisibleListener!=NULL ){
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
			for(std::vector<LineData*>::const_iterator itr=lineData.begin();
				itr!=lineData.end(); itr++){
				if( (*itr)->name==_name ){
					return (*itr)->lock;
				}
			}

			return false;
		}
		bool getLock(int index){
			if(lineData.size()<=(unsigned int)index) return false;

			return lineData[index]->lock;
		}
		bool setLock(const std::basic_string<TCHAR> &_name, bool value, bool noCallListener=false){
			for(unsigned int i=0; i<lineData.size(); i++){
				if( lineData[i]->name==_name ){

					//セットする値が現在の値と異なるときのみ変更する
					if( lineData[i]->lock!=value ){
						lineData[i]->lock= value;

						//リスナーコール
						if( !noCallListener && this->changeLockListener!=NULL ){
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
			if(lineData.size()<=(unsigned int)index) return false;

			//セットする値が現在の値と異なるときのみ変更する
			if( lineData[index]->lock!=value ){
				lineData[index]->lock= value;

				//リスナーコール
				if( !noCallListener && this->changeLockListener!=NULL ){
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
			if(lineData.size()<=(unsigned int)index) return false;

			return lineData[index]->object;
		}

		///	Accessor : cursorListener
		void setCursorListener(std::tr1::function<void()> listener){
			this->cursorListener= listener;
		}
		///	Accessor : lineShiftListener
		void setLineShiftListener(std::tr1::function<void(int beforIndex, int afterIndex)> listener){
			this->lineShiftListener= listener;
		}
		///	Accessor : changeVisibleListener
		void setChangeVisibleListener(std::tr1::function<void(int targetIndex)> listener){
			this->changeVisibleListener= listener;
		}
		///	Accessor : changeLockListener
		void setChangeLockListener(std::tr1::function<void(int targetIndex)> listener){
			this->changeLockListener= listener;
		}
		///	Accessor : callPropertyListener
		void setCallPropertyListener(std::tr1::function<void(int targetIndex)> listener){
			this->callPropertyListener= listener;
		}

	private:
		////////////////////////// MemberVar /////////////////////////////
		int currentLine,showPosLine;
		TCHAR *name;
		std::tr1::function<void()> cursorListener;										//カーソル位置が変更された直後に呼ばれる
		std::tr1::function<void(int beforIndex, int afterIndex)> lineShiftListener;		//移動が行われる直前に呼ばれる
		std::tr1::function<void(int targetIndex)> changeVisibleListener;				//可視状態が変更された直後に呼ばれる
		std::tr1::function<void(int targetIndex)> changeLockListener;					//ロック状態が変更された直後に呼ばれる
		std::tr1::function<void(int targetIndex)> callPropertyListener;					//レイヤーのプロパティを呼ぶ時に呼ばれる

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
					hdcM->setPenAndBrush(NULL,RGB(min(baseR+20,255),min(baseG+20,255),min(baseB+20,255)));
					Rectangle(hdcM->hDC,x3,y0,x1,y1);
				}

				//ラベル
				hdcM->setFont(12,_T("ＭＳ ゴシック"));
				SetTextColor(hdcM->hDC,RGB(240,240,240));
				TextOut( hdcM->hDC,
						 x3+2, posY+parent->LABEL_SIZE_Y/2-5,
						 name.c_str(), _tcslen(name.c_str()));

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
	///	ウィンドウ内部品"レイヤーテーブル"クラス
	///</summary>
	class OWP_ScrollWnd : public OrgWindowParts{
	public:
		//////////////////// Constructor/Destructor //////////////////////
		OWP_ScrollWnd(const TCHAR *_name){
			name = new TCHAR[256];
			_tcscpy_s(name, 256, _name);

			cursorListener = [](){s_dummyfunc(); };
			lineShiftListener = [](int beforIndex, int afterIndex){s_dummyfunc(); };
			changeVisibleListener = [](int targetIndex){s_dummyfunc(); };
			changeLockListener = [](int targetIndex){s_dummyfunc(); };
			callPropertyListener = [](int targetIndex){s_dummyfunc(); };

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

		}
		~OWP_ScrollWnd(){
			delete[] name;
		}

		//////////////////////////// Method //////////////////////////////
		///	Method : 親ウィンドウに登録
		void regist(OrgWindow *_parentWindow,
			WindowPos _pos, WindowSize _size,
			HDCMaster* _hdcM,
			unsigned char _baseR = 50, unsigned char _baseG = 70, unsigned char _baseB = 70){
			_regist(_parentWindow, _pos, _size, _hdcM, _baseR, _baseG, _baseB);

			//全てのグループ内部品を同じウィンドウに登録
			for (std::list<OrgWindowParts*>::iterator itr = partsList.begin();
				itr != partsList.end(); itr++){
				(*itr)->regist(parentWindow,
					_pos, _size,
					hdcM,
					baseColor.r, baseColor.g, baseColor.b);
			}

			//グループボックスと内部要素の位置とサイズを自動設定
			autoResize();
		}
		///	Method : グループ内部品を追加
		void addParts(OrgWindowParts& a){
			partsList.push_back(&a);

			// グループボックスがウィンドウに登録されている場合は
			// グループ内部品も同じウィンドウに登録する
			if (parentWindow != NULL){
				a.regist(parentWindow,
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
		void autoResize(){
			
			size = parentWindow->getSize();
			//pos = WindowPos(0, 0);

			int showLineNum = (size.y - MARGIN * 2) / (LABEL_SIZE_Y);

			int x0 = pos.x + size.x - MARGIN - SCROLL_BAR_WIDTH - 1;
			int x1 = x0 + SCROLL_BAR_WIDTH + 1;
			int y0 = pos.y + MARGIN;
			int y1 = pos.y + size.y - MARGIN;

			//if (open){
				//パーツエリアの位置とサイズを設定
				//partsAreaPos = pos + WindowPos(MARGIN, MARGIN);
				//partsAreaSize = WindowSize(size.x - MARGIN - SCROLL_BAR_WIDTH - 1, size.y - MARGIN);
				//currentPartsSizeY = MARGIN;
				partsAreaPos = pos;
				partsAreaSize = WindowSize(size.x - SCROLL_BAR_WIDTH - 1, size.y);
				currentPartsSizeY = 0;

				//全ての内部パーツの位置とサイズを自動設定
				int starty = showPosLine * (LABEL_SIZE_Y);
				//int neededy = LABEL_SIZE_Y * lineDatasize;
				//int offsetmaxy = max(MARGIN, (lineDatasize - showLineNum) * LABEL_SIZE_Y);
				//int starty = offsetmaxy * ((lineDatasize - showPosLine) / (lineDatasize - showLineNum));
				for (std::list<OrgWindowParts*>::iterator itr = partsList.begin(); itr != partsList.end(); itr++){
					(*itr)->setPos(WindowPos(partsAreaPos.x, partsAreaPos.y - starty));
					(*itr)->setSize(WindowSize(partsAreaSize.x, partsAreaSize.y));
					(*itr)->autoResize();

					//currentPartsSizeY += (*itr)->getSize().y;
				}

				//グループボックスのサイズを内部要素に合わせてトリミング
				//partsAreaSize.y = currentPartsSizeY;
				//size.y = partsAreaPos.y - pos.y + partsAreaSize.y + 3;

			//}
			//else{
			//	size.y = SIZE_CLOSE_Y;
			//}
		}
		

		/// Method : 自動サイズ設定
		//void autoResize(){
		//	size.y -= (size.y - MARGIN * 2) % (LABEL_SIZE_Y - 1) - 1;
		//}

		//	Method : 描画
		void draw(){
			drawEdge();

			//全ての内部パーツを描画
			if (open){
				for (std::list<OrgWindowParts*>::iterator itr = partsList.begin();
					itr != partsList.end(); itr++){
					(*itr)->draw();
				}
			}

			int showLineNum = (size.y - MARGIN * 2) / (LABEL_SIZE_Y);

			{//ラベルスクロールバー
				int x0 = pos.x + size.x - MARGIN - SCROLL_BAR_WIDTH - 1;
				int x1 = x0 + SCROLL_BAR_WIDTH + 1;
				int y0 = pos.y + MARGIN;
				int y1 = pos.y + size.y - MARGIN;

				//枠
				hdcM->setPenAndBrush(RGB(min(baseColor.r + 20, 255), min(baseColor.g + 20, 255), min(baseColor.b + 20, 255)), NULL);
				Rectangle(hdcM->hDC, x0, y0, x1, y1);

				//中身
				int barSize = (y1 - y0 - 4)*showLineNum / lineDatasize;
				int barStart = (y1 - y0 - 4)*showPosLine / lineDatasize;
				if (showLineNum<lineDatasize){
					hdcM->setPenAndBrush(NULL, RGB(min(baseColor.r + 20, 255), min(baseColor.g + 20, 255), min(baseColor.b + 20, 255)));
					Rectangle(hdcM->hDC, x0 + 2, y0 + 2 + barStart, x1 - 2, y0 + 2 + barStart + barSize + 1);
				}
			}
		}
		void onLButtonDown(const MouseEvent& e){
			if (!canMouseControll) return;

			int x0 = MARGIN;
			int x1 = MARGIN + LABEL_SIZE_Y * 2;
			int x2 = size.x - SCROLL_BAR_WIDTH - MARGIN - 1;
			int x3 = size.x - MARGIN;
			int y0 = MARGIN;
			int y1 = size.y - MARGIN;

			int showLineNum = (size.y - MARGIN * 2) / (LABEL_SIZE_Y);

			//ラインスクロールバー
			if (x2 <= e.localX && e.localX<x3
				&& y0 <= e.localY && e.localY<y1){
				int showLineNum = (y1 - y0) / (LABEL_SIZE_Y);
				if (showLineNum<lineDatasize){
					int barSize = (y1 - y0) * showLineNum / lineDatasize;

					int movableY = y1 - y0 - barSize;
					int movableYStart = y0 + barSize / 2;

					setShowPosLine((e.localY - movableYStart)*(lineDatasize - showLineNum) / movableY);

					dragScrollBarLine = true;
				}
			}
			autoResize();
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

			//再描画要求
			if (rewriteOnChange){
				callRewrite();
			}
		}
		///	Method : マウス移動イベント受信
		void onMouseMove(const MouseEvent& e){
			if (!canMouseControll) return;

			int x0 = MARGIN;
			int x1 = size.x - SCROLL_BAR_WIDTH - MARGIN - 1;
			int x2 = size.x - MARGIN;
			int y0 = MARGIN;
			int y1 = size.y - MARGIN;

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

		}
		///	Method : 右マウスボタンアップイベント受信
		void onRButtonUp(const MouseEvent& e){
			if (!canMouseControll) return;

			mouseRBtnOnIndex = -1;
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
			_tcscpy_s(name, 256, value);

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
			int y0 = MARGIN;
			int y1 = size.y - MARGIN + 1;

			int showLineNum = (size.y - MARGIN * 2) / (LABEL_SIZE_Y);
			if (showLineNum<lineDatasize){
				showPosLine = max(0, min(_showPosLine, lineDatasize - showLineNum));
			}
			else{
				showPosLine = 0;
			}

			//再描画要求
			if (rewriteOnChange){
				callRewrite();
			}
		}

		///	Accessor : cursorListener
		void setCursorListener(std::tr1::function<void()> listener){
			this->cursorListener = listener;
		}
		///	Accessor : lineShiftListener
		void setLineShiftListener(std::tr1::function<void(int beforIndex, int afterIndex)> listener){
			this->lineShiftListener = listener;
		}
		///	Accessor : changeVisibleListener
		void setChangeVisibleListener(std::tr1::function<void(int targetIndex)> listener){
			this->changeVisibleListener = listener;
		}
		///	Accessor : changeLockListener
		void setChangeLockListener(std::tr1::function<void(int targetIndex)> listener){
			this->changeLockListener = listener;
		}
		///	Accessor : callPropertyListener
		void setCallPropertyListener(std::tr1::function<void(int targetIndex)> listener){
			this->callPropertyListener = listener;
		}

	private:
		////////////////////////// MemberVar /////////////////////////////
		int currentLine, showPosLine;
		TCHAR *name;
		std::tr1::function<void()> cursorListener;										//カーソル位置が変更された直後に呼ばれる
		std::tr1::function<void(int beforIndex, int afterIndex)> lineShiftListener;		//移動が行われる直前に呼ばれる
		std::tr1::function<void(int targetIndex)> changeVisibleListener;				//可視状態が変更された直後に呼ばれる
		std::tr1::function<void(int targetIndex)> changeLockListener;					//ロック状態が変更された直後に呼ばれる
		std::tr1::function<void(int targetIndex)> callPropertyListener;					//レイヤーのプロパティを呼ぶ時に呼ばれる

		int lineDatasize;
		
		static const int LABEL_SIZE_Y = 15;
		static const int SCROLL_BAR_WIDTH = 10;
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
		std::tr1::function<void()> openListener;

		static const int SIZE_CLOSE_Y = 15;
		static const int BOX_POS_X = 3;
		static const int BOX_WIDTH = 10;
		static const int NAME_POS_X1 = 5;
		static const int NAME_POS_X2 = 3;
	};



void s_dummyfunc()
{
	static int dummycnt = 0;
	dummycnt++;
}
void s_dummyfuncKey( KeyboardEvent& keye )
{
	static int dummycnt = 0;
	dummycnt++;
}


}

#endif
