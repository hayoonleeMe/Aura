// Fill out your copyright notice in the Description page of Project Settings.


#include "AuraGameplayTags.h"

#include "GameplayTagsManager.h"

namespace AuraGameplayTags
{
	/* Activate Fail */
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Activate_Fail_Cost, "Activate.Fail.Cost", "Ability Activate Fail Cost Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Activate_Fail_Cooldown, "Activate.Fail.Cooldown", "Ability Activate Fail Cooldown Tag");
	
	/* Input */
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag, "InputTag", "Input Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_LMB, "InputTag.LMB", "Input Tag for Left Mouse Button");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Interact, "InputTag.Interact", "Input Tag for Interact");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_RMB, "InputTag.RMB", "Input Tag for Right Mouse Button");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_MMB, "InputTag.MMB", "Input Tag for Middle Mouse Button");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Q, "InputTag.Q", "Input Tag for Q key");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_W, "InputTag.W", "Input Tag for W key");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_E, "InputTag.E", "Input Tag for E key");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_R, "InputTag.R", "Input Tag for R key");

	/* Passive Input */
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Passive_1, "InputTag.Passive.1", "Passive Input Tag for first slot");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Passive_2, "InputTag.Passive.2", "Passive Input Tag for second slot");

	/* Primary Attributes */
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Primary_Strength, "Attributes.Primary.Strength", "Increases physical damage");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Primary_Intelligence, "Attributes.Primary.Intelligence", "Increases magical damage");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Primary_Resilience, "Attributes.Primary.Resilience", "Increases Armor and Armor Penetration");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Primary_Vigor, "Attributes.Primary.Vigor", "Increases Health");
	
	/* Secondary Attributes */
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_Armor, "Attributes.Secondary.Armor", "Reduces damage taken, improves Block Chance");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_ArmorPenetration, "Attributes.Secondary.ArmorPenetration", "Ignored Percentage of enemy Armor, increases Critical Hit Chance");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_BlockChance, "Attributes.Secondary.BlockChance", "Chance to cut incoming damage in half");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_CriticalHitChance, "Attributes.Secondary.CriticalHitChance", "Chance to double damage plus critical hit bonus");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_CriticalHitDamage, "Attributes.Secondary.CriticalHitDamage", "Bonus damage added when a critical hit is scored");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_CriticalHitResistance, "Attributes.Secondary.CriticalHitResistance", "Reduces Critical Hit Chance of attacking enemies");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_HealthRegeneration, "Attributes.Secondary.HealthRegeneration", "Amount of Health regenerated every 1 second");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_ManaRegeneration, "Attributes.Secondary.ManaRegeneration", "Amount of Mana regenerated every 1 second");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_MaxHealth, "Attributes.Secondary.MaxHealth", "Maximum amount of Health obtainable");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_MaxMana, "Attributes.Secondary.MaxMana", "Maximum amount of Mana obtainable");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_FireResistance, "Attributes.Secondary.FireResistance", "Resistance to Fire Damage");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_LightningResistance, "Attributes.Secondary.LightningResistance", "Resistance to Lightning Damage");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_ArcaneResistance, "Attributes.Secondary.ArcaneResistance", "Resistance to Arcane Damage");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_PhysicalResistance, "Attributes.Secondary.PhysicalResistance", "Resistance to Physical Damage");
	
	/* Vital Attributes */
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Vital_Health, "Attributes.Vital.Health", "Amount of damage a player can take before death");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Vital_Mana, "Attributes.Vital.Mana", "Amount of energy a player can use to cast spells");

	/* Meta Attributes */
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Meta_Level, "Attributes.Meta.Level", "Indicates the character's current progression level");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Meta_XP, "Attributes.Meta.XP", "Amount of experience points a player has accumulated for leveling up");

	/* Damage Types */
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage_Type_Fire, "Damage.Type.Fire", "Fire Damage Type");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage_Type_Lightning, "Damage.Type.Lightning", "Lightning Damage Type");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage_Type_Arcane, "Damage.Type.Arcane", "Arcane Damage Type");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage_Type_Physical, "Damage.Type.Physical", "Physical Damage Type");
	
	const TMap<FGameplayTag, FGameplayTag> DamageTypeToResistanceMap =
	{
		{ Damage_Type_Fire, Attributes_Secondary_FireResistance },
		{ Damage_Type_Lightning, Attributes_Secondary_LightningResistance },
		{ Damage_Type_Arcane, Attributes_Secondary_ArcaneResistance },
		{ Damage_Type_Physical, Attributes_Secondary_PhysicalResistance }
	};

	/* Role */
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Role_Player, "Role.Player", "Player Role");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Role_Enemy, "Role.Enemy", "Enemy Role");

	/* Event */
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Montage_FireBolt, "Event.Montage.FireBolt", "FireBolt Montage Event Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Montage_FireBall, "Event.Montage.FireBall", "FireBall Montage Event Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Montage_Firenado, "Event.Montage.Firenado", "Firenado Montage Event Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Montage_ChainLightning, "Event.Montage.ChainLightning", "ChainLightning Montage Event Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Montage_ArcaneSpike, "Event.Montage.ArcaneSpike", "ArcaneSpike Montage Event Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Montage_EnemyAttack, "Event.Montage.EnemyAttack", "EnemyAttack Montage Event Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Montage_ShamanSummonEnemy, "Event.Montage.ShamanSummonEnemy", "Shaman's Summon Enemy Montage Event Tag");

	/* Abilities */
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Abilities_ClickToMove, "Abilities.ClickToMove", "ClickToMove Ability Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Abilities_TryInteract, "Abilities.TryInteract", "TryInteract Ability Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Abilities_HitReact, "Abilities.HitReact", "HitReact Ability Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Abilities_EnemyAttack, "Abilities.EnemyAttack", "EnemyAttack Ability Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Abilities_ShamanSummonEnemy, "Abilities.ShamanSummonEnemy", "Shaman's Summon Enemy Ability Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Abilities_NeedConfirm, "Abilities.NeedConfirm", "Ability Tag that need confirm");
	
	/* Abilities.Offensive */
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Abilities_Offensive, "Abilities.Offensive", "Offensive Ability Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Abilities_Offensive_FireBolt, "Abilities.Offensive.FireBolt", "FireBolt Offensive Ability Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Abilities_Offensive_FireBall, "Abilities.Offensive.FireBall", "FireBall Offensive Ability Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Abilities_Offensive_Firenado, "Abilities.Offensive.Firenado", "Firenado Offensive Ability Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Abilities_Offensive_ChainLightning, "Abilities.Offensive.ChainLightning", "ChainLightning Offensive Ability Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Abilities_Offensive_ArcaneSpike, "Abilities.Offensive.ArcaneSpike", "ArcaneSpike Offensive Ability Tag");

	/* Abilities.Passive */
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Abilities_Passive, "Abilities.Passive", "Passive Ability Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Abilities_Passive_HaloOfProtection, "Abilities.Passive.HaloOfProtection", "HaloOfProtection Passive Ability Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Abilities_Passive_HealthSiphon, "Abilities.Passive.HealthSiphon", "HealthSiphon Passive Ability Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Abilities_Passive_ManaSiphon, "Abilities.Passive.ManaSiphon", "ManaSiphon Passive Ability Tag");

	/* Debuff */
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Debuff_Ignite, "Debuff.Ignite", "Ignite Debuff Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Debuff_Enfeeble, "Debuff.Enfeeble", "Enfeeble Debuff Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Debuff_Stun, "Debuff.Stun", "Stun Debuff Tag");

	/* Cooldown */
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Cooldown_ArcaneSpike, "Cooldown.ArcaneSpike", "ArcaneSpike Cooldown Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Cooldown_FireBall, "Cooldown.FireBall", "FireBall Cooldown Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Cooldown_Melee_GoblinSpear, "Cooldown.Melee.GoblinSpear", "GoblinSpear Melee Cooldown Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Cooldown_Melee_DemonWarrior, "Cooldown.Melee.DemonWarrior", "DemonWarrior Melee Cooldown Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Cooldown_Melee_Ghoul, "Cooldown.Melee.Ghoul", "Ghoul Melee Cooldown Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Cooldown_Range_GoblinSlingshot, "Cooldown.Range.GoblinSlingshot", "GoblinSlingshot Range Cooldown Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Cooldown_Range_DemonRanger, "Cooldown.Range.DemonRanger", "DemonRanger Range Cooldown Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Cooldown_Range_Shaman, "Cooldown.Range.Shaman", "Shaman Range Cooldown Tag");

	/* GameplayCue */
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_FireBoltImpact, "GameplayCue.FireBoltImpact", "FireBoltImpact GameplayCue Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_FireBallImpact, "GameplayCue.FireBallImpact", "FireBallImpact GameplayCue Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_Firenado, "GameplayCue.Firenado", "Firenado GameplayCue Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_Firenado_Explosion, "GameplayCue.Firenado.Explosion", "Firenado Final Explosion GameplayCue Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_EnemyMeleeImpact, "GameplayCue.EnemyMeleeImpact", "EnemyMeleeImpact GameplayCue Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_RockImpact, "GameplayCue.RockImpact", "RockImpact GameplayCue Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_LevelUp, "GameplayCue.LevelUp", "LevelUp GameplayCue Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_LightningBeam, "GameplayCue.LightningBeam", "LightningBeam GameplayCue Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_ArcaneShard, "GameplayCue.ArcaneShard", "ArcaneShard GameplayCue Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_SummonEffect, "GameplayCue.SummonEffect", "SummonEffect GameplayCue Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_ShamanSummonEffect, "GameplayCue.ShamanSummonEffect", "ShamanSummonEffect GameplayCue Tag");

	/* Gameplay */
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Gameplay_Invincibility, "Gameplay.Invincibility", "Invincibility Tag");

	/* EffectRules */
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(EffectRules_PerLife, "EffectRules.PerLife", "PerLife GameplayEffect Apply Rule Tag");
}
