#include "pch.h"


#include "DSInput_dll.h"

static DSManager* manager = 0;

EXPORT_API bool StartDS4()
{
	if (manager) {
		manager->Destroy();
		delete manager;
		manager = 0;
	}
	manager = new DSManager();
	if (!manager) {
		return false;
	}
	else {
		return true;
	}
}

EXPORT_API bool GetController()
{
	if (manager) {
		bool bsuccess;
		bsuccess = manager->GetDevice();
		return bsuccess;
	}
	else {
		return false;
	}
}

EXPORT_API bool ControllerCheck(int id)
{
	if (manager) {
		return manager->IsDevice(id);
	}
	else {
		return false;
	}
}

EXPORT_API bool ChangeColor(int id, const UCHAR r, const UCHAR g, const UCHAR b)
{
	if (manager) {
		manager->ChangeColor(id, r, g, b);
		return true;
	}
	else {
		return false;
	}
}

bool ChangePlayerLight(int id, const UCHAR val)
{
	if (manager) {
		manager->ChangePlayerLight(id, val);
		return true;
	}
	else {
		return false;
	}
}

EXPORT_API void ChangeVibration(int id, UCHAR right, UCHAR left)
{
	if (manager) {
		manager->ChangeVibration(id, right, left);
	}
}

EXPORT_API void ChangeTriggerLock(int id, UCHAR rMode, UCHAR right, UCHAR lMode, UCHAR left)
{
	if (manager) {
		manager->ChangeTriggerLock(id, rMode, right, lMode, left);
	}
}

EXPORT_API void SendOutput(int id)
{
	if (manager) {
		manager->SendOutput(id);
	}
}

EXPORT_API bool UpdateInputReport()
{
	if (manager) {
		manager->InputUpdate();
		return true;
	}
	else {
		return false;
	}
}

EXPORT_API bool GetButton(int id, UCHAR keyType)
{
	if (manager) {
		return manager->GetButton(id, keyType);
	}
	else {
		return false;
	}
}

EXPORT_API bool GetButtonDown(int id, UCHAR keyType)
{
	if (manager) {
		return manager->GetButtonDown(id, keyType);
	}
	else {
		return false;
	}
}

EXPORT_API bool GetButtonUp(int id, UCHAR keyType)
{
	if (manager) {
		return manager->GetButtonUp(id, keyType);
	}
	else {
		return false;
	}
}

EXPORT_API float GetAxis(int id, UCHAR keyType)
{
	if (manager) {
		return manager->GetAxis(id, keyType);
	}
	else {
		return false;
	}
}

EXPORT_API bool EndDS4()
{
	if (manager) {
		manager->Destroy();
		delete manager;
		manager = 0;
	}
	return true;
}
