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
 * @breaf 英数字アンダーバー以外の文字が名前にある場合、それを置き換えて英語表現出来る名前を作る。
 * @param (int type) IN 表示オブジェクトはENGNAME_DISP, ボーンはENGNAME_BONE, モーション名はENGNAME_MOTIONを指定する。
 * @param (char* srcname) IN 元の名前。２５６バイト長のバッファ。
 * @param (int srcleng) IN 256を指定する。バッファの長さが２５６長固定であることの確認のための引数である。
 * @param (char* dstname) OUT 変換後の名前。２５６バイト長のバッファ。
 * @param (int dstleng) IN 256を指定する。バッファの長さが２５６長固定であることの確認のための引数である。
 * @return 成功したら０。
 * @detail [L], [R]は_L_, _R_にする。ボーンには_Jointを付ける。モーションには_Motionを付ける。英数字以外がある場合は「ReplacedName」と数字の組み合わせの名前を作成する。
 */
	int ConvEngName( int type, char* srcname, int srcleng, char* dstname, int dstleng );


	int TermJointRepeats(char* dstname);


#else
	extern int ConvEngName( int type, char* srcname, int srcleng, char* dstname, int dstleng );
	extern int TermJointRepeats(char* dstname);
#endif

#endif