// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/EquippedSpellGlobe.h"

#include "GameplayAbilitySpec.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Data/SpellConfig.h"

UEquippedSpellGlobe::UEquippedSpellGlobe(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NormalColor = FLinearColor::White;
	CooldownColor = FLinearColor(0.25f, 0.25f, 0.25f);
	CooldownUpdateInterval = 0.05f;

	NumberFormattingOptions.MinimumFractionalDigits = 1;
	NumberFormattingOptions.MaximumFractionalDigits = 1;
}

void UEquippedSpellGlobe::SetColor(const FLinearColor& Color)
{
	Image_Background->SetColorAndOpacity(Color);
	Image_SpellIcon->SetColorAndOpacity(Color);
}

void UEquippedSpellGlobe::UpdateEquippedSpellChange(bool bEquipped, const FGameplayAbilitySpec* SpellSpec, const FSpellInfo& SpellInfo)
{
	UAuraGameplayAbility* AuraAbility = CastChecked<UAuraGameplayAbility>(SpellSpec->GetPrimaryInstance());
	FOnSpellCooldownSignature* OnSpellCooldownDelegate = AuraAbility->GetOnSpellCooldownDelegate();
	FOnSpellStackChangedSignature* OnSpellStackChangedDelegate = AuraAbility->GetOnSpellStackChangedDelegate();
	
	if (bEquipped)
	{
		Image_SpellIcon->SetBrushFromTexture(SpellInfo.SpellIcon);
		Image_SpellIcon->SetVisibility(ESlateVisibility::Visible);
		Image_Background->SetBrushFromMaterial(SpellInfo.Background);
		Image_Background->SetVisibility(ESlateVisibility::Visible);

		// Spell Cooldown
		if (OnSpellCooldownDelegate)
		{
			OnSpellCooldownDelegate->AddUObject(this, &ThisClass::UpdateEquippedSpellCooldown);
			
			if (const float CooldownRemaining = AuraAbility->GetCooldownTimeRemaining(); CooldownRemaining > 0.f)
			{
				// Spell을 장착할 때 이미 쿨다운 중이면 바로 UI에 업데이트
				UpdateEquippedSpellCooldown(CooldownRemaining);
			}
		}
		
		// Spell Stack Count
		if (OnSpellStackChangedDelegate)
		{
			OnSpellStackChangedDelegate->AddUObject(this, &ThisClass::UpdateEquippedSpellStackCount);

			UpdateEquippedSpellStackCount(AuraAbility->GetSpellStackCount());
			Text_StackCount->SetVisibility(ESlateVisibility::Visible);
		}
	}
	else
	{
		Image_SpellIcon->SetVisibility(ESlateVisibility::Collapsed);
		Image_Background->SetVisibility(ESlateVisibility::Collapsed);

		// Spell Cooldown
		if (OnSpellCooldownDelegate)
		{
			OnSpellCooldownDelegate->RemoveAll(this);
		}

		// Spell Stack Count
		if (OnSpellStackChangedDelegate)
		{
			OnSpellStackChangedDelegate->RemoveAll(this);
			Text_StackCount->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UEquippedSpellGlobe::UpdateEquippedSpellCooldown(float Cooldown)
{
	if (Cooldown > 0.f)
	{
		UpdateEquippedSpellCooldownStart(Cooldown);
	}
	else
	{
		UpdateEquippedSpellCooldownEnd();
	}
}

void UEquippedSpellGlobe::UpdateEquippedSpellCooldownStart(float Cooldown)
{
	RemainingTime = Cooldown;
	
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(CooldownTimerHandle, FTimerDelegate::CreateUObject(this, &ThisClass::UpdateTextCooldown), CooldownUpdateInterval, true, 0.f);
	}
	
	Text_Cooldown->SetVisibility(ESlateVisibility::Visible);
	SetColor(CooldownColor);
}

void UEquippedSpellGlobe::UpdateEquippedSpellCooldownEnd()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(CooldownTimerHandle);
	}

	Text_Cooldown->SetVisibility(ESlateVisibility::Collapsed);
	SetColor(NormalColor);

	PlayAnimationForward(CooldownEndAnimation);
}

void UEquippedSpellGlobe::UpdateEquippedSpellStackCount(int32 StackCount)
{
	Text_StackCount->SetText(FText::AsNumber(StackCount));
}

void UEquippedSpellGlobe::UpdateTextCooldown()
{
	RemainingTime = FMath::Max(0.f, RemainingTime - CooldownUpdateInterval);
	
	Text_Cooldown->SetText(FText::AsNumber(RemainingTime, &NumberFormattingOptions));
}
