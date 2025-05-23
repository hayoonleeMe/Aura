// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "EquippedSpellGlobe.generated.h"

struct FGameplayAbilitySpec;
class UTextBlock;
struct FSpellInfo;
class UImage;

/**
 * 
 */
UCLASS()
class AURA_API UEquippedSpellGlobe : public UUserWidget
{
	GENERATED_BODY()

public:
	UEquippedSpellGlobe(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UImage> Image_SpellIcon;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UImage> Image_Background;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Cooldown;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_StackCount;

	UPROPERTY(EditDefaultsOnly)
	FLinearColor NormalColor;

	UPROPERTY(EditDefaultsOnly)
	FLinearColor CooldownColor;

	// 위젯의 색상을 변경한다.
	// Text_StackCount는 제외한다.
	void SetColor(const FLinearColor& Color);

	FGameplayTag InputTag;

	void UpdateEquippedSpellChange(bool bEquipped, const FGameplayAbilitySpec* SpellSpec, const FSpellInfo& SpellInfo);
	void UpdateEquippedSpellCooldown(float Cooldown);
	void UpdateEquippedSpellCooldownStart(float Cooldown);
	void UpdateEquippedSpellCooldownEnd();
	void UpdateEquippedSpellStackCount(int32 StackCount);

	FTimerHandle CooldownTimerHandle;
	void UpdateTextCooldown();

	UPROPERTY(EditDefaultsOnly)
	float CooldownUpdateInterval;
	
	float RemainingTime = 0.f;

private:
	FNumberFormattingOptions NumberFormattingOptions;
};
