// Fill out your copyright notice in the Description page of Project Settings.


#include "ModMagCalc/MMC_MaxHealth.h"

#include "AbilitySystem/AuraAttributeSet.h"

UMMC_MaxHealth::UMMC_MaxHealth()
{
	VigorDef.AttributeToCapture = UAuraAttributeSet::GetVigorAttribute();
	VigorDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	VigorDef.bSnapshot = false;	// Attribute를 실시간으로 캡쳐
	RelevantAttributesToCapture.Add(VigorDef);

	LevelDef.AttributeToCapture = UAuraAttributeSet::GetLevelAttribute();
	LevelDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	LevelDef.bSnapshot = false;
	RelevantAttributesToCapture.Add(LevelDef);
}

float UMMC_MaxHealth::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// Gather tags from source and target
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;
	
	float Vigor = 0.f;
	GetCapturedAttributeMagnitude(VigorDef, Spec, EvaluateParameters, Vigor);

	float Level = 0;
	GetCapturedAttributeMagnitude(LevelDef, Spec, EvaluateParameters, Level);

	return 80.f + 2.5f * Vigor + 10.f * Level;
}
