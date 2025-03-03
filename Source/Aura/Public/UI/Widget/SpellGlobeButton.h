// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "SpellGlobeButton.generated.h"

struct FSpellInfo;
class UImage;
class UButton;
class USpellGlobeButton;

// 해당 SpellGlobeButton이 선택됐음을 알리는 델레게이트
DECLARE_DELEGATE_OneParam(FOnSpellGlobeButtonSelectedSignature, USpellGlobeButton* /*SelectedSpellGlobeButton*/);

/**
 * 
 */
UCLASS()
class AURA_API USpellGlobeButton : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Selection;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Icon;
	
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UImage> Image_Background;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Ring;

	UFUNCTION()
	void OnRingButtonClicked();

	UPROPERTY(Transient, meta=(BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> SelectAnimation;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundBase> SelectSound;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundBase> DeselectSound;
	
	void Select();
	void Deselect(bool bPlayDeselectSound);

	FOnSpellGlobeButtonSelectedSignature OnSpellGlobeButtonSelectedDelegate;
	
	FGameplayTag SpellTag;
	bool bSelected = false;

	void UpdateSpellChange(const FSpellInfo& SpellInfo) const;
};
