// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/TextValueButtonRow.h"

#include "AuraBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Components/Button.h"
#include "UI/Widget/SquareButton.h"

void UTextValueButtonRow::NativeConstruct()
{
	Super::NativeConstruct();

	Button_Upgrade->InternalButton->OnClicked.AddDynamic(this, &ThisClass::OnUpgradeButtonClicked);
}

void UTextValueButtonRow::OnUpgradeButtonClicked()
{
	UAuraAbilitySystemComponent* AuraASC = UAuraBlueprintLibrary::GetAuraAbilitySystemComponentChecked(GetOwningPlayer());
	AuraASC->ServerUpgradeAttribute(AttributeTag);
}
