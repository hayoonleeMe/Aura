// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraGameplayAbility.h"

#include "AbilitySystem/AuraAttributeSet.h"

UAuraGameplayAbility::UAuraGameplayAbility()
{
	bUseTriggeredEvent = true;
	UnlockRequiredLevel = 1;
	MaxSpellLevel = 4;

	/* Spell Stack */
	bUseSpellStack = false;
}

void UAuraGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	if (InstancingPolicy != EGameplayAbilityInstancingPolicy::InstancedPerActor)
	{
		return;
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	const FGameplayTagContainer* CooldownTags = GetCooldownTags();
	if (!ASC || !CooldownTags || CooldownTags->IsEmpty())
	{
		return;
	}

	// Cooldown Tag Change Event 등록
	const FGameplayTag CooldownTag = CooldownTags->First();
	ASC->RegisterGameplayTagEvent(CooldownTag).AddUObject(this, &ThisClass::OnCooldownTagCountChanged);

	if (bUseSpellStack)
	{
		// 초기화
		CurrentStackCount = GetMaxStackCountByLevel();
	}
}

void UAuraGameplayAbility::OnCooldownTagCountChanged(const FGameplayTag Tag, int32 Count)
{
	if (Count > 0)
	{
		/* Cooldown Started */
		OnSpellCooldownDelegate.Broadcast(GetCooldown(GetAbilityLevel()));
	}
	else
	{
		/* Cooldown Ended */
		OnSpellCooldownDelegate.Broadcast(-1.f);

		if (bUseSpellStack)
		{
			UpdateSpellStack(1);

			// Apply Pending Cooldown
			if (CurrentStackCount < GetMaxStackCountByLevel())
			{
				if (HasAuthority(&CurrentActivationInfo))
				{
					ApplyCooldown(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo());
				}
			}	
		}
	}
}

FOnSpellCooldownSignature* UAuraGameplayAbility::GetOnSpellCooldownDelegate()
{
	const FGameplayTagContainer* CooldownTags = GetCooldownTags();
	return (CooldownTags && CooldownTags->Num()) ? &OnSpellCooldownDelegate : nullptr;
}

FText UAuraGameplayAbility::GetLockedDescription() const
{
	return FText::FromString(FString::Printf(TEXT("<Default>Requires Level </><Level>%d </><Default>to unlock.</>"), UnlockRequiredLevel));
}

FText UAuraGameplayAbility::GetMaxLevelDescription()
{
	return FText::FromString(TEXT("<Default>Max Level Reached.</>"));
}

float UAuraGameplayAbility::GetManaCost(int32 Level) const
{
	float ManaCost = 0.f;
	if (UGameplayEffect* CostEffect = GetCostGameplayEffect())
	{
		for (const FGameplayModifierInfo& ModifierInfo : CostEffect->Modifiers)
		{
			if (ModifierInfo.Attribute == UAuraAttributeSet::GetManaAttribute())
			{
				ModifierInfo.ModifierMagnitude.GetStaticMagnitudeIfPossible(Level, ManaCost);
				break;
			}
		}
	}
	return FMath::Abs(ManaCost);  
}

float UAuraGameplayAbility::GetCooldown(int32 Level) const
{
	float Cooldown = 0.f;
	if (const UGameplayEffect* CooldownEffect = GetCooldownGameplayEffect())
	{
		CooldownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(Level, Cooldown);
	}
	return Cooldown;
}

FOnSpellStackChangedSignature* UAuraGameplayAbility::GetOnSpellStackChangedDelegate()
{
	return bUseSpellStack ? &OnSpellStackChangedDelegate : nullptr;
}

bool UAuraGameplayAbility::CheckCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                         FGameplayTagContainer* OptionalRelevantTags) const
{
	// 남은 스택이 있으면 항상 통과
	if (bUseSpellStack && CurrentStackCount > 0)
	{
		return true;
	}
	return Super::CheckCooldown(Handle, ActorInfo, OptionalRelevantTags);
}

bool UAuraGameplayAbility::CommitAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, FGameplayTagContainer* OptionalRelevantTags)
{
	if (!bUseSpellStack)
	{
		return Super::CommitAbility(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags);
	}

	// Spell Stack을 사용하면 Cooldown 적용 중일 때도 스펠을 사용할 수 있기 때문에 Cooldown과 Cost를 따로 수행한다.
	bool bCommitted = false;

	// ThisClass::CheckCooldown()은 스택이 남아있으면 무조건 통과시키므로 CommitAbilityCooldown()으로 쿨다운을 적용하면 항상 Effect가 적용되는 문제가 있다.
	// 따라서 직접 Super::CheckCooldown()으로 체크하고 쿨다운을 적용한다.
	if (Super::CheckCooldown(Handle, ActorInfo, nullptr))
	{
		ApplyCooldown(Handle, ActorInfo, ActivationInfo);
		bCommitted |= true;
	}
	bCommitted |= CommitAbilityCost(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags);

	if (bCommitted)
	{
		// Broadcast this commitment
		ActorInfo->AbilitySystemComponent->NotifyAbilityCommit(this);
	}
	
	return bCommitted;
}

int32 UAuraGameplayAbility::GetSpellStackCount() const
{
	return bUseSpellStack ? CurrentStackCount : -1;
}

void UAuraGameplayAbility::UpdateSpellStack(int32 AmountToAdd)
{
	CurrentStackCount += AmountToAdd;
	OnSpellStackChangedDelegate.Broadcast(CurrentStackCount);
}

int32 UAuraGameplayAbility::GetMaxStackCountByLevel() const
{
	return MaxStackCountCurve.GetValueAtLevel(GetAbilityLevel());
}
