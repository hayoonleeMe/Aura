// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/EquippedSpellGlobe.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Data/SpellConfig.h"

void UEquippedSpellGlobe::NativeConstruct()
{
	Super::NativeConstruct();

	NormalColor = FLinearColor::White;
	CooldownColor = FLinearColor(0.25f, 0.25f, 0.25f);
	CooldownUpdateInterval = 0.05f;
}

void UEquippedSpellGlobe::UpdateEquippedSpellChange(bool bEquipped, const FSpellInfo& SpellInfo) const
{
	if (bEquipped)
	{
		Image_SpellIcon->SetBrushFromTexture(SpellInfo.SpellIcon);
		Image_SpellIcon->SetVisibility(ESlateVisibility::Visible);
		Image_Background->SetBrushFromMaterial(SpellInfo.Background);
		Image_Background->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		// ClearGlobe
		Image_SpellIcon->SetVisibility(ESlateVisibility::Collapsed);
		Image_Background->SetVisibility(ESlateVisibility::Collapsed);
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
	SetColorAndOpacity(CooldownColor);
}

void UEquippedSpellGlobe::UpdateEquippedSpellCooldownEnd()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(CooldownTimerHandle);
	}

	Text_Cooldown->SetVisibility(ESlateVisibility::Collapsed);
	SetColorAndOpacity(NormalColor);
}

void UEquippedSpellGlobe::UpdateTextCooldown()
{
	RemainingTime = FMath::Max(0.f, RemainingTime - CooldownUpdateInterval);
	
	FNumberFormattingOptions Options;
	Options.MinimumFractionalDigits = 1;
	Options.MaximumFractionalDigits = 1;
	Text_Cooldown->SetText(FText::AsNumber(RemainingTime, &Options));
}
