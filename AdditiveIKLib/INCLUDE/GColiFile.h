#ifndef GCOLIFILEH
#define GCOLIFILEH

//#include <d3dx9.h>
#include <ChaVecCalc.h>
#include <coef.h>
#include <XMLIO.h>

#include <unordered_map>
#include <string>

class BPWorld;
class CModel;

/*
bulletによる当たり判定付きの地面データの入出力用クラス。
*/

class CGColiFile : public CXMLIO
{
public:

/**
 * @fn
 * CGColiFile
 * @breaf コンストラクタ
 * @return なし。
 */
	CGColiFile();
	
/**
 * @fn
 * ~CGColiFile
 * @breaf デストラクタ。
 * @return なし。
 * @detail 派生クラスのデストラクタなのでvirtual。
 */	
	virtual ~CGColiFile();


/**
 * @fn
 * WriteGColiFile
 * @breaf 地面ファイルを出力する。
 * @param (WCHAR* strpath) IN 書き出すファイルのパス。
 * @param (BPWorld* srcbpw) IN 物理管理クラス。
 * @return 成功したら０。
 */
	int WriteGColiFile( WCHAR* strpath, BPWorld* srcbpw );
	
/**
 * @fn
 * LoadGColiFile
 * @breaf 地面ファイルを読み込む。
 * @param (CModel* srcmodel) IN 地面の形状データ。
 * @param (WCHAR* strpath) IN 書き出すファイルのパス。
 * @param (BPWorld* srcbpw) IN 物理管理クラス。
 * @return 成功したら０。
 */
	int LoadGColiFile( CModel* srcmodel, WCHAR* strpath, BPWorld* srcbpw );

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
	int WriteColiID();


/**
 * @fn
 * ReadColiID
 * @breaf 地面ファイルの内容を読み込む。
 * @param (XMLIOBUF* xmliobuf) IN XML読み込み用バッファ。
 * @return 成功したら０。
 */
	int ReadColiID( XMLIOBUF* xmliobuf );

private:
	BPWorld* m_bpw;//物理管理クラス。
	CModel* m_model;//地面形状データの外部ポインタ。

};

#endif