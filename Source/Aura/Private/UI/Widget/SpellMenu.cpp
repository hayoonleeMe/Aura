// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/SpellMenu.h"

#include "AuraBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "GameplayAbilitySpec.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "Components/Button.h"
#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"
#include "Data/SpellConfig.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"
#include "UI/Widget/EquippedSpellGlobeButton.h"
#include "UI/Widget/EquippedSpellRow.h"
#include "UI/Widget/FramedValue.h"
#include "UI/Widget/OffensiveSpellTree.h"
#include "UI/Widget/PassiveSpellTree.h"
#include "UI/Widget/SpellDescription.h"
#include "UI/Widget/SpellGlobeButton.h"
#include "UI/Widget/SquareButton.h"

void USpellMenu::CloseMenu()
{
	Super::CloseMenu();

	RemoveFromParent();
}

void USpellMenu::NativeConstruct()
{
	Super::NativeConstruct();

	check(SpellEquipSound);

	if (const IPlayerInterface* PlayerInterface = Cast<IPlayerInterface>(GetOwningPlayer()))
	{
		SpellConfig = PlayerInterface->GetSpellConfig();
	}

	/* Spell Menu */
	Button_SpendPoint->InternalButton->OnClicked.AddDynamic(this, &ThisClass::OnSpendPointButtonClicked);
	Button_Equip->InternalButton->OnClicked.AddDynamic(this, &ThisClass::OnEquipButtonClicked);
	Button_Close->InternalButton->OnClicked.AddDynamic(this, &ThisClass::CloseMenu);

	AAuraPlayerState* AuraPS = GetOwningPlayerState<AAuraPlayerState>(true);
	AuraPS->OnSpellPointsChangedDelegate.AddUObject(this, &ThisClass::UpdateSpellPointsChange);

	/* Spell Tree */
	AuraASC = UAuraBlueprintLibrary::GetAuraAbilitySystemComponentChecked(GetOwningPlayerState());
	AuraASC->OnActivatableAbilitiesReplicatedDelegate.AddUObject(this, &ThisClass::OnSpellGiven);
	
	OffensiveSpellTree->GlobeButton_1->OnSpellGlobeButtonSelectedDelegate.BindUObject(this, &ThisClass::OnSpellGlobeButtonSelected);
	OffensiveSpellTree->GlobeButton_2->OnSpellGlobeButtonSelectedDelegate.BindUObject(this, &ThisClass::OnSpellGlobeButtonSelected);
	OffensiveSpellTree->GlobeButton_3->OnSpellGlobeButtonSelectedDelegate.BindUObject(this, &ThisClass::OnSpellGlobeButtonSelected);
	OffensiveSpellTree->GlobeButton_4->OnSpellGlobeButtonSelectedDelegate.BindUObject(this, &ThisClass::OnSpellGlobeButtonSelected);
	OffensiveSpellTree->GlobeButton_5->OnSpellGlobeButtonSelectedDelegate.BindUObject(this, &ThisClass::OnSpellGlobeButtonSelected);
	OffensiveSpellTree->GlobeButton_6->OnSpellGlobeButtonSelectedDelegate.BindUObject(this, &ThisClass::OnSpellGlobeButtonSelected);
	OffensiveSpellTree->GlobeButton_7->OnSpellGlobeButtonSelectedDelegate.BindUObject(this, &ThisClass::OnSpellGlobeButtonSelected);
	OffensiveSpellTree->GlobeButton_8->OnSpellGlobeButtonSelectedDelegate.BindUObject(this, &ThisClass::OnSpellGlobeButtonSelected);
	OffensiveSpellTree->GlobeButton_9->OnSpellGlobeButtonSelectedDelegate.BindUObject(this, &ThisClass::OnSpellGlobeButtonSelected);

	PassiveSpellTree->GlobeButton_HaloOfProtection->OnSpellGlobeButtonSelectedDelegate.BindUObject(this, &ThisClass::OnSpellGlobeButtonSelected);
	PassiveSpellTree->GlobeButton_HealthSiphon->OnSpellGlobeButtonSelectedDelegate.BindUObject(this, &ThisClass::OnSpellGlobeButtonSelected);
	PassiveSpellTree->GlobeButton_ManaSiphon->OnSpellGlobeButtonSelectedDelegate.BindUObject(this, &ThisClass::OnSpellGlobeButtonSelected);

	SpellGlobeButtons =
	{
		OffensiveSpellTree->GlobeButton_1,
		OffensiveSpellTree->GlobeButton_2,
		OffensiveSpellTree->GlobeButton_3,
		OffensiveSpellTree->GlobeButton_4,
		OffensiveSpellTree->GlobeButton_5,
		OffensiveSpellTree->GlobeButton_6,
		OffensiveSpellTree->GlobeButton_7,
		OffensiveSpellTree->GlobeButton_8,
		OffensiveSpellTree->GlobeButton_9,
		PassiveSpellTree->GlobeButton_HaloOfProtection,
		PassiveSpellTree->GlobeButton_HealthSiphon,
		PassiveSpellTree->GlobeButton_ManaSiphon
	};

	/* Equipped Spell Row */
	EquippedSpellRow->GlobeButton_LMB->OnEquippedSpellGlobeButtonSelectedDelegate.BindUObject(this, &ThisClass::OnEquippedSpellGlobeButtonSelected);
	EquippedSpellRow->GlobeButton_MMB->OnEquippedSpellGlobeButtonSelectedDelegate.BindUObject(this, &ThisClass::OnEquippedSpellGlobeButtonSelected);
	EquippedSpellRow->GlobeButton_Q->OnEquippedSpellGlobeButtonSelectedDelegate.BindUObject(this, &ThisClass::OnEquippedSpellGlobeButtonSelected);
	EquippedSpellRow->GlobeButton_W->OnEquippedSpellGlobeButtonSelectedDelegate.BindUObject(this, &ThisClass::OnEquippedSpellGlobeButtonSelected);
	EquippedSpellRow->GlobeButton_E->OnEquippedSpellGlobeButtonSelectedDelegate.BindUObject(this, &ThisClass::OnEquippedSpellGlobeButtonSelected);
	EquippedSpellRow->GlobeButton_R->OnEquippedSpellGlobeButtonSelectedDelegate.BindUObject(this, &ThisClass::OnEquippedSpellGlobeButtonSelected);
	EquippedSpellRow->GlobeButton_Passive_1->OnEquippedSpellGlobeButtonSelectedDelegate.BindUObject(this, &ThisClass::OnEquippedSpellGlobeButtonSelected);
	EquippedSpellRow->GlobeButton_Passive_2->OnEquippedSpellGlobeButtonSelectedDelegate.BindUObject(this, &ThisClass::OnEquippedSpellGlobeButtonSelected);
	
	EquippedSpellGlobeButtons =
	{
		EquippedSpellRow->GlobeButton_LMB,
		EquippedSpellRow->GlobeButton_MMB,
		EquippedSpellRow->GlobeButton_Q,
		EquippedSpellRow->GlobeButton_W,
		EquippedSpellRow->GlobeButton_E,
		EquippedSpellRow->GlobeButton_R,
		EquippedSpellRow->GlobeButton_Passive_1,
		EquippedSpellRow->GlobeButton_Passive_2
	};

	BroadcastInitialValues();
}

void USpellMenu::BroadcastInitialValues()
{
	// Spell Points 값 전달
	const AAuraPlayerState* AuraPS = GetOwningPlayerState<AAuraPlayerState>(true);
	UpdateSpellPointsChange(AuraPS->GetSpellPoints());

	// Spells, Equipped Spells 초기화
	TArray<TTuple<FGameplayTag, FGameplayTag>> StartupSpells;
	AuraASC->GetSpellAndInputTagPairs(StartupSpells);
	
	for (const TTuple<FGameplayTag, FGameplayTag>& Tuple : StartupSpells)
	{
		UpdateSpellChange(Tuple.Key);
		if (Tuple.Value.IsValid())
		{
			UpdateEquippedSpellChange(true, Tuple.Value, Tuple.Key);
		}
	}
}

void USpellMenu::OnSpellGiven()
{
	// 서버에서 Spell을 Unlock - GiveAbility 또는 Upgrade - Increase Spec Level을 수행해 ActivatableAbilities가 변경되고 클라이언트로 Replicate 될 때 호출된다.
	// 따라서 서버에서 Spell을 변경했음을 클라이언트에서 알 수 있다.
	if (CurrentSelectedSpellGlobeButton)
	{
		UpdateSpellChange(CurrentSelectedSpellGlobeButton->SpellTag);
	}
}

void USpellMenu::UpdateSpellPointsChange(int32 Value)
{
	bHasSpellPoints = Value > 0;
	FramedValue_SpellPoints->Text_Value->SetText(FText::AsNumber(Value));
}

bool USpellMenu::CanSpendPoint() const
{
	// Spell Point가 없거나 선택한 Spell Globe가 없으므로 불가능
	if (!bHasSpellPoints || !CurrentSelectedSpellGlobeButton)
	{
		return false;
	}

	if (const FGameplayAbilitySpec* SpellSpec = AuraASC->GetSpellSpecForSpellTag(CurrentSelectedSpellGlobeButton->SpellTag))
	{
		// Spell Spec을 가져올 수 있으면 이미 Unlock 된 것을 의미
		// Unlock 된 상태라면, Spell을 더 레벨업 할 수 있는지 반환
		const UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(SpellSpec->Ability);
		return AuraAbility && SpellSpec->Level < AuraAbility->MaxSpellLevel; 
	}

	if (const TSubclassOf<UGameplayAbility>& SpellAbilityClass = SpellConfig->GetSpellInfoByTag(CurrentSelectedSpellGlobeButton->SpellTag).SpellAbilityClass)
	{
		if (const UAuraGameplayAbility* AuraAbilityCDO = SpellAbilityClass->GetDefaultObject<UAuraGameplayAbility>())
		{
			// 현재 플레이어의 레벨이 Spell을 Unlock 할 수 있는 레벨인지 반환
			const int32 PlayerLevel = UAuraBlueprintLibrary::GetAuraAttributeSetChecked(GetOwningPlayerState())->GetLevel();
			return PlayerLevel >= AuraAbilityCDO->UnlockRequiredLevel;
		}
	}

	return false;
}

void USpellMenu::OnSpendPointButtonClicked()
{
	const FSpellInfo& SpellInfo = SpellConfig->GetSpellInfoByTag(CurrentSelectedSpellGlobeButton->SpellTag);
	AuraASC->ServerSpendPointButtonPressed(CurrentSelectedSpellGlobeButton->SpellTag, SpellInfo.SpellAbilityClass);
}

void USpellMenu::OnEquipButtonClicked()
{
	if (EquippedSpellRow->bWaitSelectGlobe)
	{
		EquippedSpellRow->FinishSpellEquip();
	}
	else
	{
		EquippedSpellRow->WaitSelectGlobe(IsSelectedSpellOffensive());
	}
}

bool USpellMenu::CanEquipSpell() const
{
	return CurrentSelectedSpellGlobeButton && AuraASC->IsSpellUnlocked(CurrentSelectedSpellGlobeButton->SpellTag);
}

void USpellMenu::EnableButtons() const
{
	Button_SpendPoint->SetIsEnabled(CanSpendPoint());
	Button_Equip->SetIsEnabled(CanEquipSpell());
}

void USpellMenu::UpdateDescription() const
{
	if (CurrentSelectedSpellGlobeButton)
	{
		if (const FGameplayAbilitySpec* SpellSpec = AuraASC->GetSpellSpecForSpellTag(CurrentSelectedSpellGlobeButton->SpellTag))
		{
			// SpellSpec을 구할 수 있다는 것은 해당 Spell이 이미 Unlock 됐다는 것을 의미
			if (const UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(SpellSpec->Ability))
			{
				// Spell이 최대 레벨이면 Next Level Description은 표시 X
				const FText NextLevelDescriptionText = SpellSpec->Level < AuraAbility->MaxSpellLevel ? AuraAbility->GetDescription(SpellSpec->Level + 1) : AuraAbility->GetMaxLevelDescription();
				const FText DescriptionText = AuraAbility->GetDescription(SpellSpec->Level);

				SpellDescription->RichText_Description->SetText(DescriptionText);
				NextLevelSpellDescription->RichText_Description->SetText(NextLevelDescriptionText);
			}
		}
		else
		{
			if (const TSubclassOf<UGameplayAbility>& SpellAbilityClass = SpellConfig->GetSpellInfoByTag(CurrentSelectedSpellGlobeButton->SpellTag).SpellAbilityClass)
			{
				// Spell이 잠긴 상태
				if (const UAuraGameplayAbility* AuraAbilityCDO = Cast<UAuraGameplayAbility>(SpellAbilityClass.GetDefaultObject()))
				{
					SpellDescription->RichText_Description->SetText(AuraAbilityCDO->GetLockedDescription());
					NextLevelSpellDescription->RichText_Description->SetText(AuraAbilityCDO->GetDescription(1));
				}
			}
			else
			{
				// 미구현 Spell
				SpellDescription->RichText_Description->SetText(FText::FromString(TEXT("<Default>This spell is not implemented yet and will be added in a future update.</>")));
				NextLevelSpellDescription->RichText_Description->SetText(FText());
			}
		}
	}
	else
	{
		// Spell을 선택하지 않았으므로 Description 표시 X
		SpellDescription->RichText_Description->SetText(FText());
		NextLevelSpellDescription->RichText_Description->SetText(FText());
	}
}

void USpellMenu::OnSpellGlobeButtonSelected(USpellGlobeButton* SelectedSpellGlobeButton)
{
	CurrentSelectedSpellGlobeButton = SelectedSpellGlobeButton->bSelected ? SelectedSpellGlobeButton : nullptr;

	// 하나의 Spell Globe Button만 선택
	for (USpellGlobeButton* SpellGlobeButton : SpellGlobeButtons)
	{
		if (SpellGlobeButton != SelectedSpellGlobeButton && SpellGlobeButton->bSelected)
		{
			SpellGlobeButton->Deselect(false);
		}
	}

	EnableButtons();
	UpdateDescription();
}

bool USpellMenu::IsSelectedSpellOffensive() const
{
	return CurrentSelectedSpellGlobeButton->SpellTag.MatchesTag(AuraGameplayTags::Abilities_Offensive);
}

void USpellMenu::UpdateSpellChange(const FGameplayTag& SpellTag)
{
	const FSpellInfo& SpellInfo = SpellConfig->GetSpellInfoByTag(SpellTag);
	for (const USpellGlobeButton* SpellGlobeButton : SpellGlobeButtons)
	{
		if (SpellGlobeButton->SpellTag.MatchesTagExact(SpellTag))
		{
			SpellGlobeButton->UpdateSpellChange(SpellInfo);
			EnableButtons();
			UpdateDescription();
			return;
		}
	}
}

void USpellMenu::OnEquippedSpellGlobeButtonSelected(const FGameplayTag& InputTag)
{
	if (EquippedSpellRow->bWaitSelectGlobe && CurrentSelectedSpellGlobeButton && InputTag.IsValid())
	{
		PlaySound(SpellEquipSound);
		AuraASC->ServerHandleEquipSpell(CurrentSelectedSpellGlobeButton->SpellTag, InputTag);
	}
}

void USpellMenu::UpdateEquippedSpellChange(bool bEquipped, const FGameplayTag& InputTag, const FGameplayTag& SpellTag)
{
	const FSpellInfo& SpellInfo = SpellConfig->GetSpellInfoByTag(SpellTag);
	for (const UEquippedSpellGlobeButton* EquippedSpellGlobeButton : EquippedSpellGlobeButtons)
	{
		if (EquippedSpellGlobeButton->InputTag.MatchesTagExact(InputTag))
		{
			EquippedSpellGlobeButton->UpdateEquippedSpellChange(bEquipped, SpellInfo);
		}
	}

	EquippedSpellRow->FinishSpellEquip();
}
