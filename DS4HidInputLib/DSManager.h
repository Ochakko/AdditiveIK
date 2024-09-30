#pragma once
#include "DSDevice.h"

class DSManager
{
public:
	DSManager();
	bool GetDevice();
	bool IsDevice(int id);
	void ChangeColor(int id, const UCHAR r, const UCHAR g, const UCHAR b);
	void ChangePlayerLight(int id, const UCHAR val);
	void ChangeVibration(int id, UCHAR right, UCHAR left);
	void ChangeTriggerLock(int id, UCHAR rMode, UCHAR right, UCHAR lMode, UCHAR left);
	void SendOutput(int id);
	void InputUpdate();
	bool GetButton(int id, UCHAR key);
	bool GetButtonDown(int id, UCHAR key);
	bool GetButtonUp(int id, UCHAR key);
	float GetAxis(int id, UCHAR axis);
	void Destroy();

private:
	bool initflag;
	DSDevice* dsDevice[4];
};
