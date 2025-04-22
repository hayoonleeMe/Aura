// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/HealthManaSpells.h"

#include "AuraBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Data/SpellConfig.h"
#include "Player/AuraPlayerController.h"
#include "UI/Widget/EquippedSpellGlobe.h"
#include "GameFramework/PlayerState.h"

void UHealthManaSpells::NativeConstruct()
{
	Super::NativeConstruct();

	Globe_LMB->InputTag = AuraGameplayTags::InputTag_LMB;
	Globe_MMB->InputTag = AuraGameplayTags::InputTag_MMB;
	Globe_Q->InputTag = AuraGameplayTags::InputTag_Q;
	Globe_W->InputTag = AuraGameplayTags::InputTag_W;
	Globe_E->InputTag = AuraGameplayTags::InputTag_E;
	Globe_R->InputTag = AuraGameplayTags::InputTag_R;
	Globe_Passive_1->InputTag = AuraGameplayTags::InputTag_Passive_1;
	Globe_Passive_2->InputTag = AuraGameplayTags::InputTag_Passive_2;
	
	InputTagToSpellGlobeMap.Add(Globe_LMB->InputTag, Globe_LMB);
	InputTagToSpellGlobeMap.Add(Globe_MMB->InputTag, Globe_MMB);
	InputTagToSpellGlobeMap.Add(Globe_Q->InputTag, Globe_Q);
	InputTagToSpellGlobeMap.Add(Globe_W->InputTag, Globe_W);
	InputTagToSpellGlobeMap.Add(Globe_E->InputTag, Globe_E);
	InputTagToSpellGlobeMap.Add(Globe_R->InputTag, Globe_R);
	InputTagToSpellGlobeMap.Add(Globe_Passive_1->InputTag, Globe_Passive_1);
	InputTagToSpellGlobeMap.Add(Globe_Passive_2->InputTag, Globe_Passive_2);

	if (const IPlayerInterface* PlayerInterface = Cast<IPlayerInterface>(GetOwningPlayer()))
	{
		SpellConfig = PlayerInterface->GetSpellConfig();
	}

	UAuraAbilitySystemComponent* AuraASC = UAuraBlueprintLibrary::GetAuraAbilitySystemComponentChecked(GetOwningPlayerState());
	AuraASC->OnEquippedSpellAbilityChangedDelegate.AddUObject(this, &ThisClass::OnEquippedSpellChanged);

	BroadcastInitialValues();
}

void UHealthManaSpells::BroadcastInitialValues()
{
	UAuraAbilitySystemComponent* AuraASC = UAuraBlueprintLibrary::GetAuraAbilitySystemComponentChecked(GetOwningPlayerState());
	TArray<TTuple<FGameplayTag, FGameplayTag>> StartupSpells;
	AuraASC->GetSpellAndInputTagPairs(StartupSpells);
	
	for (const TTuple<FGameplayTag, FGameplayTag>& Tuple : StartupSpells)
	{
		OnEquippedSpellChanged(true, Tuple.Value, Tuple.Key);
	}
}

void UHealthManaSpells::OnEquippedSpellChanged(bool bEquipped, const FGameplayTag& InputTag, const FGameplayTag& SpellTag)
{
	UEquippedSpellGlobe* SpellGlobe = InputTagToSpellGlobeMap.FindChecked(InputTag);
	SpellGlobe->UpdateEquippedSpellChange(bEquipped, SpellConfig->GetSpellInfoByTag(SpellTag));

	UpdateEquippedSpellCooldown(bEquipped, SpellTag, SpellGlobe);
}

void UHealthManaSpells::UpdateEquippedSpellCooldown(bool bEquipped, const FGameplayTag& SpellTag, UEquippedSpellGlobe* SpellGlobe)
{
	UAuraAbilitySystemComponent* AuraASC = UAuraBlueprintLibrary::GetAuraAbilitySystemComponentChecked(GetOwningPlayerState());
	if (const FGameplayAbilitySpec* SpellSpec = AuraASC->GetSpellSpecForSpellTag(SpellTag))
	{
		// Spell Cooldown Tag는 하나만 존재한다고 가정
		const FGameplayTagContainer* CooldownTags = SpellSpec->Ability->GetCooldownTags();
		const FGameplayTag SpellCooldownTag = CooldownTags ? CooldownTags->First() : FGameplayTag::EmptyTag;
		
		if (SpellCooldownTag.IsValid())
		{
			FOnGameplayEffectTagCountChanged& CooldownTagEvent = AuraASC->RegisterGameplayTagEvent(SpellCooldownTag, EGameplayTagEventType::NewOrRemoved);
		
			if (bEquipped)
			{
				// CooldownTag Event 등록
				CooldownTagEvent.AddWeakLambda(this, [this, SpellGlobe, AuraASC](const FGameplayTag SpellCooldownTag, int32 Count)
				{
					if (Count > 0)
					{
						// Spell을 실행한 뒤 Cooldown이 적용되면, InputTag에 해당하는 EquippedGlobe Widget에서 Timer를 설정해 일정 시간마다 남은 Cooldown Time을 업데이트해 표시
						SpellGlobe->UpdateEquippedSpellCooldownStart(AuraASC->GetCooldownTimeRemaining(SpellCooldownTag));
					}
					else
					{
						// Cooldown End
						SpellGlobe->UpdateEquippedSpellCooldownEnd();
					}
				});
			
				// Spell을 장착할 때 이미 Cooldown Tag가 존재하므로 바로 UI에 업데이트
				if (AuraASC->HasMatchingGameplayTag(SpellCooldownTag))
				{
					SpellGlobe->UpdateEquippedSpellCooldownStart(AuraASC->GetCooldownTimeRemaining(SpellCooldownTag));
				}
			}
			else
			{
				// CooldownTag에 대해 등록된 Event 제거
				CooldownTagEvent.RemoveAll(this);
			}	
		}
	}
}
