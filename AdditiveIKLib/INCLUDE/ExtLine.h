#ifndef EXTLINEH
#define EXTLINEH

#include <coef.h>
//#include <D3DX9.h>
#include <ChaVecCalc.h>

class CMQOFace;
class CMQOMaterial;
class CModel;

class CExtLine
{
public:

/**
 * @fn
 * CExtLine
 * @breaf コンストラクタ。
 * @return なし。
 */
	CExtLine();

/**
 * @fn
 * ~CExtLine
 * @breaf デストラクタ。
 * @return なし。
 */
	~CExtLine();

/**
 * @fn
 * CreateExtLine
 * @breaf 線分データを作成する。
 * @param (int pointnum) IN 頂点データpointptrの頂点数。
 * @param (int facenum) IN 面データfaceptrの面数。
 * @param (ChaVector3* pointptr) IN 頂点データ。
 * @param (CMQOFace* faceptr) IN 面データ。
 * @param (ChaVector4 srccol) IN 線分の色。
 * @return 成功したら０。
 */
	int CreateExtLine( CModel* srcmodel, int pointnum, int facenum, ChaVector3* pointptr, CMQOFace* faceptr, ChaVector4 srccol );


/**
 * @fn
 * CalcBound
 * @breaf 線分を囲い込むバウンダリーデータを作成する。
 * @return 成功したら０。
 */
	int CalcBound();


private:

/**
 * @fn
 * InitParams
 * @breaf メンバを初期化する。
 * @return ０。
 */
	int InitParams();

/**
 * @fn
 * DestroyObjs
 * @breaf アロケートしたメモリを開放する。
 * @return ０。
 */
	int DestroyObjs();


/**
 * @fn
 * CreateBuffer
 * @breaf ラインバッファを作成してセットする。
 * @param (EXTLINEV* lineptr) IN,OUT このバッファに線分データをセットする。
 * @param (int arrayleng) IN lineptrの線分数。
 * @param (int* setnum) OUT セットした線分数。
 * @return 成功したら０。
 * @detail まず必要なデータ長を確かめるためにlineptrに０をセットして呼ぶ。setnumの値をみてlineptrをアロケートし、lineptrを引数に指定してもう一度呼び出す。
 */
	int CreateBuffer( EXTLINEV* lineptr, int arrayleng, int* setnum );

public:
	CMQOMaterial* GetMaterial() {
		return m_material;
	};
	int GetLineNum()
	{
		return m_linenum;
	}
	EXTLINEV* GetExtLineV()
	{
		return m_linev;
	}
	MODELBOUND GetBound()
	{
		return m_bound;
	}
	ChaVector4 GetColor()
	{
		return m_color;
	}

private:

	int m_linenum;//線分の数。
	EXTLINEV* m_linev;//頂点データ。
	ChaVector4 m_color;//線分の色
	MODELBOUND	m_bound;//線分を囲い込むバウンダリーデータ。

private:
	//以下、MQOObjectでアロケートした外部ポインタとデータ。
	int m_pointnum;//頂点数。
	int m_facenum;//面（線分）の数。
	ChaVector3* m_pointptr;//頂点データ。
	CMQOFace* m_faceptr;//面(線分)データ。
	CMQOMaterial* m_material;
};


#endif