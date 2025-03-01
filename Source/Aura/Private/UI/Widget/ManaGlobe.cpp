// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/ManaGlobe.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Kismet/KismetMathLibrary.h"

void UManaGlobe::NativeConstruct()
{
	Super::NativeConstruct();

	UAuraAbilitySystemComponent* AuraASC = GetOwnerAuraAbilitySystemComponentChecked();
	AuraASC->GetGameplayAttributeValueChangeDelegate(UAuraAttributeSet::GetManaAttribute()).AddWeakLambda(this, [this](const FOnAttributeChangeData& Data)
	{
		UpdateMana(Data.NewValue);
	});
	AuraASC->GetGameplayAttributeValueChangeDelegate(UAuraAttributeSet::GetMaxManaAttribute()).AddWeakLambda(this, [this](const FOnAttributeChangeData& Data)
	{
		UpdateMaxMana(Data.NewValue);
	});

	BroadcastInitialValues();
}

void UManaGlobe::BroadcastInitialValues()
{
	const UAuraAttributeSet* AuraAS = GetOwnerAuraAttributeSetChecked();
	UpdateMana(AuraAS->GetMana());
	UpdateMaxMana(AuraAS->GetMaxMana());
}

void UManaGlobe::UpdateMana(float NewValue)
{
	Mana = NewValue;
	TargetPercent = UKismetMathLibrary::SafeDivide(Mana, MaxMana);
}

void UManaGlobe::UpdateMaxMana(float NewValue)
{
	MaxMana = NewValue;
	TargetPercent = UKismetMathLibrary::SafeDivide(Mana, MaxMana);
}
