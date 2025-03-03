// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/SpellGlobeButton.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Data/SpellConfig.h"

void USpellGlobeButton::NativeConstruct()
{
	Super::NativeConstruct();

	check(SelectSound);
	check(DeselectSound);

	Button_Ring->OnClicked.AddDynamic(this, &ThisClass::OnRingButtonClicked);
}

void USpellGlobeButton::OnRingButtonClicked()
{
	if (!bSelected)
	{
		Select();
	}
	else
	{
		Deselect(true);
	}

	if (OnSpellGlobeButtonSelectedDelegate.IsBound())
	{
		OnSpellGlobeButtonSelectedDelegate.Execute(this);
	}
}

void USpellGlobeButton::Select()
{
	bSelected = true;
	
	PlayAnimationForward(SelectAnimation);
	PlaySound(SelectSound);
}

void USpellGlobeButton::Deselect(bool bPlayDeselectSound)
{
	bSelected = false;
	
	Image_Selection->SetRenderOpacity(0.f);
	if (bPlayDeselectSound)
	{
		PlaySound(DeselectSound);
	}
}

void USpellGlobeButton::UpdateSpellChange(const FSpellInfo& SpellInfo) const
{
	Image_Icon->SetBrushFromTexture(SpellInfo.SpellIcon);
	Image_Background->SetBrushFromMaterial(SpellInfo.Background);
}
