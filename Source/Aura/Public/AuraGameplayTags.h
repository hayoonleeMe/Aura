// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NativeGameplayTags.h"

namespace AuraGameplayTags
{
	// ============================================================================
	// Activate Fail
	// 어빌리티 실행이 실패한 이유를 나타내는 태그
	// ============================================================================

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Activate_Fail_Cost);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Activate_Fail_Cooldown);

	// ============================================================================
	// Input
	// ============================================================================

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_LMB);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Interact);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_RMB);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_MMB);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Q);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_W);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_E);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_R);

	// ============================================================================
	// Passive Input
	// 실제 트리거하는 Key는 아니지만, UI에 Passive Spell을 나타내기 위한 Tag
	// ============================================================================

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Passive_1);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Passive_2);

	// ============================================================================
	// Primary Attributes
	// ============================================================================

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Primary);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Primary_Strength);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Primary_Intelligence);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Primary_Resilience);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Primary_Vigor);

	// ============================================================================
	// Secondary Attributes
	// ============================================================================

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_Armor);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_ArmorPenetration);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_BlockChance);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_CriticalHitChance);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_CriticalHitDamage);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_CriticalHitResistance);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_HealthRegeneration);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_ManaRegeneration);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_MaxHealth);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_MaxMana);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_FireResistance);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_LightningResistance);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_ArcaneResistance);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_PhysicalResistance);

	// ============================================================================
	// Vital Attributes
	// ============================================================================

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Vital_Health);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Vital_Mana);

	// ============================================================================
	// Meta Attributes
	// ============================================================================

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Meta_Level);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Meta_XP);

	// ============================================================================
	// Damage Types
	// ============================================================================

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Damage_Type_Fire);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Damage_Type_Lightning);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Damage_Type_Arcane);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Damage_Type_Physical);

	extern const TMap<FGameplayTag, FGameplayTag> DamageTypeToResistanceMap;

	// ============================================================================
	// Role
	// ============================================================================

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Role_Player);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Role_Enemy);

	// ============================================================================
	// Event
	// ============================================================================

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Montage_FireBolt);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Montage_ChainLightning);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Montage_ArcaneSpike);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Montage_EnemyAttack);
	
	// ============================================================================
	// Abilities
	// ============================================================================
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Abilities_ClickToMove);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Abilities_TryInteract);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Abilities_HitReact);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Abilities_EnemyAttack);

	// ============================================================================
	// Abilities.Offensive
	// 플레이어가 Damage를 입히기 위해 직접 사용하는 Offensive Ability
	// ============================================================================

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Abilities_Offensive);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Abilities_Offensive_FireBolt);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Abilities_Offensive_ChainLightning);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Abilities_Offensive_ArcaneSpike);

	// ============================================================================
	// Abilities.Passive
	// 장착하는 동안 계속해서 적용되는 Passive Ability
	// ============================================================================

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Abilities_Passive);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Abilities_Passive_HaloOfProtection);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Abilities_Passive_HealthSiphon);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Abilities_Passive_ManaSiphon);

	// ============================================================================
	// Debuff
	// ============================================================================

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Debuff_Ignite);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Debuff_Enfeeble);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Debuff_Stun);

	// ============================================================================
	// Cooldown
	// ============================================================================

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cooldown_ArcaneSpike);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cooldown_Melee_GoblinSpear);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cooldown_Melee_DemonWarrior);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cooldown_Melee_Ghoul);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cooldown_Range_GoblinSlingshot);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cooldown_Range_DemonRanger);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cooldown_Range_Shaman);

	// ============================================================================
	// GameplayCue
	// ============================================================================

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_FireBoltImpact);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_EnemyMeleeImpact);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_RockImpact);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_LevelUp);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_LightningBeam);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_ArcaneShard);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_SummonEffect);

	// ============================================================================
	// Gameplay
	// ============================================================================

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Gameplay_Invincibility);
}
