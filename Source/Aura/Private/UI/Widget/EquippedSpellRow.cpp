// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/EquippedSpellRow.h"

#include "AuraGameplayTags.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "UI/Widget/EquippedSpellGlobeButton.h"

void UEquippedSpellRow::NativeConstruct()
{
	Super::NativeConstruct();

	GlobeButton_LMB->InputTag = AuraGameplayTags::InputTag_LMB;
	GlobeButton_MMB->InputTag = AuraGameplayTags::InputTag_MMB;
	GlobeButton_Q->InputTag = AuraGameplayTags::InputTag_Q;
	GlobeButton_W->InputTag = AuraGameplayTags::InputTag_W;
	GlobeButton_E->InputTag = AuraGameplayTags::InputTag_E;
	GlobeButton_R->InputTag = AuraGameplayTags::InputTag_R;
	
	GlobeButton_Passive_1->InputTag = AuraGameplayTags::InputTag_Passive_1;
	GlobeButton_Passive_2->InputTag = AuraGameplayTags::InputTag_Passive_2;
}

void UEquippedSpellRow::WaitSelectGlobe(bool bWaitOffensive)
{
	bWaitSelectGlobe = true;
	
	if (bWaitOffensive)
	{
		Image_OffensiveSelection->SetVisibility(ESlateVisibility::Visible);
		PlayAnimation(SelectOffensiveAnimation,0.f, 0);	// Loop

		GlobeButton_LMB->Button_Ring->SetIsEnabled(true);
		GlobeButton_MMB->Button_Ring->SetIsEnabled(true);
		GlobeButton_Q->Button_Ring->SetIsEnabled(true);
		GlobeButton_W->Button_Ring->SetIsEnabled(true);
		GlobeButton_E->Button_Ring->SetIsEnabled(true);
		GlobeButton_R->Button_Ring->SetIsEnabled(true);
	}
	else
	{
		Image_PassiveSelection->SetVisibility(ESlateVisibility::Visible);
		PlayAnimation(SelectPassiveAnimation,0.f, 0);	// Loop
		
		GlobeButton_Passive_1->Button_Ring->SetIsEnabled(true);
		GlobeButton_Passive_2->Button_Ring->SetIsEnabled(true);
	}
}

void UEquippedSpellRow::FinishSpellEquip()
{
	bWaitSelectGlobe = false;

	StopAllAnimations();
	Image_OffensiveSelection->SetVisibility(ESlateVisibility::Collapsed);
	Image_PassiveSelection->SetVisibility(ESlateVisibility::Collapsed);

	GlobeButton_LMB->Button_Ring->SetIsEnabled(false);
	GlobeButton_MMB->Button_Ring->SetIsEnabled(false);
	GlobeButton_Q->Button_Ring->SetIsEnabled(false);
	GlobeButton_W->Button_Ring->SetIsEnabled(false);
	GlobeButton_E->Button_Ring->SetIsEnabled(false);
	GlobeButton_R->Button_Ring->SetIsEnabled(false);

	GlobeButton_Passive_1->Button_Ring->SetIsEnabled(false);
	GlobeButton_Passive_2->Button_Ring->SetIsEnabled(false);
}
