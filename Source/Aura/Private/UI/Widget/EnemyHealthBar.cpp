// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/EnemyHealthBar.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Components/ProgressBar.h"
#include "Interface/CombatInterface.h"
#include "Kismet/KismetMathLibrary.h"

void UEnemyHealthBar::InitializeHealthBar(UAbilitySystemComponent* ASC, const UAttributeSet* AS)
{
	UAuraAbilitySystemComponent* AuraASC = CastChecked<UAuraAbilitySystemComponent>(ASC);
	const UAuraAttributeSet* AuraAS = CastChecked<UAuraAttributeSet>(AS);
	
	AuraASC->GetGameplayAttributeValueChangeDelegate(UAuraAttributeSet::GetHealthAttribute()).AddWeakLambda(this, [this](const FOnAttributeChangeData& Data)
	{
		UpdateHealth(Data.NewValue);
	});
	AuraASC->GetGameplayAttributeValueChangeDelegate(UAuraAttributeSet::GetMaxHealthAttribute()).AddWeakLambda(this, [this](const FOnAttributeChangeData& Data)
	{
		UpdateMaxHealth(Data.NewValue);
	});

	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(AuraASC->GetAvatarActor_Direct()))
	{
		CombatInterface->GetOnCharacterDeadDelegate()->AddDynamic(this, &ThisClass::OnOwnerDead);
	}

	UpdateHealth(AuraAS->GetHealth());
	UpdateMaxHealth(AuraAS->GetMaxHealth());
}

void UEnemyHealthBar::UpdateHealth(float NewValue)
{
	Health = NewValue;
	ProgressBar->SetPercent(UKismetMathLibrary::SafeDivide(Health, MaxHealth));
}

void UEnemyHealthBar::UpdateMaxHealth(float NewValue)
{
	MaxHealth = NewValue;
	ProgressBar->SetPercent(UKismetMathLibrary::SafeDivide(Health, MaxHealth));
}

void UEnemyHealthBar::OnOwnerDead()
{
	SetVisibility(ESlateVisibility::Collapsed);
}
