#ifndef LMTFILEH
#define LMTFILEH

//#include <d3dx9.h>
#include <ChaVecCalc.h>
#include <Coef.h>
#include <XMLIO.h>

#include <map>
#include <string>

class CModel;
class CMQOObject;
class CBone;
class CRigidElem;


/*
制限角度ファイルのクラス。
Impはimpulseのimp。
*/


class CLmtFile : public CXMLIO
{
public:

/**
 * @fn
 * CLmtFile
 * @breaf コンストラクタ。
 * @return なし。
 */
	CLmtFile();

/**
 * @fn
 * ~CLmtFile
 * @breaf デストラクタ。
 * @return なし。
 * @detail 派生クラスのデストラクタなのでvirtual。
 */
	virtual ~CLmtFile();


/**
 * @fn
 * WriteLmtFile
 * @breaf 制限角度ファイルを書き出す。
 * @param (WCHAR* strpath) IN 書き出すファイルのパス。
 * @param (CModel* srcmodel) IN 書き出すモデルデータ。
 * @return 成功したら０。
 * @detail srcmodelに設定されている制限角度を書き出す。
 */
	int WriteLmtFile( WCHAR* strpath, CModel* srcmodel, char* fbxcomment);


/**
 * @fn
 * LoadLmtFile
 * @breaf 制限角度ファイルを読み込む。
 * @param (WCHAR* strpath) IN 読み込むファイルのパス。
 * @param (CModel* srcmodel) IN 読み込んだ制限角度パラメータを設定するモデル。
 * @return 成功したら０。
 */
	int LoadLmtFile( WCHAR* strpath, CModel* srcmodel, char* fbxcomment );


private:

/**
 * @fn
 * InitParams
 * @breaf メンバを初期化する。
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
 * WriteLmtReq
 * @breaf 制限角度書き出し関数をボーンに関して再帰的に呼び出す。
 * @param (CBone* srcbone) IN 制限角度パラメータを持つボーン。
 * @return なし。
 * @detail それぞれのボーンについての書き出し関数はWriteImpである。
 */
	void WriteLmtReq(bool limitdegflag, CBone* srcbone);

/**
 * @fn
 * WriteLmt
 * @breaf それぞれのボーンについて制限角度データを書き出す。
 * @param (CBone* srcbone) IN 制限角度パラメータを持つボーン。
 * @return 成功したら０。
 */
	int WriteLmt(bool limitdegflag, CBone* srcbone);


/**
 * @fn
 * ReadBone
 * @breaf ボーンごとの読み込み関数。
 * @param (XMLIOBUF* xmliobuf) IN XMLのあるボーン１つ分のバッファ。
 * @return 成功したら０。
 */
	int ReadBone(bool limitdegflag, XMLIOBUF* xmliobuf, CModel* srcmodel);

/**
 * @fn
 * ReadRE
 * @breaf 剛体ごとの読み込み関数。
 * @param (XMLIOBUF* xmliobuf) IN XMLのある剛体に１つ分のバッファ。
 * @param (CBone* srcbone) IN 制限角度パラメータを持つボーン。
 * @return 成功したら０。
 */
	//int ReadRE( XMLIOBUF* xmliobuf, CBone* srcbone );



private:
	CModel* m_model;
	ANGLELIMIT m_anglelimit;

};

#endif