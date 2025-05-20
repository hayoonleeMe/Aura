// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RangeDecalActor.generated.h"

/**
 * @todo
 * 현재 TSR AA Method를 사용하는데 TSR에서 Translucent Decal에 대한 문제가 있어서 그 대체로 단순한 Circle을 나타내는 M_RangeDecal Material을 사용한다. 
 * Decal로 사용하고 싶은 Material은 Deferred Decal Domain + Translucent Blend Mode인 M_MagicCircle_1 Material이지만 보류한다.
 * TSR을 사용하면 위 같은 Translucent Decal을 움직이면 떨리는 현상과 약간의 잔상이 남는 Ghosting 현상이 발생하는 것으로 보인다.
 * 이를 해결하기 위해 r.TSR.ShadingRejection.Flickering과 r.TSR.ShadingRejection.Flickering.Period를 모두 0으로 사용해
 * 약간이나마 해결했지만 여전히 문제가 조금씩 남아있다.
 * Anti Aliasing Method로 FXAA, MSAA를 사용하면 Decal을 움직여도 문제가 발생하지 않지만 렌더링 품질이 별로라서 TSR을 포기하지 못했다.
 * 추가로 5.4+ 에는 Has Pixel Animation 이라는 옵션이 추가됐다는데, 이를 사용하면 될지도 모르겠다.
 */

/**
 * @todo
 * UAuraAbility_Firenado에 한정되지 않고 공용으로 사용할 수 있도록 개선
 */

class UAuraAbility_Firenado;
class AAuraPlayerController;

/**
 * 마우스 커서 위치에 스펠의 범위를 나타낸다.
 */
UCLASS()
class AURA_API ARangeDecalActor : public AActor
{
	GENERATED_BODY()

public:
	ARangeDecalActor();
	virtual void Tick(float DeltaTime) override;

	void Initialize(UAuraAbility_Firenado* InOwningAbility);

	// bShow가 true면 Radius만큼 범위를 표시하고, false면 화면에서 숨긴다. 
	void ShowRangeDecal(bool bShow, float Radius = 0.f);

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UDecalComponent> RangeDecalComponent;

	UPROPERTY()
	TObjectPtr<UAuraAbility_Firenado> OwningAbility;

	UPROPERTY()
	TObjectPtr<AAuraPlayerController> AuraPC;
};
