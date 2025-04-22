// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/GameOverlay.h"

#include "AuraBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Animation/WidgetAnimation.h"
#include "Components/NamedSlot.h"
#include "Components/VerticalBox.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"
#include "UI/Widget/AttributeMenu.h"
#include "UI/Widget/GameEndAlert.h"
#include "UI/Widget/MenuShortcutAlert.h"
#include "UI/Widget/PauseMenu.h"
#include "UI/Widget/RespawnTimer.h"
#include "UI/Widget/SpellMenu.h"
#include "UI/Widget/StageInfoHUD.h"
#include "UI/Widget/StageStartAlert.h"
#include "UI/Widget/StageWaitingTimer.h"
#include "UI/Widget/TutorialMenu.h"

UGameOverlay::UGameOverlay(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	MenuShortcutAlertDisplayTime = 7.f;
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
	check(PreStageHUDClass);
	check(MenuShortcutAlertClass);

	/* Game Overlay */
	AAuraPlayerController* AuraPC = CastChecked<AAuraPlayerController>(GetOwningPlayer());
	AuraPC->OnStageStatusChangedDelegate.BindUObject(this, &ThisClass::OnStageStatusChanged);
	AuraPC->OnTotalEnemyCountChangedDelegate.BindUObject(this, &ThisClass::OnTotalEnemyCountChanged);
	AuraPC->OnRespawnStartedDelegate.AddUObject(this, &ThisClass::OnRespawnStarted);
	AuraPC->OnGameEndDelegate.BindUObject(this, &ThisClass::OnGameEnd);

	/* Attribute Menu */
	AAuraPlayerState* AuraPS = GetOwningPlayerState<AAuraPlayerState>(true);
	AuraPS->OnAttributePointsChangedDelegate.AddUObject(this, &ThisClass::OnAttributePointsChanged);
	AuraPS->OnSpellPointsChangedDelegate.AddUObject(this, &ThisClass::OnSpellPointsChanged);

	UAuraAbilitySystemComponent* AuraASC = UAuraBlueprintLibrary::GetAuraAbilitySystemComponentChecked(GetOwningPlayerState());
	UAuraAttributeSet* AuraAS = UAuraBlueprintLibrary::GetAuraAttributeSetChecked(GetOwningPlayerState());
	
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

void UGameOverlay::CloseAllMenu()
{
	if (AttributeMenu)
	{
		AttributeMenu->CloseMenu();
		AttributeMenu = nullptr;
	}
	if (SpellMenu)
	{
		SpellMenu->CloseMenu();
		SpellMenu = nullptr;
	}
	if (PauseMenu)
	{
		PauseMenu->CloseMenu();
		PauseMenu = nullptr;
	}
	if (TutorialMenu)
	{
		TutorialMenu->CloseMenu();
		TutorialMenu = nullptr;
	}
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
		AttributeMenu->CloseMenu();
		AttributeMenu = nullptr;
	}
	else
	{
		if (SpellMenu)
		{
			// Remove SpellMenu if exists
			SpellMenu->CloseMenu();
			SpellMenu = nullptr;
		}
		if (TutorialMenu)
		{
			// Remove TutorialMenu if exists
			TutorialMenu->CloseMenu();
			TutorialMenu = nullptr;
		}
		
		// Open AttributeMenu
		AttributeMenu = CreateWidget<UAttributeMenu>(this, AttributeMenuClass);
		AttributeMenu->GetOnRemovedDelegate().AddUObject(this, &ThisClass::OnAttributeMenuClosed);
		AttributeMenu->SetAlignmentInViewport({ 0.5f, 0.5f });
		if (GEngine && GEngine->GameViewport)
		{
			FVector2D ViewportSize;
			GEngine->GameViewport->GetViewportSize(ViewportSize);
			AttributeMenu->SetPositionInViewport(ViewportSize * 0.5f);
		}
		AttributeMenu->AddToViewport();
	}
}

void UGameOverlay::OnAttributeMenuClosed()
{
	AttributeMenu = nullptr;
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
		SpellMenu->CloseMenu();
		SpellMenu = nullptr;
	}
	else
	{
		if (AttributeMenu)
		{
			// Remove AttributeMenu if exists
			AttributeMenu->CloseMenu();
			AttributeMenu = nullptr;
		}
		if (TutorialMenu)
		{
			// Remove TutorialMenu if exists
			TutorialMenu->CloseMenu();
			TutorialMenu = nullptr;
		}
		
		// Open SpellMenu
		SpellMenu = CreateWidget<USpellMenu>(this, SpellMenuClass);
		SpellMenu->GetOnRemovedDelegate().AddUObject(this, &ThisClass::OnSpellMenuClosed);
		SpellMenu->SetAlignmentInViewport({ 0.5f, 0.5f });
		if (GEngine && GEngine->GameViewport)
		{
			FVector2D ViewportSize;
			GEngine->GameViewport->GetViewportSize(ViewportSize);
			SpellMenu->SetPositionInViewport(ViewportSize * 0.5f);
		}
		SpellMenu->AddToViewport();
	}
}

void UGameOverlay::OnSpellMenuClosed()
{
	SpellMenu = nullptr;
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
		SpellMenu->UpdateEquippedSpellChange(bEquipped, InputTag, SpellTag);
	}
}

void UGameOverlay::OpenPauseMenu()
{
	// Open PauseMenu
	PauseMenu = CreateWidget<UPauseMenu>(this, PauseMenuClass);
	PauseMenu->GetOnRemovedDelegate().AddUObject(this, &ThisClass::OnPauseMenuClosed);
	PauseMenu->AddToViewport(1);

	// GameOverlay 숨김
	SetVisibility(ESlateVisibility::Collapsed);
}

void UGameOverlay::OnPauseMenuClosed()
{
	PauseMenu = nullptr;
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UGameOverlay::OpenTutorialMenu()
{
	if (PauseMenu)
	{
		return;
	}
	
	if (TutorialMenu)
	{
		TutorialMenu->CloseMenu();
		TutorialMenu = nullptr;
	}
	else
	{
		if (AttributeMenu)
		{
			// Remove AttributeMenu if exists
			AttributeMenu->CloseMenu();
			AttributeMenu = nullptr;
		}
		if (SpellMenu)
		{
			// Remove AttributeMenu if exists
			SpellMenu->CloseMenu();
			SpellMenu = nullptr;
		}
		
		// Open TutorialMenu
		TutorialMenu = CreateWidget<UTutorialMenu>(this, TutorialMenuClass);
		TutorialMenu->GetOnRemovedDelegate().AddUObject(this, &ThisClass::OnTutorialMenuClosed);
		TutorialMenu->SetAlignmentInViewport({ 0.5f, 0.5f });
		if (GEngine && GEngine->GameViewport)
		{
			FVector2D ViewportSize;
			GEngine->GameViewport->GetViewportSize(ViewportSize);
			TutorialMenu->SetPositionInViewport(ViewportSize * 0.5f);
		}
		TutorialMenu->AddToViewport();
	}
}

void UGameOverlay::OnTutorialMenuClosed()
{
	TutorialMenu = nullptr;
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
		// Remove PreStageHUD Widget
		if (PreStageHUD)
		{
			PreStageHUD->RemoveFromParent();
			PreStageHUD = nullptr;
		}
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
		StageInfoHUD->InitializeStageInfoHUD(StageNumber, TotalEnemyCount);
		StageInfoHUD->AddToViewport();

		// NamedSlot_StageWaiting 위치에 StageInfo widget 표시
		NamedSlot_StageWaiting->AddChild(StageInfoHUD);
	}
}

void UGameOverlay::OnTotalEnemyCountChanged(int32 TotalEnemyCount) const
{
	if (StageInfoHUD)
	{
		StageInfoHUD->UpdateTotalEnemyCount(TotalEnemyCount);
	}
}

void UGameOverlay::ShowPreStageHUD()
{
	PreStageHUD = CreateWidget(this, PreStageHUDClass);
	PreStageHUD->AddToViewport();

	// NamedSlot_StageWaiting 위치에 PreStageHUD widget 표시
	NamedSlot_StageWaiting->AddChild(PreStageHUD);
}

void UGameOverlay::ShowAllMenuShortcutAlert(const TArray<TTuple<EGameMenuType, FKey>>& MenuKeys)
{
	for (const TTuple<EGameMenuType, FKey>& Tuple : MenuKeys)
	{
		if (UMenuShortcutAlert* AlertWidget = CreateWidget<UMenuShortcutAlert>(this, MenuShortcutAlertClass))
		{
			AlertWidget->SetText(Tuple.Key, Tuple.Value);
			Box_MenuShortcutAlert->AddChildToVerticalBox(AlertWidget);
		}
	}

	PlayAnimationForward(MenuShortcutAlertSlide);
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateUObject(this, &ThisClass::OnMenuShortcutAlertDisplayTimeEnded), MenuShortcutAlertDisplayTime, false);
}

void UGameOverlay::OnMenuShortcutAlertDisplayTimeEnded()
{
	PlayAnimationForward(MenuShortcutAlertFadeIn);
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this]()
	{
		Box_MenuShortcutAlert->RemoveFromParent();
		Box_MenuShortcutAlert = nullptr;
	}), MenuShortcutAlertFadeIn->GetEndTime(), false);
}
