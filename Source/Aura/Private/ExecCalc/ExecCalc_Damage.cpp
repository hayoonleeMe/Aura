// Fill out your copyright notice in the Description page of Project Settings.


#include "ExecCalc/ExecCalc_Damage.h"

#include "AuraBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Interaction/CombatInterface.h"

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
		const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
		const AuraDamageStatics& DamageStatics = GetDamageStatics();
		Map.Add(GameplayTags.Attributes_Secondary_Armor, DamageStatics.ArmorDef);
		Map.Add(GameplayTags.Attributes_Secondary_ArmorPenetration, DamageStatics.ArmorPenetrationDef);
		Map.Add(GameplayTags.Attributes_Secondary_BlockChance, DamageStatics.BlockChanceDef);
		Map.Add(GameplayTags.Attributes_Secondary_CriticalHitChance, DamageStatics.CriticalHitChanceDef);
		Map.Add(GameplayTags.Attributes_Secondary_CriticalHitDamage, DamageStatics.CriticalHitDamageDef);
		Map.Add(GameplayTags.Attributes_Secondary_CriticalHitResistance, DamageStatics.CriticalHitResistanceDef);
		Map.Add(GameplayTags.Attributes_Secondary_FireResistance, DamageStatics.FireResistanceDef);
		Map.Add(GameplayTags.Attributes_Secondary_LightningResistance, DamageStatics.LightningResistanceDef);
		Map.Add(GameplayTags.Attributes_Secondary_ArcaneResistance, DamageStatics.ArcaneResistanceDef);
		Map.Add(GameplayTags.Attributes_Secondary_PhysicalResistance, DamageStatics.PhysicalResistanceDef);
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
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	const AuraDamageStatics& DamageStatics = GetDamageStatics();
	const TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition>& TagToCaptureDefMap = GetTagToCaptureDefMap();

	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	// Player Level
	const int32 SourceCharacterLevel = ICombatInterface::Execute_GetCharacterLevel(SourceAvatar);
	const int32 TargetCharacterLevel = ICombatInterface::Execute_GetCharacterLevel(TargetAvatar);

	// Get Damage SetByCaller Magnitude
	float Damage = 0.f;

	// 이 Damage Effect를 적용할 때 설정한 SetByCaller 값을 찾아 저항값을 적용한 데미지를 합산한다.
	for (const TTuple<FGameplayTag, FGameplayTag>& Tuple : GameplayTags.DamageTypeToResistanceMap)
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

	// Target BlockChance
	float TargetBlockChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics.BlockChanceDef, EvaluateParameters, TargetBlockChance);
	
	// If Block, halve the Damage
	const bool bBlocked = FMath::RandRange(1, 100) <= TargetBlockChance;
	Damage = bBlocked ? Damage * 0.5f : Damage;
	// TODO : Set to Custom EffectContext

	// Damage 계산식에 사용되는 계수를 Level에 따른 Curve로 저장하는 CurveTable
	const UCurveTable* DamageCalculationCoefficients = UAuraBlueprintLibrary::GetDamageCalculationCoefficients(SourceAvatar);
	check(DamageCalculationCoefficients);

	// Target Armor & Source ArmorPenetration
	float TargetArmor = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics.ArmorDef, EvaluateParameters, TargetArmor);

	float SourceArmorPenetration = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics.ArmorPenetrationDef, EvaluateParameters, SourceArmorPenetration);

	// ArmorPenetration ignores a percentage of the Target's Armor
	const FRealCurve* ArmorPenetrationCurve = DamageCalculationCoefficients->FindCurve(TEXT("ArmorPenetration"), FString());
	check(ArmorPenetrationCurve);
	const float ArmorPenetrationCoefficient = ArmorPenetrationCurve->Eval(SourceCharacterLevel);
	const float EffectiveArmor = TargetArmor * (100 - SourceArmorPenetration * ArmorPenetrationCoefficient) / 100.f;

	// Armor ignores a percentage of incoming Damage
	const FRealCurve* EffectiveArmorCurve = DamageCalculationCoefficients->FindCurve(TEXT("EffectiveArmor"), FString());
	check(EffectiveArmorCurve);
	const float EffectiveArmorCoefficient = EffectiveArmorCurve->Eval(TargetCharacterLevel);
	Damage *= (100 - EffectiveArmor * EffectiveArmorCoefficient) / 100.f;

	// Critical Hit
	float SourceCriticalHitChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics.CriticalHitChanceDef, EvaluateParameters, SourceCriticalHitChance);

	float SourceCriticalHitDamage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics.CriticalHitDamageDef, EvaluateParameters, SourceCriticalHitDamage);

	float TargetCriticalHitResistance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics.CriticalHitResistanceDef, EvaluateParameters, TargetCriticalHitResistance);

	const FRealCurve* CriticalHitResistanceCurve = DamageCalculationCoefficients->FindCurve(TEXT("CriticalHitResistance"), FString());
	check(CriticalHitResistanceCurve);
	const float CriticalHitResistanceCoefficient = CriticalHitResistanceCurve->Eval(TargetCharacterLevel);

	// Critical Hit Resistance reduces Critical Hit Chance by a certain percentage
	const float EffectiveCriticalHitChance = SourceCriticalHitChance - TargetCriticalHitResistance * CriticalHitResistanceCoefficient;
	const bool bCriticalHit = FMath::RandRange(1, 100) <= EffectiveCriticalHitChance;
	// Double damage plus a bonus if critical hit
	Damage = bCriticalHit ? Damage * 2.f + SourceCriticalHitDamage : Damage; 
	// TODO : Set to Custom EffectContext

	// 최종 Damage를 IncomingDamage Attribute에 Override 적용 
	const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Override, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}