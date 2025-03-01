// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/HealthGlobe.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Kismet/KismetMathLibrary.h"

void UHealthGlobe::NativeConstruct()
{
	Super::NativeConstruct();

	UAuraAbilitySystemComponent* AuraASC = GetOwnerAuraAbilitySystemComponentChecked();
	AuraASC->GetGameplayAttributeValueChangeDelegate(UAuraAttributeSet::GetHealthAttribute()).AddWeakLambda(this, [this](const FOnAttributeChangeData& Data)
	{
		UpdateHealth(Data.NewValue);
	});
	AuraASC->GetGameplayAttributeValueChangeDelegate(UAuraAttributeSet::GetMaxHealthAttribute()).AddWeakLambda(this, [this](const FOnAttributeChangeData& Data)
	{
		UpdateMaxHealth(Data.NewValue);
	});

	BroadcastInitialValues();
}

void UHealthGlobe::BroadcastInitialValues()
{
	const UAuraAttributeSet* AuraAS = GetOwnerAuraAttributeSetChecked();
	UpdateHealth(AuraAS->GetHealth());
	UpdateMaxHealth(AuraAS->GetMaxHealth());
}

void UHealthGlobe::UpdateHealth(float NewValue)
{
	Health = NewValue;
	TargetPercent = UKismetMathLibrary::SafeDivide(Health, MaxHealth);
}

void UHealthGlobe::UpdateMaxHealth(float NewValue)
{
	MaxHealth = NewValue;
	TargetPercent = UKismetMathLibrary::SafeDivide(Health, MaxHealth);
}
