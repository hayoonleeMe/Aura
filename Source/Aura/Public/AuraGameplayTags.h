// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

/**
 *	Singleton containing native gameplay tags
 */
struct FAuraGameplayTags
{
	static FAuraGameplayTags& Get() { return GameplayTags; }
	static void InitializeNativeGameplayTags();

	/*
	 *	Input
	 */
	FGameplayTag InputTag_LMB;
	FGameplayTag InputTag_RMB;
	FGameplayTag InputTag_1;
	FGameplayTag InputTag_2;
	FGameplayTag InputTag_3;
	FGameplayTag InputTag_4;

	/*
	 *	Primary Attributes
	 */
	FGameplayTag Attributes_Primary_Strength;
	FGameplayTag Attributes_Primary_Intelligence;
	FGameplayTag Attributes_Primary_Resilience;
	FGameplayTag Attributes_Primary_Vigor;

	/*
	 *	Secondary Attributes
	 */
	FGameplayTag Attributes_Secondary_Armor;
	FGameplayTag Attributes_Secondary_ArmorPenetration;
	FGameplayTag Attributes_Secondary_BlockChance;
	FGameplayTag Attributes_Secondary_CriticalHitChance;
	FGameplayTag Attributes_Secondary_CriticalHitDamage;
	FGameplayTag Attributes_Secondary_CriticalHitResistance;
	FGameplayTag Attributes_Secondary_HealthRegeneration;
	FGameplayTag Attributes_Secondary_ManaRegeneration;
	FGameplayTag Attributes_Secondary_MaxHealth;
	FGameplayTag Attributes_Secondary_MaxMana;
	FGameplayTag Attributes_Secondary_FireResistance;
	FGameplayTag Attributes_Secondary_LightningResistance;
	FGameplayTag Attributes_Secondary_ArcaneResistance;
	FGameplayTag Attributes_Secondary_PhysicalResistance;

	/*
	 *	Vital Attributes
	 */
	FGameplayTag Attributes_Vital_Health;
	FGameplayTag Attributes_Vital_Mana;

	/*
	 *	Damage Types
	 */
	FGameplayTag Damage_Type_Fire;
	FGameplayTag Damage_Type_Lightning;
	FGameplayTag Damage_Type_Arcane;
	FGameplayTag Damage_Type_Physical;

	TMap<FGameplayTag, FGameplayTag> DamageTypeToResistanceMap;

	/*
	 *	Role
	 */
	FGameplayTag Role_Player;
	FGameplayTag Role_Enemy;

	/*
	 *	Event
	 */
	FGameplayTag Event_Montage_FireBolt;

	/*
	 *	Abilities
	 */
	FGameplayTag Abilities_ClickToMove;
	FGameplayTag Abilities_FireBolt;
	FGameplayTag Abilities_HitReact;

	/*
	 *	GameplayCue
	 */
	FGameplayTag GameplayCue_FireBoltImpact;
	
private:
	static FAuraGameplayTags GameplayTags;
};
