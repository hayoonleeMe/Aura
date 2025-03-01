// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/PassiveSpellTree.h"

#include "AuraGameplayTags.h"
#include "UI/Widget/SpellGlobeButton.h"

void UPassiveSpellTree::NativeConstruct()
{
	Super::NativeConstruct();

	GlobeButton_HaloOfProtection->SpellTag = AuraGameplayTags::Abilities_Passive_HaloOfProtection;
	GlobeButton_HealthSiphon->SpellTag = AuraGameplayTags::Abilities_Passive_HealthSiphon;
	GlobeButton_ManaSiphon->SpellTag = AuraGameplayTags::Abilities_Passive_ManaSiphon;
}
