#include "pch.h"


#include "DSManager.h"
#include "DS4Device.h"
#include "DSenseDevice.h"

//#include <rpc.h>
//#pragma comment ( lib, "Rpcrt4.lib" )


DSManager::DSManager()
{
	for (int i = 0; i < 4; i++) {
		dsDevice[i] = 0;
	}
	initflag = true;
}


//BOOL GuidFromString(GUID* pGuid, std::string oGuidString)
//{
//	// 文字列をGUIDに変換する
//	if (RPC_S_OK == ::UuidFromString((RPC_CSTR)oGuidString.c_str(), (UUID*)pGuid)) {
//
//		// 変換できました。
//		return(TRUE);
//	}
//	return(FALSE);
//}

bool DSManager::GetDevice()
{
	bool foundflag = false;

	GUID hidGuid;
	HDEVINFO hdevInfo;


	HidD_GetHidGuid(&hidGuid);

	//hidGuid = GUID("000011240000-10008-8000-00805F9B34FB");
	//std::string strguid = "000011240000-10008-8000-00805F9B34FB";//HID対応のbluetoothデバイス
	//GuidFromString(&hidGuid, strguid);

	hdevInfo = SetupDiGetClassDevs(&hidGuid, NULL, NULL, (DIGCF_PRESENT | DIGCF_INTERFACEDEVICE));
	SP_DEVICE_INTERFACE_DATA data;
	data.cbSize = sizeof(SP_INTERFACE_DEVICE_DATA);
	for (int i = 0; SetupDiEnumDeviceInterfaces(hdevInfo, NULL, &hidGuid, i, &data); i++)
	{
		PSP_INTERFACE_DEVICE_DETAIL_DATA detail = 0;
		DWORD size = 0;

		//デバイスインタフェース詳細情報のメモリサイズを取得
		SetupDiGetDeviceInterfaceDetail(hdevInfo, &data, NULL, 0, &size, NULL);

		if (size > 0) {//sizeが正の場合のif文追加　2021/03/25 add OchakkoLAB

			//デバイスインターフェース詳細情報の領域を確保する
			//detail = new SP_INTERFACE_DEVICE_DETAIL_DATA[size];
			//detail = new SP_INTERFACE_DEVICE_DETAIL_DATA();
			detail = (PSP_INTERFACE_DEVICE_DETAIL_DATA)malloc(sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA) * size);
			//detail = (PSP_INTERFACE_DEVICE_DETAIL_DATA)malloc(sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA));
			if (detail)
			{
				//ZeroMemory(detail, sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA) * size);
				ZeroMemory(detail, sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA));

				DWORD len = 0;
				//memset(detail, 0, size);

				//デバイスインターフェースの詳細情報を読込む
				detail->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);
				if (SetupDiGetInterfaceDeviceDetail(hdevInfo, &data, detail, size, &len, NULL))
				{
					//Hidデバイスの作成
					HidDevice *pdevice = 0;
					pdevice = new HidDevice();//使用するときにはポインタもコピー。つまり使用する場合には破棄してはいけない。
					if (pdevice) {
						pdevice->Create(detail->DevicePath, size + 4, 0);
						//if (pdevice->isDevice && pdevice->GetIsCapabilities()) {
						if (pdevice->isDevice) {

							if (pdevice->GetVendorID() == 0x54C && pdevice->GetProductID() == 0xce6) {
								//	PS5コントローラー
								for (int i = 0; i < 4; i++) {
									if (!dsDevice[i]) {
										dsDevice[i] = new DSenseDevice(*pdevice, i, size + 4);
										foundflag = true;

										//pdevice//使用するときにはポインタもコピー。つまり使用する場合にはCloseHandleしてはいけない。
										pdevice->Destroy(false);
										delete pdevice;
										pdevice = 0;
										if (detail) {
											free(detail);
											detail = 0;
										}
										break;
									}
								}
							}
							else if (pdevice->GetVendorID() == 0x54c && (pdevice->GetProductID() == 0x5c4 || pdevice->GetProductID() == 0x9CC))
							{
								for (int i = 0; i < 4; i++) {
									if (!dsDevice[i]) {
										//PS4コントローラーとして設定
										dsDevice[i] = new DS4Device(*pdevice, i, size + 4);
										foundflag = true;

										//pdevice//使用するときにはポインタもコピー。つまり使用する場合にはCloseHandleしてはいけない。
										pdevice->Destroy(false);
										delete pdevice;
										pdevice = 0;
										if (detail) {
											free(detail);
											detail = 0;
										}
										break;
									}
								}
								
							}
							else
							{
								//デバイスの破棄
								if (pdevice) {
									pdevice->Destroy(true);
									delete pdevice;
									pdevice = 0;
								}
								if (detail) {
									free(detail);
									detail = 0;
								}
							}
						}
						else {
							//デバイスの破棄
							if (pdevice) {
								pdevice->Destroy(true);
								delete pdevice;
								pdevice = 0;
							}
							if (detail) {
								free(detail);
								detail = 0;
							}
						}
						

					}
				}
			}
			
		}
	}
	SetupDestroyDiskSpaceList(hdevInfo);
	return foundflag;
}

bool DSManager::IsDevice(int id)
{
	if (!dsDevice[id]) return false;
	bool device = dsDevice[id]->IsDSDevice();
	if (!device)
	{
		dsDevice[id]->Destroy();
		delete dsDevice[id];
		dsDevice[id] = nullptr;
	}
	return device;
}

void DSManager::ChangeColor(int id, const UCHAR r, const UCHAR g, const UCHAR b)
{
	if (!IsDevice(id)) return;
	dsDevice[id]->ChangeLedColor(LED(r, g, b));
}

void DSManager::ChangePlayerLight(int id, const UCHAR val)
{
	if (!IsDevice(id)) return;
	dsDevice[id]->ChangePlayerLight(val);
}

void DSManager::ChangeVibration(int id, UCHAR right, UCHAR left)
{
	if (!IsDevice(id)) return;
	dsDevice[id]->ChangeVibration(right, left);
}

void DSManager::ChangeTriggerLock(int id, UCHAR rMode, UCHAR right, UCHAR lMode, UCHAR left)
{
	if (!IsDevice(id)) return;
	dsDevice[id]->ChangeTriggerLock(rMode, right, lMode, left);
}

void DSManager::SendOutput(int id)
{
	if (!IsDevice(id)) return;
	dsDevice[id]->SendOutputReport();
}

void DSManager::InputUpdate()
{
	for (int i = 0; i < 4; i++)
	{
		if (dsDevice[i] == nullptr) continue;
		if (dsDevice[i]->IsDSDevice() == false) { continue; }
		dsDevice[i]->GetInputReport();
	}
}

bool DSManager::GetButton(int id, UCHAR key)
{
	if (IsDevice(id) == false) return false;
	return dsDevice[id]->GetButton(key);
}

bool DSManager::GetButtonDown(int id, UCHAR key)
{
	if (IsDevice(id) == false) return false;
	return dsDevice[id]->GetButtonDown(key);
}

bool DSManager::GetButtonUp(int id, UCHAR key)
{
	if (IsDevice(id) == false) return false;
	return dsDevice[id]->GetButtonUp(key);
}

float DSManager::GetAxis(int id, UCHAR axis)
{
	if (IsDevice(id) == false) return 0;
	return dsDevice[id]->GetAxis(axis);
}

void DSManager::Destroy()
{
	for (int i = 0; i < 4; i++)
	{
		if (dsDevice[i]) {
			dsDevice[i]->Destroy();
			delete dsDevice[i];//20210325 add
			dsDevice[i] = nullptr;
		}
	}
}
