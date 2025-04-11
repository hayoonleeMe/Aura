// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AuraLocalPlayer.h"

bool UAuraLocalPlayer::CalcSceneViewInitOptions(FSceneViewInitOptions& OutInitOptions, FViewport* Viewport, FViewElementDrawer* ViewDrawer, int32 StereoViewIndex)
{
	const bool bReturn = Super::CalcSceneViewInitOptions(OutInitOptions, Viewport, ViewDrawer, StereoViewIndex);

	if (bBlack)
	{
		OutInitOptions.OverlayColor = FLinearColor::Black;
		OutInitOptions.OverlayColor.A = 1.f;
	}

	return bReturn;
}
