
#include "AbilitySystem/AuraGameplayEffectContext.h"

FAuraGameplayEffectContext* FAuraGameplayEffectContext::ExtractEffectContext(FGameplayEffectContextHandle Handle)
{
	FGameplayEffectContext* EffectContext = Handle.Get();
	if (EffectContext && EffectContext->GetScriptStruct()->IsChildOf(FAuraGameplayEffectContext::StaticStruct()))
	{
		return static_cast<FAuraGameplayEffectContext*>(EffectContext);
	}
	return nullptr;
}

bool FAuraGameplayEffectContext::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	uint16 RepBits = 0;
	if (Ar.IsSaving())
	{
		if (bReplicateInstigator && Instigator.IsValid())
		{
			RepBits |= 1 << 0;
		}
		if (bReplicateEffectCauser && EffectCauser.IsValid() )
		{
			RepBits |= 1 << 1;
		}
		if (AbilityCDO.IsValid())
		{
			RepBits |= 1 << 2;
		}
		if (bReplicateSourceObject && SourceObject.IsValid())
		{
			RepBits |= 1 << 3;
		}
		if (Actors.Num() > 0)
		{
			RepBits |= 1 << 4;
		}
		if (HitResult.IsValid())
		{
			RepBits |= 1 << 5;
		}
		if (bHasWorldOrigin)
		{
			RepBits |= 1 << 6;
		}
		if (bCanBlockedHit && bIsBlockedHit)
		{
			RepBits |= 1 << 7;
		}
		if (!bCanBlockedHit)
		{
			RepBits |= 1 << 8;
		}
		if (bCanCriticalHit && bIsCriticalHit)
		{
			RepBits |= 1 << 9;
		}
		if (!bCanCriticalHit)
		{
			RepBits |= 1 << 10;
		}
		if (!bShouldActivateHitReact)
		{
			RepBits |= 1 << 11;
		}
	}

	Ar.SerializeBits(&RepBits, 12);

	if (RepBits & (1 << 0))
	{
		Ar << Instigator;
	}
	if (RepBits & (1 << 1))
	{
		Ar << EffectCauser;
	}
	if (RepBits & (1 << 2))
	{
		Ar << AbilityCDO;
	}
	if (RepBits & (1 << 3))
	{
		Ar << SourceObject;
	}
	if (RepBits & (1 << 4))
	{
		SafeNetSerializeTArray_Default<31>(Ar, Actors);
	}
	if (RepBits & (1 << 5))
	{
		if (Ar.IsLoading())
		{
			if (!HitResult.IsValid())
			{
				HitResult = TSharedPtr<FHitResult>(new FHitResult());
			}
		}
		HitResult->NetSerialize(Ar, Map, bOutSuccess);
	}
	if (RepBits & (1 << 6))
	{
		Ar << WorldOrigin;
		bHasWorldOrigin = true;
	}
	else
	{
		bHasWorldOrigin = false;
	}
	// bIsBlockedHit
	if (RepBits & (1 << 7))
	{
		Ar << bIsBlockedHit;
	}
	// bCanBlockedHit
	if (RepBits & (1 << 8))
	{
		Ar << bCanBlockedHit;
	}
	// bIsCriticalHit
	if (RepBits & (1 << 9))
	{
		Ar << bIsCriticalHit;
	}
	// bCanCriticalHit
	if (RepBits & (1 << 10))
	{
		Ar << bCanCriticalHit;
	}
	// bShouldActivateHitReact
	if (RepBits & (1 << 11))
	{
		Ar << bShouldActivateHitReact;
	}

	if (Ar.IsLoading())
	{
		AddInstigator(Instigator.Get(), EffectCauser.Get()); // Just to initialize InstigatorAbilitySystemComponent
	}	
	
	bOutSuccess = true;
	return true;
}
