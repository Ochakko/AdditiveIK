#ifndef IMPFILEH
#define IMPFILEH

//#include <d3dx9.h>
#include <ChaVecCalc.h>
#include <coef.h>
#include <XMLIO.h>

#include <map>
#include <string>

class CModel;
class CMQOObject;
class CBone;
class CRigidElem;


/*
インパルスファイルのクラス。
Impはimpulseのimp。
*/


class CImpFile : public CXMLIO
{
public:

/**
 * @fn
 * CImpFile
 * @breaf コンストラクタ。
 * @return なし。
 */
	CImpFile();

/**
 * @fn
 * ~CImpFile
 * @breaf デストラクタ。
 * @return なし。
 * @detail 派生クラスのデストラクタなのでvirtual。
 */
	virtual ~CImpFile();


/**
 * @fn
 * WriteImpFile
 * @breaf インパルスファイルを書き出す。
 * @param (WCHAR* strpath) IN 書き出すファイルのパス。
 * @param (CModel* srcmodel) IN 書き出すモデルデータ。
 * @return 成功したら０。
 * @detail srcmodelに設定されているインパルスを書き出す。
 */
	int WriteImpFile( WCHAR* strpath, CModel* srcmodel );


/**
 * @fn
 * LoadImpFile
 * @breaf インパルスファイルを読み込む。
 * @param (WCHAR* strpath) IN 読み込むファイルのパス。
 * @param (CModel* srcmodel) IN 読み込んだインパルスパラメータを設定するモデル。
 * @return 成功したら０。
 */
	int LoadImpFile( WCHAR* strpath, CModel* srcmodel );


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
 * WriteImpReq
 * @breaf インパルスデータ書き出し関数をボーンに関して再帰的に呼び出す。
 * @param (CBone* srcbone) IN インパルスパラメータを持つボーン。
 * @return なし。
 * @detail それぞれのボーンについての書き出し関数はWriteImpである。
 */
	void WriteImpReq( CBone* srcbone );

/**
 * @fn
 * WriteImp
 * @breaf それぞれのボーンについてインパルスデータを書き出す。
 * @param (CBone* srcbone) IN インパルスパラメータを持つボーン。
 * @return 成功したら０。
 */
	int WriteImp( CBone* srcbone );


/**
 * @fn
 * ReadBone
 * @breaf ボーンごとの読み込み関数。
 * @param (XMLIOBUF* xmliobuf) IN XMLのあるボーン１つ分のバッファ。
 * @return 成功したら０。
 */
	int ReadBone(XMLIOBUF* xmliobuf, CModel* srcmodel);

/**
 * @fn
 * ReadRE
 * @breaf 剛体ごとの読み込み関数。
 * @param (XMLIOBUF* xmliobuf) IN XMLのある剛体に１つ分のバッファ。
 * @param (CBone* srcbone) IN インパルスパラメータを持つボーン。
 * @return 成功したら０。
 */
	int ReadRE(XMLIOBUF* xmliobuf, CModel* srcmodel, CBone* srcbone);


private:
	std::string m_strimp;//インパルスファイル名。
	CModel* m_model;

};

#endif