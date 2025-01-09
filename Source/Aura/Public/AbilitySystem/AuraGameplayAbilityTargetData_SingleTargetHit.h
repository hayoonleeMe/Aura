
#pragma once

#include "Abilities/GameplayAbilityTargetTypes.h"

#include "AuraGameplayAbilityTargetData_SingleTargetHit.generated.h"

/**
 * CursorTarget을 함께 보내기 위해 sub struct
 */
USTRUCT()
struct FAuraGameplayAbilityTargetData_SingleTargetHit : public FGameplayAbilityTargetData_SingleTargetHit
{
	GENERATED_BODY()

	FAuraGameplayAbilityTargetData_SingleTargetHit()
	{ }

	FAuraGameplayAbilityTargetData_SingleTargetHit(const FHitResult& InHitResult)
		: FGameplayAbilityTargetData_SingleTargetHit(InHitResult)
	{ }

	UPROPERTY()
	TWeakObjectPtr<AActor> CursorTarget;

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FAuraGameplayAbilityTargetData_SingleTargetHit::StaticStruct();
	}
};

template<>
struct TStructOpsTypeTraits<FAuraGameplayAbilityTargetData_SingleTargetHit> : public TStructOpsTypeTraitsBase2<FAuraGameplayAbilityTargetData_SingleTargetHit>
{
	enum
	{
		WithNetSerializer = true	// For now this is REQUIRED for FGameplayAbilityTargetDataHandle net serialization to work
	};
};
