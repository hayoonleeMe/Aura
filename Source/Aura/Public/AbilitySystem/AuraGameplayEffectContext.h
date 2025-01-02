
#pragma once

#include "GameplayEffectTypes.h"

#include "AuraGameplayEffectContext.generated.h"

/**
 * GameplayEffectContext에 추가적인 데이터를 저장하기 위한 child struct
 */
USTRUCT()
struct FAuraGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()

public:
	FAuraGameplayEffectContext()
		: bIsBlockedHit(false), bIsCriticalHit(false)
	{}
	
	bool IsBlockedHit() const { return bIsBlockedHit; }
	bool IsCriticalHit() const { return bIsCriticalHit; }

	void SetIsBlockedHit(bool bInIsBlockedHit) { bIsBlockedHit = bInIsBlockedHit; }
	void SetIsCriticalHit(bool bInIsCriticalHit) { bIsCriticalHit = bInIsCriticalHit; }

	// Handle로부터 FAuraGameplayEffectContext 포인터를 반환한다.
	// 구할 수 없으면 nullptr를 반환한다.
	static FAuraGameplayEffectContext* ExtractEffectContext(FGameplayEffectContextHandle Handle);

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FAuraGameplayEffectContext::StaticStruct();
	}

	virtual FGameplayEffectContext* Duplicate() const override
	{
		FAuraGameplayEffectContext* NewContext = new FAuraGameplayEffectContext();
		*NewContext = *this;
		if (GetHitResult())
		{
			// Does a deep copy of the hit result
			NewContext->AddHitResult(*GetHitResult(), true);
		}
		return NewContext;
	}

	virtual bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess) override;

protected:
	// Target의 Block Chance에 의해 데미지가 경감됐는지를 나타냄
	UPROPERTY()
	bool bIsBlockedHit;

	// Source의 Critical Hit Chance에 의해 데미지가 증가됐는지를 나타냄
	UPROPERTY()
	bool bIsCriticalHit;
};

template<>
struct TStructOpsTypeTraits<FAuraGameplayEffectContext> : public TStructOpsTypeTraitsBase2<FAuraGameplayEffectContext>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true
	};
};
