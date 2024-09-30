#pragma once

#include<Windows.h>
#include<SetupAPI.h>
#include<bluetoothapis.h>
#include<iostream>

extern "C"
{
#include <hidsdi.h>
}
//
//#pragma comment(lib,"hid.lib")
//#pragma comment(lib,"setupapi.lib")
//
//using namespace std;

class HidDevice
{
public:
	HidDevice();
	~HidDevice();

	HidDevice Create(char* path, UINT leng, int id);
	HIDP_CAPS GetCapabilities() ;
	void Destroy(bool closeflag) ;
	inline HANDLE GetHandle() 
	{
		return deviceHandle;
	};
	inline USHORT GetProductID() 
	{
		return productID;
	};
	inline USHORT GetVendorID() 
	{
		return vendorID;
	};
	inline void SetDevicePath(char* srcpath)
	{
		devicePath = srcpath;
	};
	inline char* GetDevicePath()
	{
		return devicePath;
	};
	bool GetIsCapabilities()
	{
		return isCapabilities;
	}

	void InitParams();

public:
	BOOL isDevice;

private:
	USHORT productID;
	USHORT vendorID;
	HANDLE deviceHandle;
	HIDP_CAPS capabilities;
	bool isCapabilities;
	char* devicePath;

};