#ifndef GETMATERIALH
#define GETMATERIALH

#include <mqomaterial.h>
#include <map>
//using namespace std;

/**
 * @fn
 * GetMaterialFromNo
 * @breaf 材質番号から材質クラスのポインタを取得する。
 * @param (map<int,CMQOMaterial*>& srcmat) IN 材質と番号のmap。このmapから該当する材質を探し出す。
 * @param (int matno) IN 材質番号。
 * @return CMQOMaterial* 材質番号に該当する材質へのポインタ。
 */
CMQOMaterial* GetMaterialFromNo( std::map<int,CMQOMaterial*>& srcmat, int matno );

#endif