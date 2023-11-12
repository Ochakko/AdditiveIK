#ifndef GETMATERIALH
#define GETMATERIALH

#include <mqomaterial.h>
#include <map>
//using namespace std;

/**
 * @fn
 * GetMaterialFromNo
 * @breaf �ގ��ԍ�����ގ��N���X�̃|�C���^���擾����B
 * @param (map<int,CMQOMaterial*>& srcmat) IN �ގ��Ɣԍ���map�B����map����Y������ގ���T���o���B
 * @param (int matno) IN �ގ��ԍ��B
 * @return CMQOMaterial* �ގ��ԍ��ɊY������ގ��ւ̃|�C���^�B
 */
CMQOMaterial* GetMaterialFromNo( std::map<int,CMQOMaterial*>& srcmat, int matno );

#endif