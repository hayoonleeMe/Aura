// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/OffensiveSpellTree.h"

#include "AuraGameplayTags.h"
#include "UI/Widget/SpellGlobeButton.h"

void UOffensiveSpellTree::NativeConstruct()
{
	Super::NativeConstruct();

	GlobeButton_1->SpellTag = AuraGameplayTags::Abilities_Offensive_Firenado;
	GlobeButton_2->SpellTag = AuraGameplayTags::Abilities_Offensive_FireBall;
	GlobeButton_3->SpellTag = AuraGameplayTags::Abilities_Offensive_FireBolt;
	GlobeButton_4->SpellTag = AuraGameplayTags::Abilities_Offensive;
	GlobeButton_5->SpellTag = AuraGameplayTags::Abilities_Offensive;
	GlobeButton_6->SpellTag = AuraGameplayTags::Abilities_Offensive_ChainLightning;
	GlobeButton_7->SpellTag = AuraGameplayTags::Abilities_Offensive;
	GlobeButton_8->SpellTag = AuraGameplayTags::Abilities_Offensive;
	GlobeButton_9->SpellTag = AuraGameplayTags::Abilities_Offensive_ArcaneSpike;
}
