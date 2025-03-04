// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/GameOverlay.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Components/Button.h"
#include "Components/NamedSlot.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"
#include "UI/Widget/AttributeMenu.h"
#include "UI/Widget/GameEndAlert.h"
#include "UI/Widget/PauseMenu.h"
#include "UI/Widget/RespawnTimer.h"
#include "UI/Widget/SpellMenu.h"
#include "UI/Widget/SquareButton.h"
#include "UI/Widget/StageInfoHUD.h"
#include "UI/Widget/StageStartAlert.h"
#include "UI/Widget/StageWaitingTimer.h"

UGameOverlay::UGameOverlay(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsFocusable(true);
}

void UGameOverlay::NativeConstruct()
{
	Super::NativeConstruct();

	check(RespawnTimerClass);
	check(GameEndAlertClass);
	check(AttributeMenuClass);
	check(SpellMenuClass);
	check(PauseMenuClass);
	check(StageInfoHUDClass);
	check(StageWaitingTimerClass);
	check(StageStartAlertClass);

	/* Game Overlay */
	Button_AttributeMenu->InternalButton->OnClicked.AddDynamic(this, &ThisClass::OpenAttributeMenu);
	Button_SpellMenu->InternalButton->OnClicked.AddDynamic(this, &ThisClass::OpenSpellMenu);
	Button_PauseMenu->InternalButton->OnClicked.AddDynamic(this, &ThisClass::OpenPauseMenu);

	AAuraPlayerController* AuraPC = CastChecked<AAuraPlayerController>(GetOwningPlayer());
	AuraPC->OnStageStatusChangedDelegate.BindUObject(this, &ThisClass::OnStageStatusChanged);
	AuraPC->OnRespawnStartedDelegate.AddUObject(this, &ThisClass::OnRespawnStarted);
	AuraPC->OnGameEndDelegate.BindUObject(this, &ThisClass::OnGameEnd);

	/* Attribute Menu */
	AAuraPlayerState* AuraPS = GetOwningPlayerState<AAuraPlayerState>(true);
	AuraPS->OnAttributePointsChangedDelegate.AddUObject(this, &ThisClass::OnAttributePointsChanged);
	AuraPS->OnSpellPointsChangedDelegate.AddUObject(this, &ThisClass::OnSpellPointsChanged);

	UAuraAbilitySystemComponent* AuraASC = GetOwnerAuraAbilitySystemComponentChecked();
	UAuraAttributeSet* AuraAS = GetOwnerAuraAttributeSetChecked();
	
	for (const TTuple<FGameplayTag, FGameplayAttribute>& Pair : AuraAS->TagToAttributeMap)
	{
		AuraASC->GetGameplayAttributeValueChangeDelegate(Pair.Value).AddWeakLambda(this, [this, Pair](const FOnAttributeChangeData& AttributeChangeData)
		{
			if (AttributeMenu)
			{
				AttributeMenu->UpdateAttributeValueChange(Pair.Key, AttributeChangeData.NewValue);
			}
		});
	}

	/* Spell Menu */
	AuraASC->OnSpellAbilityChangedDelegate.AddUObject(this, &ThisClass::OnSpellChanged);
	AuraASC->OnEquippedSpellAbilityChangedDelegate.AddUObject(this, &ThisClass::OnEquippedSpellChanged);
}

void UGameOverlay::OnRespawnStarted(float RespawnTimerEndSeconds)
{
	if (RespawnTimerEndSeconds > 0.0)
	{
		URespawnTimer* RespawnTimer = CreateWidget<URespawnTimer>(this, RespawnTimerClass);
		RespawnTimer->RespawnTimerEndSeconds = RespawnTimerEndSeconds;
		RespawnTimer->AddToViewport();

		// NamedSlot_StageStartAlert 위치에 RespawnTimer widget 표시
		NamedSlot_StageStartAlert->AddChild(RespawnTimer);	// TODO : StageStartAlert와 겹칠 수 있으므로 처리 필요
	}
}

void UGameOverlay::OnGameEnd()
{
	// GameEnd를 알리는 위젯 표시
	UUserWidget* GameEndAlert = CreateWidget(this, GameEndAlertClass);
	GameEndAlert->AddToViewport();
	
	// Hide other widgets if exists
	if (AttributeMenu)
	{
		AttributeMenu->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (SpellMenu)
	{
		SpellMenu->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (PauseMenu)
	{
		PauseMenu->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (StageStartAlert)
	{
		StageStartAlert->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (StageWaitingTimer)
	{
		StageWaitingTimer->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (StageInfoHUD)
	{
		StageInfoHUD->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UGameOverlay::OpenAttributeMenu()
{
	if (PauseMenu)
	{
		return;
	}
	
	if (AttributeMenu)
	{
		AttributeMenu->RemoveFromParent();
		AttributeMenu = nullptr;
		SetInGameInputMode();
	}
	else
	{
		if (SpellMenu)
		{
			// Remove SpellMenu if exists
			SpellMenu->RemoveFromParent();
			SpellMenu = nullptr;
		}
		
		// Open AttributeMenu
		AttributeMenu = CreateWidget<UAttributeMenu>(this, AttributeMenuClass);
		AttributeMenu->OnRemovedDelegate.AddUObject(this, &ThisClass::OnAttributeMenuClosed);
		AttributeMenu->SetAlignmentInViewport({ 0.5f, 0.5f });
		if (GEngine && GEngine->GameViewport)
		{
			FVector2D ViewportSize;
			GEngine->GameViewport->GetViewportSize(ViewportSize);
			AttributeMenu->SetPositionInViewport(ViewportSize * 0.5f);
		}
		AttributeMenu->AddToViewport();

		// Set Input Mode
		SetUIInputMode(AttributeMenu);
	}
}

void UGameOverlay::OnAttributeMenuClosed()
{
	AttributeMenu = nullptr;
	SetInGameInputMode();
}

void UGameOverlay::OnAttributePointsChanged(int32 Value) const
{
	if (AttributeMenu)
	{
		AttributeMenu->UpdateAttributePointsChange(Value);
	}
}

void UGameOverlay::OpenSpellMenu()
{
	if (PauseMenu)
	{
		return;
	}
	
	if (SpellMenu)
	{
		SpellMenu->RemoveFromParent();
		SpellMenu = nullptr;
		SetInGameInputMode();
	}
	else
	{
		if (AttributeMenu)
		{
			// Remove AttributeMenu if exists
			AttributeMenu->RemoveFromParent();
			AttributeMenu = nullptr;
		}
		
		// Open SpellMenu
		SpellMenu = CreateWidget<USpellMenu>(this, SpellMenuClass);
		SpellMenu->OnRemovedDelegate.AddUObject(this, &ThisClass::OnSpellMenuClosed);
		SpellMenu->SetAlignmentInViewport({ 0.5f, 0.5f });
		if (GEngine && GEngine->GameViewport)
		{
			FVector2D ViewportSize;
			GEngine->GameViewport->GetViewportSize(ViewportSize);
			SpellMenu->SetPositionInViewport(ViewportSize * 0.5f);
		}
		SpellMenu->AddToViewport();

		// Set Input Mode
		SetUIInputMode(SpellMenu);
	}
}

void UGameOverlay::OnSpellMenuClosed()
{
	SpellMenu = nullptr;
	SetInGameInputMode();
}

void UGameOverlay::OnSpellPointsChanged(int32 Value) const
{
	if (SpellMenu)
	{
		SpellMenu->UpdateSpellPointsChange(Value);
	}
}

void UGameOverlay::OnSpellChanged(const FGameplayTag& SpellTag)
{
	if (SpellMenu)
	{
		SpellMenu->UpdateSpellChange(SpellTag);
	}
}

void UGameOverlay::OnEquippedSpellChanged(bool bEquipped, const FGameplayTag& InputTag, const FGameplayTag& SpellTag)
{
	if (SpellMenu)
	{
		SpellMenu->UpdateEquippedSpellChange(bEquipped, InputTag, SpellTag, true);
	}
}

void UGameOverlay::OpenPauseMenu()
{
	if (AttributeMenu)
	{
		AttributeMenu->RemoveFromParent();
		AttributeMenu = nullptr;
	}
	if (SpellMenu)
	{
		SpellMenu->RemoveFromParent();
		SpellMenu = nullptr;
	}
	
	// Open PauseMenu
	PauseMenu = CreateWidget<UPauseMenu>(this, PauseMenuClass);
	PauseMenu->OnRemovedDelegate.AddUObject(this, &ThisClass::OnPauseMenuClosed);
	PauseMenu->AddToViewport(1);

	// GameOverlay 숨김
	SetRenderOpacity(0.f);
	
	// Set Input Mode
	SetUIInputMode(PauseMenu);
}

void UGameOverlay::OnPauseMenuClosed()
{
	PauseMenu = nullptr;
	SetInGameInputMode();
	SetRenderOpacity(1.f);
}

void UGameOverlay::OnStageStatusChanged(EStageStatus StageStatus, int32 StageNumber, double WaitingTimerEndSeconds, int32 TotalEnemyCount)
{
	if (StageStatus == EStageStatus::Waiting)
	{
		// Remove another stage widgets
		if (StageStartAlert)
		{
			StageStartAlert->RemoveFromParent();
			StageStartAlert = nullptr;
		}
		if (StageInfoHUD)
		{
			StageInfoHUD->RemoveFromParent();
			StageInfoHUD = nullptr;
		}

		StageWaitingTimer = CreateWidget<UStageWaitingTimer>(this, StageWaitingTimerClass);
		StageWaitingTimer->StageNumber = StageNumber;
		StageWaitingTimer->WaitingTimerEndSeconds = WaitingTimerEndSeconds;
		StageWaitingTimer->AddToViewport();

		// NamedSlot_StageWaiting 위치에 StageWaiting widget 표시 
		NamedSlot_StageWaiting->AddChild(StageWaitingTimer);
	}
	else
	{
		// Remove StageWaiting widget
		if (StageWaitingTimer)
		{
			StageWaitingTimer->RemoveFromParent();
			StageWaitingTimer = nullptr;
		}

		StageStartAlert = CreateWidget<UStageStartAlert>(this, StageStartAlertClass);
		StageStartAlert->StageNumber = StageNumber;
		StageStartAlert->AddToViewport();

		// NamedSlot_StageStartAlert 위치에 StageStartAlert widget 표시
		NamedSlot_StageStartAlert->AddChild(StageStartAlert);

		StageInfoHUD = CreateWidget<UStageInfoHUD>(this, StageInfoHUDClass);
		StageInfoHUD->StageNumber = StageNumber;
		StageInfoHUD->EnemyCount = TotalEnemyCount;
		StageInfoHUD->AddToViewport();

		// NamedSlot_StageWaiting 위치에 StageInfo widget 표시
		NamedSlot_StageWaiting->AddChild(StageInfoHUD);
	}
}
