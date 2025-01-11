
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
		: bIsBlockedHit(false), bCanBlockedHit(true), bIsCriticalHit(false), bCanCriticalHit(true), bShouldActivateHitReact(true), DebuffChance(0.f)
	{}
	
	bool IsBlockedHit() const { return bIsBlockedHit; }
	bool CanBlockedHit() const { return bCanBlockedHit; }
	bool IsCriticalHit() const { return bIsCriticalHit; }
	bool CanCriticalHit() const { return bCanCriticalHit; }
	bool ShouldActivateHitReact() const { return bShouldActivateHitReact; }
	FGameplayTag GetDebuffTag() const { return DebuffTag; }
	float GetDebuffChance() const { return DebuffChance; }

	void SetIsBlockedHit(bool bInIsBlockedHit) { bIsBlockedHit = bInIsBlockedHit; }
	void SetCanBlockedHit(bool bInCanBlockedHit) { bCanBlockedHit = bInCanBlockedHit; }
	void SetIsCriticalHit(bool bInIsCriticalHit) { bIsCriticalHit = bInIsCriticalHit; }
	void SetCanCriticalHit(bool bInCanCriticalHit) { bCanCriticalHit = bInCanCriticalHit; }
	void SetShouldActivateHitReact(bool bShouldActivate) { bShouldActivateHitReact = bShouldActivate; }
	void SetDebuffTag(const FGameplayTag& Tag) { DebuffTag = Tag; }
	void SetDebuffChance(float Chance) { DebuffChance = Chance; }

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

	// Blocked Hit이 발생할 수 있는지를 나타냄
	UPROPERTY()
	bool bCanBlockedHit;

	// Source의 Critical Hit Chance에 의해 데미지가 증가됐는지를 나타냄
	UPROPERTY()
	bool bIsCriticalHit;

	// Critical Hit이 발생할 수 있는지를 나타냄
	UPROPERTY()
	bool bCanCriticalHit;

	// HitReact를 실행하는지 여부를 나타냄
	UPROPERTY()
	bool bShouldActivateHitReact;

	// Target에 적용할 Debuff를 나타내는 Tag
	// Debuff는 서버에서만 수행하므로 Serialize 필요 X
	UPROPERTY()
	FGameplayTag DebuffTag;
	
	// Target에 Debuff를 적용할 확률
	// Debuff는 서버에서만 수행하므로 Serialize 필요 X
	UPROPERTY()
	float DebuffChance;
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
