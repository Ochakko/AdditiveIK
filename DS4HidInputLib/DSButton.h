#pragma once
#include "DSButtonStatus.h"

typedef int BOOL;

struct DSButton
{
	DSButtonStatus status;

	DSButton() ;
	void ChangeStatus(BOOL isButtonDown) ;
};