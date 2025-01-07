// Fill out your copyright notice in the Description page of Project Settings.


#include "AuraGameplayTags.h"

#include "GameplayTagsManager.h"

FAuraGameplayTags FAuraGameplayTags::GameplayTags;

void FAuraGameplayTags::InitializeNativeGameplayTags()
{
	UGameplayTagsManager& Manager = UGameplayTagsManager::Get();

	/* Input */
	GameplayTags.InputTag = Manager.AddNativeGameplayTag(TEXT("InputTag"), TEXT("Input Tag"));
	GameplayTags.InputTag_LMB = Manager.AddNativeGameplayTag(TEXT("InputTag.LMB"), TEXT("Input Tag for Left Mouse Button"));
	GameplayTags.InputTag_RMB = Manager.AddNativeGameplayTag(TEXT("InputTag.RMB"), TEXT("Input Tag for Right Mouse Button"));
	GameplayTags.InputTag_1 = Manager.AddNativeGameplayTag(TEXT("InputTag.1"), TEXT("Input Tag for 1 key"));
	GameplayTags.InputTag_2 = Manager.AddNativeGameplayTag(TEXT("InputTag.2"), TEXT("Input Tag for 2 key"));
	GameplayTags.InputTag_3 = Manager.AddNativeGameplayTag(TEXT("InputTag.3"), TEXT("Input Tag for 3 key"));
	GameplayTags.InputTag_4 = Manager.AddNativeGameplayTag(TEXT("InputTag.4"), TEXT("Input Tag for 4 key"));

	/* Passive Input */
	GameplayTags.InputTag_Passive_1 = Manager.AddNativeGameplayTag(TEXT("InputTag.Passive.1"), TEXT("Passive Input Tag for first slot"));
	GameplayTags.InputTag_Passive_2 = Manager.AddNativeGameplayTag(TEXT("InputTag.Passive.2"), TEXT("Passive Input Tag for second slot"));

	/* Primary Attributes */
	GameplayTags.Attributes_Primary_Strength = Manager.AddNativeGameplayTag(TEXT("Attributes.Primary.Strength"), TEXT("Increases physical damage"));
	GameplayTags.Attributes_Primary_Intelligence = Manager.AddNativeGameplayTag(TEXT("Attributes.Primary.Intelligence"), TEXT("Increases magical damage"));
	GameplayTags.Attributes_Primary_Resilience = Manager.AddNativeGameplayTag(TEXT("Attributes.Primary.Resilience"), TEXT("Increases Armor and Armor Penetration"));
	GameplayTags.Attributes_Primary_Vigor = Manager.AddNativeGameplayTag(TEXT("Attributes.Primary.Vigor"), TEXT("Increases Health"));
	
	/* Secondary Attributes */
	GameplayTags.Attributes_Secondary_Armor = Manager.AddNativeGameplayTag(TEXT("Attributes.Secondary.Armor"), TEXT("Reduces damage taken, improves Block Chance"));
	GameplayTags.Attributes_Secondary_ArmorPenetration = Manager.AddNativeGameplayTag(TEXT("Attributes.Secondary.ArmorPenetration"), TEXT("Ignored Percentage of enemy Armor, increases Critical Hit Chance"));
	GameplayTags.Attributes_Secondary_BlockChance = Manager.AddNativeGameplayTag(TEXT("Attributes.Secondary.BlockChance"), TEXT("Chance to cut incoming damage in half"));
	GameplayTags.Attributes_Secondary_CriticalHitChance = Manager.AddNativeGameplayTag(TEXT("Attributes.Secondary.CriticalHitChance"), TEXT("Chance to double damage plus critical hit bonus"));
	GameplayTags.Attributes_Secondary_CriticalHitDamage = Manager.AddNativeGameplayTag(TEXT("Attributes.Secondary.CriticalHitDamage"), TEXT("Bonus damage added when a critical hit is scored"));
	GameplayTags.Attributes_Secondary_CriticalHitResistance = Manager.AddNativeGameplayTag(TEXT("Attributes.Secondary.CriticalHitResistance"), TEXT("Reduces Critical Hit Chance of attacking enemies"));
	GameplayTags.Attributes_Secondary_HealthRegeneration = Manager.AddNativeGameplayTag(TEXT("Attributes.Secondary.HealthRegeneration"), TEXT("Amount of Health regenerated every 1 second"));
	GameplayTags.Attributes_Secondary_ManaRegeneration = Manager.AddNativeGameplayTag(TEXT("Attributes.Secondary.ManaRegeneration"), TEXT("Amount of Mana regenerated every 1 second"));
	GameplayTags.Attributes_Secondary_MaxHealth = Manager.AddNativeGameplayTag(TEXT("Attributes.Secondary.MaxHealth"), TEXT("Maximum amount of Health obtainable"));
	GameplayTags.Attributes_Secondary_MaxMana = Manager.AddNativeGameplayTag(TEXT("Attributes.Secondary.MaxMana"), TEXT("Maximum amount of Mana obtainable"));
	GameplayTags.Attributes_Secondary_FireResistance = Manager.AddNativeGameplayTag(TEXT("Attributes.Secondary.FireResistance"), TEXT("Resistance to Fire Damage"));
	GameplayTags.Attributes_Secondary_LightningResistance = Manager.AddNativeGameplayTag(TEXT("Attributes.Secondary.LightningResistance"), TEXT("Resistance to Lightning Damage"));
	GameplayTags.Attributes_Secondary_ArcaneResistance = Manager.AddNativeGameplayTag(TEXT("Attributes.Secondary.ArcaneResistance"), TEXT("Resistance to Arcane Damage"));
	GameplayTags.Attributes_Secondary_PhysicalResistance = Manager.AddNativeGameplayTag(TEXT("Attributes.Secondary.PhysicalResistance"), TEXT("Resistance to Physical Damage"));
	
	/* Vital Attributes */
	GameplayTags.Attributes_Vital_Health = Manager.AddNativeGameplayTag(TEXT("Attributes.Vital.Health"), TEXT("Amount of damage a player can take before death"));
	GameplayTags.Attributes_Vital_Mana = Manager.AddNativeGameplayTag(TEXT("Attributes.Vital.Mana"), TEXT("Amount of energy a player can use to cast spells"));

	/* Meta Attributes */
	GameplayTags.Attributes_Meta_Level = Manager.AddNativeGameplayTag(TEXT("Attributes.Meta.Level"), TEXT("Indicates the character's current progression level"));
	GameplayTags.Attributes_Meta_XP = Manager.AddNativeGameplayTag(TEXT("Attributes.Meta.XP"), TEXT("Amount of experience points a player has accumulated for leveling up"));

	/* Damage Types */
	GameplayTags.Damage_Type_Fire = Manager.AddNativeGameplayTag(TEXT("Damage.Type.Fire"), TEXT("Fire Damage Type"));
	GameplayTags.DamageTypeToResistanceMap.Add(GameplayTags.Damage_Type_Fire, GameplayTags.Attributes_Secondary_FireResistance);
	
	GameplayTags.Damage_Type_Lightning = Manager.AddNativeGameplayTag(TEXT("Damage.Type.Lightning"), TEXT("Lightning Damage Type"));
	GameplayTags.DamageTypeToResistanceMap.Add(GameplayTags.Damage_Type_Lightning, GameplayTags.Attributes_Secondary_LightningResistance);
	
	GameplayTags.Damage_Type_Arcane = Manager.AddNativeGameplayTag(TEXT("Damage.Type.Arcane"), TEXT("Arcane Damage Type"));
	GameplayTags.DamageTypeToResistanceMap.Add(GameplayTags.Damage_Type_Arcane, GameplayTags.Attributes_Secondary_ArcaneResistance);
	
	GameplayTags.Damage_Type_Physical = Manager.AddNativeGameplayTag(TEXT("Damage.Type.Physical"), TEXT("Physical Damage Type"));
	GameplayTags.DamageTypeToResistanceMap.Add(GameplayTags.Damage_Type_Physical, GameplayTags.Attributes_Secondary_PhysicalResistance);

	/* Role */
	GameplayTags.Role_Player = Manager.AddNativeGameplayTag(TEXT("Role.Player"), TEXT("Player Role"));
	GameplayTags.Role_Enemy = Manager.AddNativeGameplayTag(TEXT("Role.Enemy"), TEXT("Enemy Role"));

	/* Event */
	GameplayTags.Event_Montage_FireBolt = Manager.AddNativeGameplayTag(TEXT("Event.Montage.FireBolt"), TEXT("FireBolt Montage Event Tag"));
	GameplayTags.Event_Montage_ChainLightning = Manager.AddNativeGameplayTag(TEXT("Event.Montage.ChainLightning"), TEXT("ChainLightning Montage Event Tag"));
	GameplayTags.Event_Montage_ArcaneSpike = Manager.AddNativeGameplayTag(TEXT("Event.Montage.ArcaneSpike"), TEXT("ArcaneSpike Montage Event Tag"));
	GameplayTags.Event_Montage_EnemyAttack = Manager.AddNativeGameplayTag(TEXT("Event.Montage.EnemyAttack"), TEXT("EnemyAttack Montage Event Tag"));

	/* Abilities */
	GameplayTags.Abilities_ClickToMove = Manager.AddNativeGameplayTag(TEXT("Abilities.ClickToMove"), TEXT("ClickToMove Ability Tag"));
	GameplayTags.Abilities_HitReact = Manager.AddNativeGameplayTag(TEXT("Abilities.HitReact"), TEXT("HitReact Ability Tag"));
	GameplayTags.Abilities_EnemyAttack = Manager.AddNativeGameplayTag(TEXT("Abilities.EnemyAttack"), TEXT("EnemyAttack Ability Tag"));

	/* Abilities.Offensive */
	GameplayTags.Abilities_Offensive = Manager.AddNativeGameplayTag(TEXT("Abilities.Offensive"), TEXT("Offensive Ability Tag"));
	GameplayTags.Abilities_Offensive_FireBolt = Manager.AddNativeGameplayTag(TEXT("Abilities.Offensive.FireBolt"), TEXT("FireBolt Offensive Ability Tag"));
	GameplayTags.Abilities_Offensive_ChainLightning = Manager.AddNativeGameplayTag(TEXT("Abilities.Offensive.ChainLightning"), TEXT("ChainLightning Offensive Ability Tag"));
	GameplayTags.Abilities_Offensive_ArcaneSpike = Manager.AddNativeGameplayTag(TEXT("Abilities.Offensive.ArcaneSpike"), TEXT("ArcaneSpike Offensive Ability Tag"));

	/* Abilities.Passive */
	GameplayTags.Abilities_Passive = Manager.AddNativeGameplayTag(TEXT("Abilities.Passive"), TEXT("Passive Ability Tag"));
	GameplayTags.Abilities_Passive_Test = Manager.AddNativeGameplayTag(TEXT("Abilities.Passive.Test"), TEXT("Test Passive Ability Tag"));

	/* Cooldown */
	GameplayTags.Cooldown_ArcaneSpike = Manager.AddNativeGameplayTag(TEXT("Cooldown.ArcaneSpike"), TEXT("ArcaneSpike Cooldown Tag"));
	
	GameplayTags.Cooldown_Melee_GoblinSpear = Manager.AddNativeGameplayTag(TEXT("Cooldown.Melee.GoblinSpear"), TEXT("GoblinSpear Melee Cooldown Tag"));
	GameplayTags.Cooldown_Melee_DemonWarrior = Manager.AddNativeGameplayTag(TEXT("Cooldown.Melee.DemonWarrior"), TEXT("DemonWarrior Melee Cooldown Tag"));
	GameplayTags.Cooldown_Melee_Ghoul = Manager.AddNativeGameplayTag(TEXT("Cooldown.Melee.Ghoul"), TEXT("Ghoul Melee Cooldown Tag"));
	GameplayTags.Cooldown_Range_GoblinSlingshot = Manager.AddNativeGameplayTag(TEXT("Cooldown.Range.GoblinSlingshot"), TEXT("GoblinSlingshot Range Cooldown Tag"));
	GameplayTags.Cooldown_Range_DemonRanger = Manager.AddNativeGameplayTag(TEXT("Cooldown.Range.DemonRanger"), TEXT("DemonRanger Range Cooldown Tag"));
	GameplayTags.Cooldown_Range_Shaman = Manager.AddNativeGameplayTag(TEXT("Cooldown.Range.Shaman"), TEXT("Shaman Range Cooldown Tag"));

	/* GameplayCue */
	GameplayTags.GameplayCue_FireBoltImpact = Manager.AddNativeGameplayTag(TEXT("GameplayCue.FireBoltImpact"), TEXT("FireBoltImpact GameplayCue Tag"));
	GameplayTags.GameplayCue_EnemyMeleeImpact = Manager.AddNativeGameplayTag(TEXT("GameplayCue.EnemyMeleeImpact"), TEXT("EnemyMeleeImpact GameplayCue Tag"));
	GameplayTags.GameplayCue_RockImpact = Manager.AddNativeGameplayTag(TEXT("GameplayCue.RockImpact"), TEXT("RockImpact GameplayCue Tag"));
	GameplayTags.GameplayCue_LevelUp = Manager.AddNativeGameplayTag(TEXT("GameplayCue.LevelUp"), TEXT("LevelUp GameplayCue Tag"));
	GameplayTags.GameplayCue_LightningBeam = Manager.AddNativeGameplayTag(TEXT("GameplayCue.LightningBeam"), TEXT("LightningBeam GameplayCue Tag"));
	GameplayTags.GameplayCue_ArcaneShard = Manager.AddNativeGameplayTag(TEXT("GameplayCue.ArcaneShard"), TEXT("ArcaneShard GameplayCue Tag"));
}
