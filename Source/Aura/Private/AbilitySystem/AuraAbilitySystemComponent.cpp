// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AuraGameplayTags.h"
#include "GameplayCueManager.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "Data/AuraInputConfig.h"
#include "Interface/PlayerInterface.h"
#include "GameFramework/PlayerController.h"

void UAuraAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);

	// Initial Caching
	if (!AuraInputConfig)
	{
		if (const IPlayerInterface* PlayerInterface = Cast<IPlayerInterface>(AbilityActorInfo.IsValid() ? AbilityActorInfo->PlayerController : nullptr))
		{
			AuraInputConfig = PlayerInterface->GetAuraInputConfig();
		}
	}
}

void UAuraAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();

	// Spell 변경 사항을 알린다.
	OnActivatableAbilitiesReplicatedDelegate.Broadcast();
}

void UAuraAbilitySystemComponent::OnAbilityFailed(const UGameplayAbility* Ability, const FGameplayTagContainer& FailureTags)
{
	const bool bFailDueToCost = FailureTags.HasTagExact(AuraGameplayTags::Activate_Fail_Cost); 
	const bool bFailDueToCooldown = FailureTags.HasTagExact(AuraGameplayTags::Activate_Fail_Cooldown);
	if (!bFailDueToCost && !bFailDueToCooldown)
	{
		return;
	}

	// Cost, Cooldown을 제외하고 실제 어빌리티를 사용할 수 있을 때만 화면에 표시
	if (!Ability || !Ability->DoesAbilitySatisfyTagRequirements(*this))
	{
		return;
	}
	
	const APawn* Pawn = Cast<APawn>(GetAvatarActor());
	if (AController* Controller = Pawn ? Pawn->GetController() : nullptr)
	{
		if (Controller->IsLocalPlayerController())
		{
			if (IPlayerInterface* PlayerInterface = Cast<IPlayerInterface>(Controller))
			{
				if (bFailDueToCost)
				{
					PlayerInterface->IndicateAbilityActivateCostFail();
				}
				else if (bFailDueToCooldown)
				{
					PlayerInterface->IndicateAbilityActivateCooldownFail();
				}
			}
		}
	}
}

void UAuraAbilitySystemComponent::AddAbilities(const TArray<TSubclassOf<UGameplayAbility>>& Abilities, int32 InLevel)
{
	for (const TSubclassOf<UGameplayAbility>& AbilityClass : Abilities)
	{
		FGameplayAbilitySpec AbilitySpec(AbilityClass, InLevel);
		if (const UAuraGameplayAbility* AuraGameplayAbility = Cast<UAuraGameplayAbility>(AbilitySpec.Ability))
		{
			if (AuraGameplayAbility->StartupInputTag.IsValid())
			{
				AbilitySpec.InputID = AuraInputConfig->GetInputIDForInputTag(AuraGameplayAbility->StartupInputTag);
			}
			GiveAbility(AbilitySpec);
		}
	}
}

void UAuraAbilitySystemComponent::AbilityInputPressed(int32 InputID)
{
	if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromInputID(InputID))
	{
		if (!AbilitySpec->IsActive())
		{
			TryActivateAbility(AbilitySpec->Handle);
		}
		AbilitySpecInputPressed(*AbilitySpec);
	}
}

void UAuraAbilitySystemComponent::AbilityInputReleased(int32 InputID)
{
	if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromInputID(InputID))
	{
		AbilitySpecInputReleased(*AbilitySpec);
	}
}

void UAuraAbilitySystemComponent::ServerUpgradeAttribute_Implementation(const FGameplayTag& AttributeTag)
{
	// Attribute Points가 있을 때, 1만큼 소모해 진행
	IPlayerInterface* PlayerInterface = CastChecked<IPlayerInterface>(GetAvatarActor());
	if (PlayerInterface->GetAttributePoints() <= 0)
	{
		return;
	}
	PlayerInterface->DecrementAttributePoints();
	
	if (const UAuraAttributeSet* AuraAS = Cast<UAuraAttributeSet>(GetAttributeSet(UAuraAttributeSet::StaticClass())))
	{
		if (AuraAS->TagToAttributeMap.Contains(AttributeTag))
		{
			// AttributeTag에 해당하는 Attribute의 값에 1을 더한다.
			const FGameplayAttribute& AttributeToUpgrade = AuraAS->TagToAttributeMap[AttributeTag];
			ApplyModToAttribute(AttributeToUpgrade, EGameplayModOp::Additive, 1.f);
		}
	}
}

void UAuraAbilitySystemComponent::ServerSpendPointButtonPressed_Implementation(const FGameplayTag& SpellTag, TSubclassOf<UGameplayAbility> SpellAbilityClass)
{
	// Spell Points가 있을 때, 1만큼 소모해 진행
	IPlayerInterface* PlayerInterface = CastChecked<IPlayerInterface>(GetAvatarActor());
	if (PlayerInterface->GetSpellPoints() <= 0)
	{
		return;
	}
	PlayerInterface->DecrementSpellPoints();
	
	if (IsSpellUnlocked(SpellTag))
	{
		UpgradeSpell(SpellTag);
	}
	else
	{
		checkf(SpellAbilityClass, TEXT("Need to set SpellAbilityClass for %s Tag"), *SpellTag.GetTagName().ToString());
		// 해당 Spell을 Unlock해 Give Ability
		UnlockSpell(SpellTag, SpellAbilityClass);
	}
}

bool UAuraAbilitySystemComponent::IsSpellUnlocked(const FGameplayTag& SpellTag)
{
	// Spell Ability가 Give되어 ActivatableAbilities에 존재하면 Unlock 된 것을 의미 
	return GetSpellSpecForSpellTag(SpellTag) != nullptr;
}

void UAuraAbilitySystemComponent::ActivateAllPassiveSpells()
{
	ABILITYLIST_SCOPE_LOCK();

	const FGameplayTagContainer TagContainer(AuraGameplayTags::Abilities_Passive);
	TArray<FGameplayAbilitySpec*> ActivatableAbilitySpecs;
	GetActivatableGameplayAbilitySpecsByAllMatchingTags(TagContainer, ActivatableAbilitySpecs);
	
	for (const FGameplayAbilitySpec* AbilitySpec : ActivatableAbilitySpecs)
	{
		TryActivateAbility(AbilitySpec->Handle);
	}
}

void UAuraAbilitySystemComponent::UnlockSpell(const FGameplayTag& SpellTag, const TSubclassOf<UGameplayAbility>& SpellAbilityClass)
{
	const FGameplayAbilitySpec SpellSpec(SpellAbilityClass);
	GiveAbility(SpellSpec);

	// Spell이 Unlock 됨을 전달해 UI에 표시
	OnSpellAbilityChangedDelegate.Broadcast(SpellTag);
}

void UAuraAbilitySystemComponent::UpgradeSpell(const FGameplayTag& SpellTag)
{
	if (FGameplayAbilitySpec* SpellSpec = GetSpellSpecForSpellTag(SpellTag))
	{
		SpellSpec->Level += 1;
		MarkAbilitySpecDirty(*SpellSpec);

		// Spell의 Level 변경을 전달
		OnSpellAbilityChangedDelegate.Broadcast(SpellTag);
	}
}

void UAuraAbilitySystemComponent::ServerHandleEquipSpell_Implementation(const FGameplayTag& SpellTagToEquip, const FGameplayTag& InputTag)
{
	FGameplayAbilitySpec* SpellSpecToEquip = GetSpellSpecForSpellTag(SpellTagToEquip);
	if (!SpellSpecToEquip)
	{
		return;
	}

	// Input에 이미 Spell이 장착되어 있다면
	const int32 EquippedInputID = AuraInputConfig->GetInputIDForInputTag(InputTag);
	if (FGameplayAbilitySpec* EquippedSpellSpec = FindAbilitySpecFromInputID(EquippedInputID))
	{
		// Input의 이미 장착된 Spell을 장착 해제
		UnEquipSpell(EquippedSpellSpec, InputTag, true);
		
		// 장착하려는 Spell이 Input에 이미 장착되어 있으므로 장착 해제만 수행하고 return
		if (EquippedSpellSpec == SpellSpecToEquip)
		{
			return;
		}
	}

	// 장착하고자 하는 Spell이 다른 Input에 장착되어 있다면, 그 Input에서 장착 해제
	const FGameplayTag PrevInputTag = AuraInputConfig->GetInputTagForInputID(SpellSpecToEquip->InputID);
	if (PrevInputTag.IsValid())
	{
		UnEquipSpell(SpellSpecToEquip, PrevInputTag, false);
	}
	
	// InputID를 설정해 장착
	SpellSpecToEquip->InputID = AuraInputConfig->GetInputIDForInputTag(InputTag);

	// InputTag에 Spell을 장착했음을 전달
	ClientBroadcastEquippedSpellChange(true, InputTag, SpellTagToEquip);

	MarkAbilitySpecDirty(*SpellSpecToEquip);

	// Passive Spell은 장착과 동시에 활성화
	if (SpellSpecToEquip->Ability->AbilityTags.HasTag(AuraGameplayTags::Abilities_Passive) && !SpellSpecToEquip->Ability->IsActive())
	{
		TryActivateAbility(SpellSpecToEquip->Handle);
	}
}

void UAuraAbilitySystemComponent::UnEquipSpell(FGameplayAbilitySpec* SpellSpecToUnEquip, const FGameplayTag& InputTagToRemove, bool bClearPassiveSpell)
{
	if (SpellSpecToUnEquip)
	{
		SpellSpecToUnEquip->InputID = INDEX_NONE;
		MarkAbilitySpecDirty(*SpellSpecToUnEquip);

		// InputTagToRemove이 나타내는 Input에 대한 UnEquip을 전달한다.
		ClientBroadcastEquippedSpellChange(false, InputTagToRemove, GetSpellTagForSpellSpec(SpellSpecToUnEquip));

		// Passive Spell은 장착과 동시에 실행되므로, 장착 해제 후 종료
		if (bClearPassiveSpell)
		{
			if (SpellSpecToUnEquip->Ability->AbilityTags.HasTag(AuraGameplayTags::Abilities_Passive))
			{
				CancelAbilitySpec(*SpellSpecToUnEquip, nullptr);
			}
		}
	}
}

void UAuraAbilitySystemComponent::ClientBroadcastEquippedSpellChange_Implementation(bool bEquipped, const FGameplayTag& InputTag, const FGameplayTag& SpellTag)
{
	OnEquippedSpellAbilityChangedDelegate.Broadcast(bEquipped, InputTag, SpellTag);
}

void UAuraAbilitySystemComponent::ApplyXPGainEffect(int32 XPAmount)
{
	const FGameplayEffectContextHandle EffectContextHandle = MakeEffectContext();

	// Make Instant Additive GameplayEffect
	if (UGameplayEffect* EffectDef = NewObject<UGameplayEffect>(GetTransientPackage(), TEXT("XPGainEffect")))
	{
		FGameplayModifierInfo ModifierInfo;
		ModifierInfo.Attribute = UAuraAttributeSet::GetXPAttribute();
		ModifierInfo.ModifierOp = EGameplayModOp::Additive;
		ModifierInfo.ModifierMagnitude = FScalableFloat(XPAmount);
		EffectDef->Modifiers.Add(ModifierInfo);
		EffectDef->DurationPolicy = EGameplayEffectDurationType::Instant;
		
		const FGameplayEffectSpec EffectSpec(EffectDef, EffectContextHandle, 1.f);
		ApplyGameplayEffectSpecToSelf(EffectSpec);		
	}
}

void UAuraAbilitySystemComponent::ApplyEffectSpecToSelf(const TSubclassOf<UGameplayEffect>& EffectClass, float Level)
{
	if (EffectClass)
	{
		const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(EffectClass, Level, MakeEffectContext());
		ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
	}
}

void UAuraAbilitySystemComponent::ApplyEffectSpecToSelfWithSetByCaller(const TSubclassOf<UGameplayEffect>& EffectClass, const FGameplayTag& DataTag,
	float Magnitude, float Level)
{
	if (EffectClass)
	{
		const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(EffectClass, Level, MakeEffectContext());
		SpecHandle.Data->SetSetByCallerMagnitude(DataTag, Magnitude);
		ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
	}
}

void UAuraAbilitySystemComponent::ClientExecuteGameplayCue_Implementation(const FGameplayTag& CueTag)
{
	if (GetAvatarActor())
	{
		UGameplayCueManager::ExecuteGameplayCue_NonReplicated(GetAvatarActor(), CueTag, FGameplayCueParameters());
	}
}

FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetSpellSpecForSpellTag(const FGameplayTag& SpellTag)
{
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.Ability->AbilityTags.HasTagExact(SpellTag))
		{
			return &AbilitySpec;
		}
	}
	return nullptr;
}

FGameplayTag UAuraAbilitySystemComponent::GetSpellTagForSpellSpec(const FGameplayAbilitySpec* SpellSpec)
{
	if (SpellSpec)
	{
		for (const FGameplayTag& Tag : SpellSpec->Ability->AbilityTags)
		{
			if (Tag.MatchesTag(AuraGameplayTags::Abilities_Offensive) || Tag.MatchesTag(AuraGameplayTags::Abilities_Passive))
			{
				return Tag;
			}
		}
	}
	return FGameplayTag::EmptyTag;
}

void UAuraAbilitySystemComponent::GetSpellAndInputTagPairs(TArray<TTuple<FGameplayTag, FGameplayTag>>& OutArray)
{
	for (FGameplayAbilitySpec& SpellSpec : GetActivatableAbilities())
	{
		const FGameplayTag SpellTag = GetSpellTagForSpellSpec(&SpellSpec);
		if (SpellTag.IsValid())
		{
			// 등록하지 않은 Spell은 InputTag가 없으므로 유효한지 체크하지 않고 Add
			const FGameplayTag InputTag = AuraInputConfig->GetInputTagForInputID(SpellSpec.InputID);
			OutArray.Add({ SpellTag, InputTag });
		}
	}
}

float UAuraAbilitySystemComponent::GetCooldownTimeRemaining(const FGameplayTag& CooldownTag) const
{
	if (HasMatchingGameplayTag(CooldownTag))
	{
		const FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(CooldownTag.GetSingleTagContainer());
		TArray<float> TimesRemaining = GetActiveEffectsTimeRemaining(Query);
		if (TimesRemaining.Num())
		{
			TimesRemaining.Sort();
			return TimesRemaining.Last();
		}
	}
	return 0.f;
}
