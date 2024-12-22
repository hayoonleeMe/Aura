// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "Data/SpellConfig.h"
#include "Player/AuraPlayerState.h"

void USpellMenuWidgetController::BroadcastInitialValues()
{
	// SpellPoints 값 전달
	const AAuraPlayerState* AuraPS = GetAuraPlayerStateChecked();
	OnSpellPointsChangedDelegate.Broadcast(AuraPS->GetSpellPoints());
}

void USpellMenuWidgetController::BindCallbacksToDependencies()
{
	// SpellPoints가 변경되면 그 값을 전달
	AAuraPlayerState* AuraPS = GetAuraPlayerStateChecked();
	AuraPS->OnSpellPointsChangedDelegate.AddWeakLambda(this, [this](int32 Value)
	{
		OnSpellPointsChangedDelegate.Broadcast(Value);
	});
}

bool USpellMenuWidgetController::IsSelectedSpellOffensive() const
{
	return SelectedSpellTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Abilities.Offensive")));
}


void USpellMenuWidgetController::SelectSpellGlobe(const FGameplayTag& SpellTag)
{
	if (SelectedSpellTag.MatchesTagExact(SpellTag))
	{
		// 선택했던 Spell Globe를 선택 해제하고 초기화
		SelectedSpellTag = FGameplayTag::EmptyTag;
		
		// 기존 Spell Globe 선택 해제 및 Sound 재생
		SelectSpellGlobeDelegate.Broadcast(false, SpellTag, true);
	}
	else
	{
		// 기존 Spell Globe 선택 해제
		SelectSpellGlobeDelegate.Broadcast(false, SelectedSpellTag, false);
		
		// 새 Spell Globe 선택
		SelectedSpellTag = SpellTag;
		SelectSpellGlobeDelegate.Broadcast(true, SelectedSpellTag, false);
	}
}

void USpellMenuWidgetController::DeselectSpellGlobe()
{
	if (SelectedSpellTag.IsValid())
	{
		const FGameplayTag DupSelectedSpellTag = SelectedSpellTag;
		SelectedSpellTag = FGameplayTag::EmptyTag;
		SelectSpellGlobeDelegate.Broadcast(false, DupSelectedSpellTag, false);
	}
}
