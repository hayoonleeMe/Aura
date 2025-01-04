// Fill out your copyright notice in the Description page of Project Settings.


#include "ModMagCalc/MMC_MaxMana.h"

#include "AbilitySystem/AuraAttributeSet.h"

UMMC_MaxMana::UMMC_MaxMana()
{
	IntelligenceDef.AttributeToCapture = UAuraAttributeSet::GetIntelligenceAttribute();
	IntelligenceDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	IntelligenceDef.bSnapshot = false;	// Attribute를 실시간으로 캡쳐
	RelevantAttributesToCapture.Add(IntelligenceDef);

	LevelDef.AttributeToCapture = UAuraAttributeSet::GetLevelAttribute();
	LevelDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	LevelDef.bSnapshot = false;
	RelevantAttributesToCapture.Add(LevelDef);
}

float UMMC_MaxMana::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// Gather tags from source and target
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;
	
	float Intelligence = 0.f;
	GetCapturedAttributeMagnitude(IntelligenceDef, Spec, EvaluateParameters, Intelligence);

	float Level = 0;
	GetCapturedAttributeMagnitude(LevelDef, Spec, EvaluateParameters, Level);
	
	return 50.f + 2.5f * Intelligence + 15.f * Level;
}
