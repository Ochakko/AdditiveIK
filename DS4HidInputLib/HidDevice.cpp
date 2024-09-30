#include "pch.h"


#include "HidDevice.h"


#pragma comment(lib,"hid.lib")
#pragma comment(lib,"setupapi.lib")

using namespace std;

HidDevice::HidDevice()
{
	InitParams();
}

HidDevice::~HidDevice()
{
	//Close����Ƃ��Ƃ��Ȃ��Ƃ�������̂ŁADestroy�͎蓮�ŌĂ΂Ȃ��Ă͂Ȃ�Ȃ�
}

void HidDevice::InitParams()
{
	isDevice = FALSE;
	productID = 0;
	vendorID = 0;
	deviceHandle = INVALID_HANDLE_VALUE;
	ZeroMemory(&capabilities, sizeof(HIDP_CAPS));
	isCapabilities = false;
	devicePath = 0;
}

HidDevice HidDevice::Create(char *path, UINT leng, int id)
{
	//if (devicePath) {
	//	free(devicePath);
	//	devicePath = 0;
	//}

	Destroy(true);


	if (!path) {
		_ASSERT(0);
		isDevice = FALSE;
		//InitParams();
		return *this;//!!!!!!!!
	}


	//�p�X�̃R�s�[
	size_t size = 1;
	//for (UINT i = 0; path[i] != '\0'; i++)
	//{
	//	size += 1;
	//}
	//size = strlen(path);
	//if ((size <= 0) || (size > 2048)) {
	//	_ASSERT(0);
	//	//isDevice = FALSE;
	//	Destroy();
	//	return *this;//!!!!!!!!
	//}

	//devicePath = new char[sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA) * leng];
	devicePath = (char*)malloc(sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA) * ((size_t)leng + 1));
	if (!devicePath) {
		_ASSERT(0);
		isDevice = FALSE;
		//Destroy();
		return *this;//!!!!!!!!
	}
	if (devicePath) {
		ZeroMemory(devicePath, sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA) * ((size_t)leng + 1));
	}
	//for (UINT i = 0; i < size; i++)
	//{
	//	devicePath[i] = path[i];
	//}
	strcpy_s(devicePath, (sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA) * ((size_t)leng + 1)), path);

	//�f�o�C�X�n���h���̍쐬
	deviceHandle = CreateFile(
		devicePath,
		(GENERIC_READ | GENERIC_WRITE),
		(FILE_SHARE_READ | FILE_SHARE_WRITE),
		NULL,
		OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (deviceHandle == INVALID_HANDLE_VALUE) {
		//_ASSERT(0);
		//Destroy();
		if (devicePath) {
			delete[] devicePath;
			devicePath = 0;
		}
		isDevice = FALSE;
		return *this;//!!!!!!!!
	}


	HIDD_ATTRIBUTES attributes;
	if (HidD_GetAttributes(deviceHandle, &attributes))
	{
		productID = attributes.ProductID;
		vendorID = attributes.VendorID;
	}
	else {
		Destroy(true);
		isDevice = FALSE;
		return *this;//!!!!!!!!
	}

	isCapabilities = false;
	isDevice = TRUE;

	return *this;
}

HIDP_CAPS HidDevice::GetCapabilities() 
{
	if (isCapabilities == true)
	{
		return capabilities;
	}

	PHIDP_PREPARSED_DATA preparsedData;
	if (HidD_GetPreparsedData(deviceHandle, &preparsedData))
	{
		//��{���̎擾
		if (HIDP_STATUS_SUCCESS == HidP_GetCaps(preparsedData, &capabilities))
		{
			isCapabilities = true;
		}
	}

	HidD_FreePreparsedData(preparsedData);

	return capabilities;
}

void HidDevice::Destroy(bool closeflag) 
{

	//delete[] devicePath;
	if (devicePath) {
	//	delete[] devicePath;
		free(devicePath);
		devicePath = 0;
	}

	if (closeflag) {
		//isDevice = 0;
		if (isDevice && isCapabilities && (deviceHandle != INVALID_HANDLE_VALUE)) {
			//if ((deviceHandle != INVALID_HANDLE_VALUE)) {
				//�L�[�{�[�h�Ȃǂ͕��Ȃ��B�g��������������B
			CloseHandle(deviceHandle);
		}
		deviceHandle = INVALID_HANDLE_VALUE;
	}

	InitParams();
}
