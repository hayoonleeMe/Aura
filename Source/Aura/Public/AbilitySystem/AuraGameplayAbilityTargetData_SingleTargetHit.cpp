
#include "AuraGameplayAbilityTargetData_SingleTargetHit.h"

bool FAuraGameplayAbilityTargetData_SingleTargetHit::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	FGameplayAbilityTargetData_SingleTargetHit::NetSerialize(Ar, Map, bOutSuccess);

	// Simple Serialize
	Ar << CursorTarget;

	return true;
}
