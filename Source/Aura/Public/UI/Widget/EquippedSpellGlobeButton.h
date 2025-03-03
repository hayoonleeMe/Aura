// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "EquippedSpellGlobeButton.generated.h"

struct FSpellInfo;
class UButton;
class UImage;

// 해당 EquippedSpellGlobeButton이 선택됐음을 알리는 델레게이트
DECLARE_DELEGATE_OneParam(FOnEquippedSpellGlobeButtonSelectedSignature, const FGameplayTag& /*InputTag*/);

/**
 * 
 */
UCLASS()
class AURA_API UEquippedSpellGlobeButton : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UImage> Image_Icon;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UImage> Image_Background;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UButton> Button_Ring;

	UFUNCTION()
	void OnRingButtonClicked();

	FGameplayTag InputTag;

	FOnEquippedSpellGlobeButtonSelectedSignature OnEquippedSpellGlobeButtonSelectedDelegate;

	void UpdateEquippedSpellChange(bool bEquipped, const FSpellInfo& SpellInfo) const;
};
