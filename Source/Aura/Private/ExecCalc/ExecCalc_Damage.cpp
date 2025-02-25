// Fill out your copyright notice in the Description page of Project Settings.


#include "ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/AuraGameplayEffectContext.h"
#include "AbilitySystem/Abilities/AuraAbility_Debuff_Enfeeble.h"
#include "AbilitySystem/Abilities/AuraAbility_HaloOfProtection.h"

// 구조체로 Capture할 Attribute 관리 및 설정
struct AuraDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(FireResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(LightningResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArcaneResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalResistance);

	AuraDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitDamage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, FireResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, LightningResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArcaneResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, PhysicalResistance, Target, false);
	}
};

// Capture할 Attribute를 한번만 생성, 등록하기 위해 static으로 관리
static const AuraDamageStatics& GetDamageStatics()
{
	static AuraDamageStatics Statics;
	return Statics;
}

// FGameplayEffectAttributeCaptureDefinition에 편하게 접근하기 위한 Map
// 한번만 생성, 초기화하기 위해 static으로 관리
static const TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition>& GetTagToCaptureDefMap()
{
	static TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> Map;
	if (Map.IsEmpty())
	{
		const AuraDamageStatics& DamageStatics = GetDamageStatics();
		Map.Add(AuraGameplayTags::Attributes_Secondary_Armor, DamageStatics.ArmorDef);
		Map.Add(AuraGameplayTags::Attributes_Secondary_ArmorPenetration, DamageStatics.ArmorPenetrationDef);
		Map.Add(AuraGameplayTags::Attributes_Secondary_BlockChance, DamageStatics.BlockChanceDef);
		Map.Add(AuraGameplayTags::Attributes_Secondary_CriticalHitChance, DamageStatics.CriticalHitChanceDef);
		Map.Add(AuraGameplayTags::Attributes_Secondary_CriticalHitDamage, DamageStatics.CriticalHitDamageDef);
		Map.Add(AuraGameplayTags::Attributes_Secondary_CriticalHitResistance, DamageStatics.CriticalHitResistanceDef);
		Map.Add(AuraGameplayTags::Attributes_Secondary_FireResistance, DamageStatics.FireResistanceDef);
		Map.Add(AuraGameplayTags::Attributes_Secondary_LightningResistance, DamageStatics.LightningResistanceDef);
		Map.Add(AuraGameplayTags::Attributes_Secondary_ArcaneResistance, DamageStatics.ArcaneResistanceDef);
		Map.Add(AuraGameplayTags::Attributes_Secondary_PhysicalResistance, DamageStatics.PhysicalResistanceDef);
	}
	return Map;
}

UExecCalc_Damage::UExecCalc_Damage()
{
	// Capture할 Attribute 등록
	const AuraDamageStatics& DamageStatics = GetDamageStatics(); 
	RelevantAttributesToCapture.Add(DamageStatics.ArmorDef);
	RelevantAttributesToCapture.Add(DamageStatics.ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics.BlockChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics.CriticalHitChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics.CriticalHitDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics.CriticalHitResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics.FireResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics.LightningResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics.ArcaneResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics.PhysicalResistanceDef);
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                              FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const AuraDamageStatics& DamageStatics = GetDamageStatics();
	const TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition>& TagToCaptureDefMap = GetTagToCaptureDefMap();

	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	UAuraAbilitySystemComponent* TargetASC = Cast<UAuraAbilitySystemComponent>(ExecutionParams.GetTargetAbilitySystemComponent());
	if (!SourceASC || !TargetASC)
	{
		return;
	}

	// Character Level
	const int32 SourceCharacterLevel = UAuraBlueprintLibrary::GetLevelAttributeValue(SourceASC->GetAvatarActor());
	const int32 TargetCharacterLevel = UAuraBlueprintLibrary::GetLevelAttributeValue(TargetASC->GetAvatarActor());

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	// Gameplay Effect Context 
	FAuraGameplayEffectContext* AuraEffectContext = FAuraGameplayEffectContext::ExtractEffectContext(Spec.GetEffectContext());
    check(AuraEffectContext);

	// Get Damage SetByCaller Magnitude
	float Damage = 0.f;

	// 이 Damage Effect를 적용할 때 설정한 SetByCaller 값을 찾아 저항값을 적용한 데미지를 합산한다.
	for (const TTuple<FGameplayTag, FGameplayTag>& Tuple : AuraGameplayTags::DamageTypeToResistanceMap)
	{
		const FGameplayTag& DamageTypeTag = Tuple.Key;
		const FGameplayTag& ResistanceTag = Tuple.Value;

		check(TagToCaptureDefMap.Contains(ResistanceTag));
		const FGameplayEffectAttributeCaptureDefinition& CaptureDef = TagToCaptureDefMap[ResistanceTag];

		// 해당 DamageTypeTag로 설정된 SetByCaller 값이 없으면 0 반환
		float DamageTypeValue = Spec.GetSetByCallerMagnitude(DamageTypeTag, false);
		if (DamageTypeValue > 0.f)
		{
			// Target Resistance Attribute Value By DamageType
			float ResistanceValue = 0.f;
			ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CaptureDef, EvaluateParameters, ResistanceValue);
			DamageTypeValue *= (100.f - ResistanceValue) / 100.f;
			Damage += DamageTypeValue;
		}
	}

	// Damage Reduction by Halo Of Protection Passive Spell
	if (FGameplayAbilitySpec* AbilitySpec = TargetASC->GetSpellSpecForSpellTag(AuraGameplayTags::Abilities_Passive_HaloOfProtection))
	{
		float DamageReductionRate = 0.f;
		if (UAuraAbility_HaloOfProtection* HaloOfProtection = Cast<UAuraAbility_HaloOfProtection>(AbilitySpec->GetPrimaryInstance()))
		{
			DamageReductionRate = HaloOfProtection->GetDamageReductionRate();
		}
		Damage *= (1.f - DamageReductionRate);
	}

	// Block Hit
	if (AuraEffectContext->CanBlockedHit())
	{
		// Target BlockChance
		float TargetBlockChance = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics.BlockChanceDef, EvaluateParameters, TargetBlockChance);
	
		// If Block, halve the Damage
		const bool bBlocked = FMath::RandRange(1, 100) <= TargetBlockChance;
		Damage = bBlocked ? Damage * 0.5f : Damage;
		AuraEffectContext->SetIsBlockedHit(bBlocked);
	}

	// Target Armor & Source ArmorPenetration
	float TargetArmor = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics.ArmorDef, EvaluateParameters, TargetArmor);

	float SourceArmorPenetration = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics.ArmorPenetrationDef, EvaluateParameters, SourceArmorPenetration);

	// ArmorPenetration ignores a percentage of the Target's Armor
	const float SourceArmorPenetrationCoefficient = UAuraBlueprintLibrary::GetArmorPenetrationCoefficientByLevel(SourceASC->GetAvatarActor(), SourceCharacterLevel);
	const float TargetEffectiveArmor = TargetArmor * (100 - SourceArmorPenetration * SourceArmorPenetrationCoefficient) / 100.f;

	// Armor ignores a percentage of incoming Damage
	const float TargetEffectiveArmorCoefficient = UAuraBlueprintLibrary::GetEffectiveArmorCoefficientByLevel(TargetASC->GetAvatarActor(), TargetCharacterLevel);
	Damage *= (100 - TargetEffectiveArmor * TargetEffectiveArmorCoefficient) / 100.f;

	// Critical Hit
	if (AuraEffectContext->CanCriticalHit())
	{
		float SourceCriticalHitChance = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics.CriticalHitChanceDef, EvaluateParameters, SourceCriticalHitChance);

		float SourceCriticalHitDamage = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics.CriticalHitDamageDef, EvaluateParameters, SourceCriticalHitDamage);

		float TargetCriticalHitResistance = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics.CriticalHitResistanceDef, EvaluateParameters, TargetCriticalHitResistance);

		const float TargetCriticalHitResistanceCoefficient = UAuraBlueprintLibrary::GetCriticalHitResistanceCoefficientByLevel(TargetASC->GetAvatarActor(), TargetCharacterLevel);

		// Critical Hit Resistance reduces Critical Hit Chance by a certain percentage
		const float SourceEffectiveCriticalHitChance = SourceCriticalHitChance - TargetCriticalHitResistance * TargetCriticalHitResistanceCoefficient;
		const bool bCriticalHit = FMath::RandRange(1, 100) <= SourceEffectiveCriticalHitChance;
		// Double damage plus a bonus if critical hit
		Damage = bCriticalHit ? Damage * 2.f + SourceCriticalHitDamage : Damage; 
		AuraEffectContext->SetIsCriticalHit(bCriticalHit);	
	}
	
	// Health recovery from Damage
	if (SourceASC->HasMatchingGameplayTag(AuraGameplayTags::Abilities_Passive_HealthSiphon))
	{
		if (AActor* SourceAvatarActor = SourceASC->GetAvatarActor())
		{
			FGameplayEventData Payload;
			Payload.EventMagnitude = Damage;
			// Will receive this event in AuraAbility_HealthSiphon, then recovery health
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(SourceAvatarActor, AuraGameplayTags::Abilities_Passive_HealthSiphon, Payload);
		}
	}

	// Mana recovery from Damage
	if (SourceASC->HasMatchingGameplayTag(AuraGameplayTags::Abilities_Passive_ManaSiphon))
	{
		if (AActor* SourceAvatarActor = SourceASC->GetAvatarActor())
		{
			FGameplayEventData Payload;
			Payload.EventMagnitude = Damage;
			// Will receive this event in AuraAbility_ManaSiphon, then recovery mana
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(SourceAvatarActor, AuraGameplayTags::Abilities_Passive_ManaSiphon, Payload);
		}
	}

	// Damage Reduction by Enfeeble Debuff
	if (SourceASC->HasMatchingGameplayTag(AuraGameplayTags::Debuff_Enfeeble))
	{
		Damage *= (1.f - UAuraAbility_Debuff_Enfeeble::GetDamageReductionRate());
	}

	// 최종 Damage를 IncomingDamage Attribute에 Override 적용 
	const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Override, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
