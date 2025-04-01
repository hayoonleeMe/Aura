// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "EquippedSpellGlobe.generated.h"

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

	UPROPERTY(EditDefaultsOnly)
	FLinearColor NormalColor;

	UPROPERTY(EditDefaultsOnly)
	FLinearColor CooldownColor;

	FGameplayTag InputTag;

	void UpdateEquippedSpellChange(bool bEquipped, const FSpellInfo& SpellInfo) const;
	void UpdateEquippedSpellCooldownStart(float Cooldown);
	void UpdateEquippedSpellCooldownEnd();

	FTimerHandle CooldownTimerHandle;
	void UpdateTextCooldown();

	UPROPERTY(EditDefaultsOnly)
	float CooldownUpdateInterval;
	
	float RemainingTime = 0.f;

private:
	FNumberFormattingOptions NumberFormattingOptions;
};
