// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/EquippedSpellGlobeButton.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Data/SpellConfig.h"

void UEquippedSpellGlobeButton::NativeConstruct()
{
	Super::NativeConstruct();

	Button_Ring->OnClicked.AddDynamic(this, &ThisClass::OnRingButtonClicked);
}

void UEquippedSpellGlobeButton::OnRingButtonClicked()
{
	if (OnEquippedSpellGlobeButtonSelectedDelegate.IsBound())
	{
		OnEquippedSpellGlobeButtonSelectedDelegate.Execute(InputTag);
	}
}

void UEquippedSpellGlobeButton::UpdateEquippedSpellChange(bool bEquipped, const FSpellInfo& SpellInfo) const
{
	if (bEquipped)
	{
		Image_Icon->SetBrushFromTexture(SpellInfo.SpellIcon);
		Image_Background->SetBrushFromMaterial(SpellInfo.Background);
		Image_Icon->SetVisibility(ESlateVisibility::Visible);
		Image_Background->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		Image_Icon->SetVisibility(ESlateVisibility::Collapsed);
		Image_Background->SetVisibility(ESlateVisibility::Collapsed);
	}
}
