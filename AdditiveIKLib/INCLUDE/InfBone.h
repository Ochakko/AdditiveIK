#ifndef INFBONEH
#define INFBONEH

#include <coef.h>
#include <map>

class CMQOObject;

/*
ボーン影響度クラス。
InfはinfluenceのInf。
*/

class CInfBone
{
public:

/**
 * @fn
 * CInfBone
 * @breaf コンストラクタ。
 * @return なし。
 */
	CInfBone();

/**
 * @fn
 * ~CInfBone
 * @breaf デストラクタ。
 * @return なし。
 */
	~CInfBone();


/**
 * @fn
 * InitParams
 * @breaf メンバを初期化する。
 * @return ０。
 */
	int InitParams();

/**
 * @fn
 * ExistBone
 * @breaf 指定したボーンの影響度があるかどうかを調べる。
 * @param (CMQOObject* srcobj) IN 影響度を調べる３Dオブジェクト。
 * @param (int srcboneno) IN 影響度があるか調べるボーンの番号。
 * @return 指定したボーンの影響がなければ-1を返す。影響があればその影響度のインデックスを返す。
 */
	int ExistBone( CMQOObject* srcobj, int srcboneno );

/**
 * @fn
 * AddInfElem
 * @breaf 指定オブジェクトに影響度を登録する。
 * @param (CMQOObject* srcobj) IN 影響度を設定する３Dオブジェクト。
 * @param (INFELEM srcie) IN 影響度データ。
 * @return 成功したら０。
 */
	int AddInfElem( CMQOObject* srcobj, INFELEM srcie );

/**
 * @fn
 * NormalizeInf
 * @breaf 影響度を正規化する。
 * @param (CMQOObject* srcobj) IN 影響度データを持つ３Dオブジェクト。
 * @return 成功したら０。
 */
	int NormalizeInf( CMQOObject* srcobj );


	INFDATA* GetInfData(CMQOObject* srcobj);

private:

/**
 * @fn
 * DestroyObjs
 * @breaf アロケートしたメモリを開放する。
 * @return ０。
 */
	int DestroyObjs();

/**
 * @fn
 * InitElem
 * @breaf 影響度データを初期化する。
 * @param (INFDATA* infptr) IN 初期化されるデータ。
 * @return 成功したら０。
 */
	int InitElem( INFDATA* infptr );

private:
	std::map<CMQOObject*, INFDATA*> m_infdata;//影響度データ

};

#endif

