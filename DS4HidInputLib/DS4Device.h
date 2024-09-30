#pragma once
#include "DSDevice.h"
#include "DSStatus.h"

class DS4Device : public DSDevice
{
public:
	DS4Device(HidDevice srcdevice, int srccontrollerId, UINT leng) ;
	bool SendOutputReport() override;
	bool ChangeLedColor(const LED &led)  override;
	void ChangeVibration(UCHAR right, UCHAR left) override;
	bool GetInputReport() override;
	bool Destroy() override;
	inline bool IsDSDevice() override
	{
		return device.isDevice;
	}
	bool GetButton(UCHAR keyType) override;
	bool GetButtonDown(UCHAR keyType) override;
	bool GetButtonUp(UCHAR keyType) override;
	float GetAxis(UCHAR axisType) override;
	void Reset()  override;

private:
	HidDevice device;
	USHORT inputDataLength;
	USHORT outputDataLength;
	UCHAR* outputData = nullptr;
	UCHAR* inputData = nullptr;
	DSStatus status;
	int controllerId = -1;
};