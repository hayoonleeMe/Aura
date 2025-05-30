// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAttributeSet.h"

#include "AuraGameplayTags.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraGameplayEffectContext.h"
#include "Interface/CombatInterface.h"
#include "Interface/PlayerInterface.h"
#include "Net/UnrealNetwork.h"

UAuraAttributeSet::UAuraAttributeSet()
{
	/* Primary Attributes */
	TagToAttributeMap.Add(AuraGameplayTags::Attributes_Primary_Strength, GetStrengthAttribute());
	TagToAttributeMap.Add(AuraGameplayTags::Attributes_Primary_Intelligence, GetIntelligenceAttribute());
	TagToAttributeMap.Add(AuraGameplayTags::Attributes_Primary_Resilience, GetResilienceAttribute());
	TagToAttributeMap.Add(AuraGameplayTags::Attributes_Primary_Vigor, GetVigorAttribute());
	
	/* Secondary Attributes */
	TagToAttributeMap.Add(AuraGameplayTags::Attributes_Secondary_Armor, GetArmorAttribute());
	TagToAttributeMap.Add(AuraGameplayTags::Attributes_Secondary_ArmorPenetration, GetArmorPenetrationAttribute());
	TagToAttributeMap.Add(AuraGameplayTags::Attributes_Secondary_BlockChance, GetBlockChanceAttribute());
	TagToAttributeMap.Add(AuraGameplayTags::Attributes_Secondary_CriticalHitChance, GetCriticalHitChanceAttribute());
	TagToAttributeMap.Add(AuraGameplayTags::Attributes_Secondary_CriticalHitDamage, GetCriticalHitDamageAttribute());
	TagToAttributeMap.Add(AuraGameplayTags::Attributes_Secondary_CriticalHitResistance, GetCriticalHitResistanceAttribute());
	TagToAttributeMap.Add(AuraGameplayTags::Attributes_Secondary_HealthRegeneration, GetHealthRegenerationAttribute());
	TagToAttributeMap.Add(AuraGameplayTags::Attributes_Secondary_ManaRegeneration, GetManaRegenerationAttribute());
	TagToAttributeMap.Add(AuraGameplayTags::Attributes_Secondary_MaxHealth, GetMaxHealthAttribute());
	TagToAttributeMap.Add(AuraGameplayTags::Attributes_Secondary_MaxMana, GetMaxManaAttribute());
	TagToAttributeMap.Add(AuraGameplayTags::Attributes_Secondary_FireResistance, GetFireResistanceAttribute());
	TagToAttributeMap.Add(AuraGameplayTags::Attributes_Secondary_LightningResistance, GetLightningResistanceAttribute());
	TagToAttributeMap.Add(AuraGameplayTags::Attributes_Secondary_ArcaneResistance, GetArcaneResistanceAttribute());
	TagToAttributeMap.Add(AuraGameplayTags::Attributes_Secondary_PhysicalResistance, GetPhysicalResistanceAttribute());
}

void UAuraAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	/* Primary Attributes */
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Intelligence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Resilience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Vigor, COND_None, REPNOTIFY_Always);

	/* Secondary Attributes */
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ArmorPenetration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, BlockChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, HealthRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ManaRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, FireResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, LightningResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ArcaneResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, PhysicalResistance, COND_None, REPNOTIFY_Always);

	/* Vital Attributes */
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Mana, COND_None, REPNOTIFY_Always);

	/* Meta Attributes */
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Level, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, XP, COND_None, REPNOTIFY_Always);
}

void UAuraAttributeSet::PrintDebug()
{
	UE_LOG(LogTemp, Warning, TEXT("=== OwningActor : %s ==="), *GetOwningActor()->GetName());
	UE_LOG(LogTemp, Warning, TEXT("=== Primary Attributes ==="));
	UE_LOG(LogTemp, Warning, TEXT("%s : %f"), *GetStrengthAttribute().GetName(), GetStrength());
	UE_LOG(LogTemp, Warning, TEXT("%s : %f"), *GetIntelligenceAttribute().GetName(), GetIntelligence());
	UE_LOG(LogTemp, Warning, TEXT("%s : %f"), *GetResilienceAttribute().GetName(), GetResilience());
	UE_LOG(LogTemp, Warning, TEXT("%s : %f"), *GetVigorAttribute().GetName(), GetVigor());
	UE_LOG(LogTemp, Warning, TEXT("=== Secondary Attributes ==="));
	UE_LOG(LogTemp, Warning, TEXT("%s : %f"), *GetArmorAttribute().GetName(), GetArmor());
	UE_LOG(LogTemp, Warning, TEXT("%s : %f"), *GetArmorPenetrationAttribute().GetName(), GetArmorPenetration());
	UE_LOG(LogTemp, Warning, TEXT("%s : %f"), *GetBlockChanceAttribute().GetName(), GetBlockChance());
	UE_LOG(LogTemp, Warning, TEXT("%s : %f"), *GetCriticalHitChanceAttribute().GetName(), GetCriticalHitChance());
	UE_LOG(LogTemp, Warning, TEXT("%s : %f"), *GetCriticalHitDamageAttribute().GetName(), GetCriticalHitDamage());
	UE_LOG(LogTemp, Warning, TEXT("%s : %f"), *GetCriticalHitResistanceAttribute().GetName(), GetCriticalHitResistance());
	UE_LOG(LogTemp, Warning, TEXT("%s : %f"), *GetHealthRegenerationAttribute().GetName(), GetHealthRegeneration());
	UE_LOG(LogTemp, Warning, TEXT("%s : %f"), *GetManaRegenerationAttribute().GetName(), GetManaRegeneration());
	UE_LOG(LogTemp, Warning, TEXT("%s : %f"), *GetMaxHealthAttribute().GetName(), GetMaxHealth());
	UE_LOG(LogTemp, Warning, TEXT("%s : %f"), *GetMaxManaAttribute().GetName(), GetMaxMana());
	UE_LOG(LogTemp, Warning, TEXT("%s : %f"), *GetFireResistanceAttribute().GetName(), GetFireResistance());
	UE_LOG(LogTemp, Warning, TEXT("%s : %f"), *GetLightningResistanceAttribute().GetName(), GetLightningResistance());
	UE_LOG(LogTemp, Warning, TEXT("%s : %f"), *GetArcaneResistanceAttribute().GetName(), GetArcaneResistance());
	UE_LOG(LogTemp, Warning, TEXT("%s : %f"), *GetPhysicalResistanceAttribute().GetName(), GetPhysicalResistance());
	UE_LOG(LogTemp, Warning, TEXT("=== Vital Attributes ==="));
	UE_LOG(LogTemp, Warning, TEXT("%s : %f"), *GetHealthAttribute().GetName(), GetHealth());
	UE_LOG(LogTemp, Warning, TEXT("%s : %f"), *GetManaAttribute().GetName(), GetMana());
}

void UAuraAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		HandleIncomingDamage(Data.EffectSpec);
	}
	else if (Data.EvaluatedData.Attribute == GetXPAttribute())
	{
		HandlePlayerXPGain();
	}
}

void UAuraAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	// Clamping Base Value Change - by damage, regeneration, health recovery, mana recovery...
	// base value change come from instant gameplay effect, periodic gameplay effect (treated like instant ge)
	// current value change come from duration gameplay effect, infinite gameplay effect
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	else if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
	}
}

void UAuraAttributeSet::HandleIncomingDamage(const FGameplayEffectSpec& EffectSpec)
{
	const float LocalIncomingDamage = GetIncomingDamage();
	if (LocalIncomingDamage > 0.f)
	{
		AActor* AvatarActor = GetActorInfo() && GetActorInfo()->AvatarActor.IsValid() ? GetActorInfo()->AvatarActor.Get() : nullptr;
		ICombatInterface* CombatInterface = Cast<ICombatInterface>(AvatarActor);

		if (!CombatInterface || CombatInterface->IsDead())
		{
			return;
		}

		const FAuraGameplayEffectContext* AuraEffectContext = FAuraGameplayEffectContext::ExtractEffectContext(EffectSpec.GetEffectContext());
        check(AuraEffectContext);

		// Update Health
		const float NewHealth = FMath::Clamp(GetHealth() - LocalIncomingDamage, 0.f, GetMaxHealth());
		SetHealth(NewHealth);

		if (NewHealth <= 0.f)
		{
			// Dead
			CombatInterface->Die();

			// if player kill enemy
			if (CombatInterface->GetRoleTag().MatchesTagExact(AuraGameplayTags::Role_Enemy))
			{
				// Enemy Dead, Reward XP to Player
				const int32 XPReward = CombatInterface->GetXPReward();
				if (UAuraAbilitySystemComponent* InstigatorASC = Cast<UAuraAbilitySystemComponent>(AuraEffectContext->GetInstigatorAbilitySystemComponent()))
				{
					InstigatorASC->ApplyXPGainEffect(XPReward);
				}
			}
		}
		else
		{
			UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponentChecked();
			
			// Debuff
			if (FMath::RandRange(1, 100) <= AuraEffectContext->GetDebuffChance())
			{
				const FGameplayTag DebuffTag = AuraEffectContext->GetDebuffTag();
				if (DebuffTag.MatchesTagExact(AuraGameplayTags::Debuff_Ignite))
				{
					ActivateIgniteDebuff(AuraEffectContext, LocalIncomingDamage);
				}
				else if (DebuffTag.MatchesTagExact(AuraGameplayTags::Debuff_Stun))
				{
					ActivateStunDebuff(AuraEffectContext);
				}
				else
				{
					ASC->TryActivateAbilitiesByTag(DebuffTag.GetSingleTagContainer());
				}
			}

			// HitReact
			if (AuraEffectContext->ShouldActivateHitReact() && !ASC->HasMatchingGameplayTag(AuraGameplayTags::Debuff_Stun))
			{
				ASC->TryActivateAbilitiesByTag(AuraGameplayTags::Abilities_HitReact.GetTag().GetSingleTagContainer());
			}
		}

		// Damage Indicator 표시
		const IPlayerInterface* PlayerInterface = Cast<IPlayerInterface>(AuraEffectContext->GetEffectCauser());
		if (AvatarActor && PlayerInterface)
		{
			PlayerInterface->IndicateDamage(LocalIncomingDamage, AuraEffectContext->IsBlockedHit(), AuraEffectContext->IsCriticalHit(), AvatarActor->GetActorLocation());
		}
	}
}

void UAuraAttributeSet::HandlePlayerXPGain()
{
	AActor* AvatarActor = GetActorInfo() && GetActorInfo()->AvatarActor.IsValid() ? GetActorInfo()->AvatarActor.Get() : nullptr;
	IPlayerInterface* PlayerInterface = Cast<IPlayerInterface>(AvatarActor);
	if (!PlayerInterface)
	{
		return;
	}

	int32 LocalXP = GetXP();
	while (LocalXP)
	{
		const int32 NextPlayerLevel = GetLevel() + 1;
		const int32 XPRequirement = PlayerInterface->GetLevelUpXpRequirement(NextPlayerLevel);
		
		// LevelUp
		if (LocalXP >= XPRequirement)
		{
			LocalXP -= XPRequirement;
			
			// Add Attribute Points, Spell Points
			const int32 AttributePointsAward = PlayerInterface->GetLevelUpAttributePointsAward(NextPlayerLevel);
			PlayerInterface->AddToAttributePoints(AttributePointsAward);
			const int32 SpellPointsAward = PlayerInterface->GetLevelUpSpellPointsAward(NextPlayerLevel);
			PlayerInterface->AddToSpellPoints(SpellPointsAward);

			// Increment Level, will calculate new MaxHealth, MaxMana value (by MMC_MaxHealth, MMC_MaxMana)
			SetLevel(NextPlayerLevel);

			// fill Health, Mana
			SetHealth(GetMaxHealth());
			SetMana(GetMaxMana());

			// 플레이어 기기에서 NonReplicated LevelUp Effect GameplayCue 실행
			if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(GetOwningAbilitySystemComponent()))
			{
				AuraASC->ClientExecuteGameplayCue(AuraGameplayTags::GameplayCue_LevelUp);
			}
		}
		else
		{
			break;
		}
	}

	// Update Excess XP
	SetXP(LocalXP);
}

void UAuraAttributeSet::ActivateIgniteDebuff(const FAuraGameplayEffectContext* AuraEffectContext, float LocalIncomingDamage)
{
	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(GetOwningAbilitySystemComponentChecked()))
	{
		if (FGameplayAbilitySpec* AbilitySpec = AuraASC->GetSpellSpecForSpellTag(AuraEffectContext->GetDebuffTag()))
		{
			// 데미지를 입히는 Instigator를 SourceObject로 등록
			// Debuff Ability에서 이 Instigator를 이용해 Debuff Effect 적용
			AbilitySpec->SourceObject = AuraEffectContext->GetInstigator();

			// Incoming Damage를 전달하기 위해 SetByCallerTagMagnitudes 사용 
			AbilitySpec->SetByCallerTagMagnitudes.Add(AuraGameplayTags::Damage_Type_Fire, LocalIncomingDamage);
						
			AuraASC->TryActivateAbility(AbilitySpec->Handle);
		}
	}
}

void UAuraAttributeSet::ActivateStunDebuff(const FAuraGameplayEffectContext* AuraEffectContext)
{
	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(GetOwningAbilitySystemComponent()))
	{
		if (FGameplayAbilitySpec* AbilitySpec = AuraASC->GetSpellSpecForSpellTag(AuraEffectContext->GetDebuffTag()))
		{
			// Stun Duration을 전달하기 위해 SetByCallerTagMagnitudes 사용 
			AbilitySpec->SetByCallerTagMagnitudes.Add(AuraGameplayTags::Debuff_Stun, AuraEffectContext->GetDebuffDuration());
						
			AuraASC->TryActivateAbility(AbilitySpec->Handle);
		}
	}
}

void UAuraAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldStrength) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Strength, OldStrength);
}

void UAuraAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Intelligence, OldIntelligence);
}

void UAuraAttributeSet::OnRep_Resilience(const FGameplayAttributeData& OldResilience) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Resilience, OldResilience);
}

void UAuraAttributeSet::OnRep_Vigor(const FGameplayAttributeData& OldVigor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Vigor, OldVigor);
}

void UAuraAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Armor, OldArmor);
}

void UAuraAttributeSet::OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ArmorPenetration, OldArmorPenetration);
}

void UAuraAttributeSet::OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, BlockChance, OldBlockChance);
}

void UAuraAttributeSet::OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitChance, OldCriticalHitChance);
}

void UAuraAttributeSet::OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitDamage, OldCriticalHitDamage);
}

void UAuraAttributeSet::OnRep_CriticalHitResistance(const FGameplayAttributeData& OldCriticalHitResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitResistance, OldCriticalHitResistance);
}

void UAuraAttributeSet::OnRep_HealthRegeneration(const FGameplayAttributeData& OldHealthRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, HealthRegeneration, OldHealthRegeneration);
}

void UAuraAttributeSet::OnRep_ManaRegeneration(const FGameplayAttributeData& OldManaRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ManaRegeneration, OldManaRegeneration);
}

void UAuraAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxHealth, OldMaxHealth);
}

void UAuraAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxMana, OldMaxMana);
}

void UAuraAttributeSet::OnRep_FireResistance(const FGameplayAttributeData& OldFireResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, FireResistance, OldFireResistance);
}

void UAuraAttributeSet::OnRep_LightningResistance(const FGameplayAttributeData& OldLightningResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, LightningResistance, OldLightningResistance);
}

void UAuraAttributeSet::OnRep_ArcaneResistance(const FGameplayAttributeData& OldArcaneResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ArcaneResistance, OldArcaneResistance);
}

void UAuraAttributeSet::OnRep_PhysicalResistance(const FGameplayAttributeData& OldPhysicalResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, PhysicalResistance, OldPhysicalResistance);
}

void UAuraAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Health, OldHealth);
}

void UAuraAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Mana, OldMana);
}

void UAuraAttributeSet::OnRep_Level(const FGameplayAttributeData& OldLevel) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Level, OldLevel);
}

void UAuraAttributeSet::OnRep_XP(const FGameplayAttributeData& OldXP) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, XP, OldXP);	
}
