// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/AttributeMenu.h"

#include "AuraBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Data/AttributeConfig.h"
#include "Game/AuraGameStateBase.h"
#include "Player/AuraPlayerState.h"
#include "UI/Widget/AttributePointsRow.h"
#include "UI/Widget/FramedValue.h"
#include "UI/Widget/SquareButton.h"
#include "UI/Widget/TextValueButtonRow.h"

UAttributeMenu::UAttributeMenu(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsFocusable(true);	
	bUseUIMapping = true;
}

void UAttributeMenu::NativeConstruct()
{
	Super::NativeConstruct();

	const AAuraGameStateBase* AuraGameStateBase = UAuraBlueprintLibrary::GetAuraGameStateBaseChecked(GetWorld());
	AttributeConfig = AuraGameStateBase->AttributeConfig;
	check(AttributeConfig);
	
	Button_Close->InternalButton->OnClicked.AddDynamic(this, &ThisClass::OnCloseButtonClicked);

	Row_Strength->AttributeTag = AuraGameplayTags::Attributes_Primary_Strength;
	Row_Intelligence->AttributeTag = AuraGameplayTags::Attributes_Primary_Intelligence;
	Row_Resilience->AttributeTag = AuraGameplayTags::Attributes_Primary_Resilience;
	Row_Vigor->AttributeTag = AuraGameplayTags::Attributes_Primary_Vigor;

	AttributeTagToRowMap.Add(Row_Strength->AttributeTag, Row_Strength);
	AttributeTagToRowMap.Add(Row_Intelligence->AttributeTag, Row_Intelligence);
	AttributeTagToRowMap.Add(Row_Resilience->AttributeTag, Row_Resilience);
	AttributeTagToRowMap.Add(Row_Vigor->AttributeTag, Row_Vigor);
	
	Row_Armor->AttributeTag = AuraGameplayTags::Attributes_Secondary_Armor;
	Row_ArmorPenetration->AttributeTag = AuraGameplayTags::Attributes_Secondary_ArmorPenetration;
	Row_BlockChance->AttributeTag = AuraGameplayTags::Attributes_Secondary_BlockChance;
	Row_CriticalHitChance->AttributeTag = AuraGameplayTags::Attributes_Secondary_CriticalHitChance;
	Row_CriticalHitDamage->AttributeTag = AuraGameplayTags::Attributes_Secondary_CriticalHitDamage;
	Row_CriticalHitResistance->AttributeTag = AuraGameplayTags::Attributes_Secondary_CriticalHitResistance;
	Row_HealthRegeneration->AttributeTag = AuraGameplayTags::Attributes_Secondary_HealthRegeneration;
	Row_ManaRegeneration->AttributeTag = AuraGameplayTags::Attributes_Secondary_ManaRegeneration;
	Row_MaxHealth->AttributeTag = AuraGameplayTags::Attributes_Secondary_MaxHealth;
	Row_MaxMana->AttributeTag = AuraGameplayTags::Attributes_Secondary_MaxMana;
	Row_FireResistance->AttributeTag = AuraGameplayTags::Attributes_Secondary_FireResistance;
	Row_LightningResistance->AttributeTag = AuraGameplayTags::Attributes_Secondary_LightningResistance;
	Row_ArcaneResistance->AttributeTag = AuraGameplayTags::Attributes_Secondary_ArcaneResistance;
	Row_PhysicalResistance->AttributeTag = AuraGameplayTags::Attributes_Secondary_PhysicalResistance;

	AttributeTagToRowMap.Add(Row_Armor->AttributeTag, Row_Armor);
	AttributeTagToRowMap.Add(Row_ArmorPenetration->AttributeTag, Row_ArmorPenetration);
	AttributeTagToRowMap.Add(Row_BlockChance->AttributeTag, Row_BlockChance);
	AttributeTagToRowMap.Add(Row_CriticalHitChance->AttributeTag, Row_CriticalHitChance);
	AttributeTagToRowMap.Add(Row_CriticalHitDamage->AttributeTag, Row_CriticalHitDamage);
	AttributeTagToRowMap.Add(Row_CriticalHitResistance->AttributeTag, Row_CriticalHitResistance);
	AttributeTagToRowMap.Add(Row_HealthRegeneration->AttributeTag, Row_HealthRegeneration);
	AttributeTagToRowMap.Add(Row_ManaRegeneration->AttributeTag, Row_ManaRegeneration);
	AttributeTagToRowMap.Add(Row_MaxHealth->AttributeTag, Row_MaxHealth);
	AttributeTagToRowMap.Add(Row_MaxMana->AttributeTag, Row_MaxMana);
	AttributeTagToRowMap.Add(Row_FireResistance->AttributeTag, Row_FireResistance);
	AttributeTagToRowMap.Add(Row_LightningResistance->AttributeTag, Row_LightningResistance);
	AttributeTagToRowMap.Add(Row_ArcaneResistance->AttributeTag, Row_ArcaneResistance);
	AttributeTagToRowMap.Add(Row_PhysicalResistance->AttributeTag, Row_PhysicalResistance);

	// AttributePoints 값 전달
	const AAuraPlayerState* AuraPS = GetOwningPlayerState<AAuraPlayerState>(true);
	UpdateAttributePointsChange(AuraPS->GetAttributePoints());

	// Attribute Row의 값을 업데이트
	const UAuraAttributeSet* AuraAS = UAuraBlueprintLibrary::GetAuraAttributeSetChecked(GetOwningPlayer());
	for (const TTuple<FGameplayTag, FGameplayAttribute>& Tuple : AuraAS->TagToAttributeMap)
	{
		UpdateAttributeValueChange(Tuple.Key, Tuple.Value.GetNumericValue(AuraAS));
	}
}

void UAttributeMenu::UpdateAttributePointsChange(int32 Value) const
{
	Row_AttributePoints->FramedValue->Text_Value->SetText(FText::AsNumber(Value));
	
	// Enable Attribute Row Buttons
	if (Value > 0)
	{
		Row_Strength->Button_Upgrade->InternalButton->SetIsEnabled(true);
		Row_Intelligence->Button_Upgrade->InternalButton->SetIsEnabled(true);
		Row_Resilience->Button_Upgrade->InternalButton->SetIsEnabled(true);
		Row_Vigor->Button_Upgrade->InternalButton->SetIsEnabled(true);
	}
}

void UAttributeMenu::UpdateAttributeValueChange(const FGameplayTag& AttributeTag, float Value)
{
	FAttributeInfo AttributeInfo = AttributeConfig->GetAttributeInfoByTag(AttributeTag);
	AttributeInfo.Value = Value;
	UTextValueRow* Row = AttributeTagToRowMap.FindChecked(AttributeTag);
	Row->UpdateAttributeValueChange(AttributeInfo);
}

void UAttributeMenu::OnCloseButtonClicked()
{
	RemoveFromParent();
}
