#include "pch.h"

#include "DSButton.h"

DSButton::DSButton()  : status()
{
}

void DSButton::ChangeStatus(BOOL isButtonDown) 
{
	if (isButtonDown)
	{
		if (status == DSButtonStatus::None)
		{
			status = DSButtonStatus::Push;
			return;
		}

		if (status == DSButtonStatus::Push)
		{
			status = DSButtonStatus::Pushing;
			return;
		}

		if (status == DSButtonStatus::UnPush)
		{
			status = DSButtonStatus::Push;
		}
		return;
	}

	if ( !isButtonDown)
	{
		if (status == DSButtonStatus::Push)
		{
			status = DSButtonStatus::UnPush;
			return;
		}

		if (status == DSButtonStatus::Pushing)
		{
			status = DSButtonStatus::UnPush;
			return;
		}

		if (status == DSButtonStatus::UnPush)
		{
			status = DSButtonStatus::None;
		}
		return;
	}
}