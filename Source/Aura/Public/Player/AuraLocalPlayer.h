// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LocalPlayer.h"
#include "AuraLocalPlayer.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraLocalPlayer : public ULocalPlayer
{
	GENERATED_BODY()

public:
	virtual bool CalcSceneViewInitOptions(FSceneViewInitOptions& OutInitOptions, FViewport* Viewport, FViewElementDrawer* ViewDrawer, int32 StereoViewIndex) override;

	// 화면을 까맣게 오버라이드할 지 설정
	void SetSceneViewColorBlack(bool bInBlack) { bBlack = bInBlack; }
	
private:
	// true라면, 화면(Scene View)을 까맣게 오버라이드한다. (위젯 제외)
	// 처음부터 화면을 까맣게 가리고, 첫번째 SpawnBeacon Level Sequence가 끝나면 false로 설정
	bool bBlack = true;
};
