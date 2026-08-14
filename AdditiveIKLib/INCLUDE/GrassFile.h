#ifndef GRASSFILEH
#define GRASSFILEH

//#include <d3dx9.h>
#include <ChaVecCalc.h>
#include <coef.h>
#include <XMLIO.h>

#include <string>

/*
bulletによる当たり判定付きの地面データの入出力用クラス。
*/

class CGrassFile : public CXMLIO
{
public:

/**
 * @fn
 * CGrassFile
 * @breaf コンストラクタ
 * @return なし。
 */
	CGrassFile();
	
/**
 * @fn
 * ~CGrassFile
 * @breaf デストラクタ。
 * @return なし。
 * @detail 派生クラスのデストラクタなのでvirtual。
 */	
	virtual ~CGrassFile();


/**
 * @fn
 * WriteGrassFile
 * @breaf 地面ファイルを出力する。
 * @param (WCHAR* strpath) IN 書き出すファイルのパス。
 * @return 成功したら０。
 */
	int WriteGrassFile(WCHAR* strpath);
	
/**
 * @fn
 * LoadGrassFile
 * @breaf 地面ファイルを読み込む。
 * @param (WCHAR* strpath) IN 書き出すファイルのパス。
 * @return 成功したら０。
 */
	int LoadGrassFile(WCHAR* strpath);

private:

/**
 * @fn
 * InitParams
 * @breaf メンバの初期化。
 * @return ０。
 */
	virtual int InitParams();
	
/**
 * @fn
 * DestroyObjs
 * @breaf アロケートしたメモリを開放する。
 * @return ０。
 */
	virtual int DestroyObjs();

/**
	* @fn
	* WriteColiID
	* @breaf 地面のパラメータをファイルに書き出す。
	* @return 成功したら０。
	*/
	int WriteGrass();


/**
	* @fn
	* ReadColiID
	* @breaf 地面ファイルの内容を読み込む。
	* @param (XMLIOBUF* xmliobuf) IN XML読み込み用バッファ。
	* @return 成功したら０。
	*/
	int ReadGrass(XMLIOBUF* xmliobuf);

};

#endif