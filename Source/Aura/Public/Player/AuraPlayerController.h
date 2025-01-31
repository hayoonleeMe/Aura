// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "Interaction/PlayerInterface.h"
#include "AuraPlayerController.generated.h"

class UDamageIndicatorComponent;
class USplineComponent;
class UAuraAbilitySystemComponent;
class UInputMappingContext;
class UAuraInputConfig;

/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerController : public APlayerController, public IPlayerInterface
{
	GENERATED_BODY()

public:
	virtual void PlayerTick(float DeltaTime) override;

	/* Begin PlayerInterface */
	virtual FHitResult GetTargetHitResult() const override { return TargetHitResult; }
	/* End PlayerInterface */
	
	// 게임에서 기본으로 사용하는 Input Mode로 설정
	void SetInGameInputMode();
	
	// UI 전용 Input Mode로 설정
	void SetUIInputMode();

	// Damage를 나타내는 DamageIndicator Widget을 화면에 표시한다.
	UFUNCTION(Client, Reliable)
	void ClientIndicateDamage(float Damage, bool bIsBlockedHit, bool bIsCriticalHit, const FVector_NetQuantize& TargetLocation) const;

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	
private:
	// ============================================================================
	// Combat
	// ============================================================================

	void CursorTrace();

	// Cached Target HitResult Under Cursor
	FHitResult TargetHitResult;

	// 이전 프레임에서의 TargetHitResult의 Actor
	TWeakObjectPtr<AActor> TargetFromPrevFrame;

	// 현재 프레임에서의 TargetHitResult의 Actor
	TWeakObjectPtr<AActor> TargetFromCurrentFrame;
	
	// ============================================================================
	// Input
	// ============================================================================

	UPROPERTY(EditAnywhere, Category="Aura|Input")
	TObjectPtr<UInputMappingContext> AuraContext;
	
	UPROPERTY(EditDefaultsOnly, Category="Aura|Input")
	TObjectPtr<UAuraInputConfig> AuraInputConfig;

	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);

	// ============================================================================
	// GAS
	// ============================================================================

	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent;
	
	UAuraAbilitySystemComponent* GetAuraAbilitySystemComponent();

	// ============================================================================
	// Damage Indicator
	// ============================================================================

	// Damage Indicator를 표시하는 데 사용할 WidgetComponent Class
	UPROPERTY(EditDefaultsOnly, Category="Aura|Damage Indicator")
	TSubclassOf<UDamageIndicatorComponent> DamageIndicatorComponentClass;
};
