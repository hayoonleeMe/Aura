// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraAbility_Debuff_Ignite.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraGameplayEffectContext.h"

UAuraAbility_Debuff_Ignite::UAuraAbility_Debuff_Ignite()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
}

void UAuraAbility_Debuff_Ignite::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                               const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	checkf(DamageEffectClass, TEXT("Need to set DamageEffectClass"));

	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	
	// Damage Effect를 적용할 Ability System Component 결정
	// Instigator가 유효하지 않아도 Ignite Damage를 주기 위해 Target의 ASC로 계속 수행
	const UAbilitySystemComponent* InstigatorASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Cast<AActor>(GetSourceObject(Handle, ActorInfo)));
	const UAbilitySystemComponent* ASC = InstigatorASC ? InstigatorASC : GetAbilitySystemComponentFromActorInfo_Checked();

	const FGameplayEffectContextHandle EffectContextHandle = ASC->MakeEffectContext();
	if (FAuraGameplayEffectContext* AuraEffectContext = FAuraGameplayEffectContext::ExtractEffectContext(EffectContextHandle))
	{
		// Debuff_Ignite로는 BlockedHit, CriticalHit X
		AuraEffectContext->SetCanBlockedHit(false);
		AuraEffectContext->SetCanCriticalHit(false);
		// Debuff_Ignite로는 HitReact 실행 X
		AuraEffectContext->SetShouldActivateHitReact(false);
	}
	
	const FGameplayEffectSpecHandle EffectSpecHandle = ASC->MakeOutgoingSpec(DamageEffectClass, 1.f, EffectContextHandle);

	// 이 Ability를 실행할 때 Incoming Damage를 전달하기 위해 AbilitySpec의 SetByCallerTagMagnitudes 사용
	float IgniteDamage = 0.f;
	if (FGameplayAbilitySpec* Spec = GetCurrentAbilitySpec())
	{
		if (Spec->SetByCallerTagMagnitudes.Contains(GameplayTags.Damage_Type_Fire))
		{
			IgniteDamage = Spec->SetByCallerTagMagnitudes[GameplayTags.Damage_Type_Fire] * DamageRate;
		}
	}
	
	// Assign Damage
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(EffectSpecHandle, GameplayTags.Damage_Type_Fire, IgniteDamage);
	
	GetAbilitySystemComponentFromActorInfo_Checked()->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data);

	K2_EndAbility();
}
