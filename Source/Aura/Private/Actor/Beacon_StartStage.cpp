// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Beacon_StartStage.h"

#include "Aura/Aura.h"
#include "Component/LevelSequenceManageComponent.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Game/StageGameMode.h"
#include "Interface/PlayerInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UI/HUD/AuraHUD.h"

ABeacon_StartStage::ABeacon_StartStage()
{
	bReplicates = true;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Component"));
	SetRootComponent(BoxComponent);
	
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	MeshComponent->SetupAttachment(BoxComponent);
	MeshComponent->SetCustomDepthStencilValue(CUSTOM_DEPTH_STENCIL_BLUE);
	MeshComponent->SetCollisionResponseToChannel(ECC_Target, ECR_Block);
	MeshComponent->SetRelativeLocation(FVector(0.f, 0.f, -160.f));

	/* Tooltip */
	TooltipWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Tooltip Widget Component"));
	TooltipWidgetComponent->SetupAttachment(BoxComponent);
	TooltipWidgetComponent->SetVisibility(false);
	TooltipWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	TooltipWidgetComponent->SetDrawAtDesiredSize(true);
	TooltipWidgetComponent->SetPivot(FVector2D(0.f, 0.5f));
	TooltipWidgetComponent->SetRelativeLocation(FVector(0.f, 50.f, 0.f));

	MeshComponent->SetCanEverAffectNavigation(true);
	BoxComponent->SetCanEverAffectNavigation(false);
	TooltipWidgetComponent->SetCanEverAffectNavigation(false);
}

void ABeacon_StartStage::HighlightActor()
{
	MeshComponent->SetRenderCustomDepth(true);

	if (!bHasInteractedWithPlayer)
	{
		TooltipWidgetComponent->SetVisibility(true);
	}
}

void ABeacon_StartStage::UnHighlightActor()
{
	MeshComponent->SetRenderCustomDepth(false);

	TooltipWidgetComponent->SetVisibility(false);
}

void ABeacon_StartStage::Interact()
{
	if (!bHasInteractedWithPlayer)
	{
		bHasInteractedWithPlayer = true;
		
		if (InteractSound)
		{
			UGameplayStatics::PlaySound2D(this, InteractSound);
		}

		TooltipWidgetComponent->SetVisibility(false);

		StartGlowTimeline(DynamicMaterialInstance);
	}
}

void ABeacon_StartStage::OnGlowTimelineFinished()
{
	ServerInteract();
}

void ABeacon_StartStage::BeginPlay()
{
	Super::BeginPlay();

	DynamicMaterialInstance = UMaterialInstanceDynamic::Create(MeshComponent->GetMaterial(0), this);
	MeshComponent->SetMaterial(0, DynamicMaterialInstance);

	PlaySpawnBeaconLevelSequence();
}

void ABeacon_StartStage::PlaySpawnBeaconLevelSequence()
{
	SetActorHiddenInGame(true);

	// Disable Input
	APlayerController* PC = GetWorld() ? GetWorld()->GetFirstPlayerController<APlayerController>() : nullptr;
	if (PC)
	{
		PC->DisableInput(PC);
	}
	
	// Hide HUD
	if (const AAuraHUD* AuraHUD = PC ? PC->GetHUD<AAuraHUD>() : nullptr)
	{
		AuraHUD->ShowGameOverlay(false);
	}

	// Play
	if (const IPlayerInterface* PlayerInterface = Cast<IPlayerInterface>(GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr))
	{
		if (ULevelSequenceManageComponent* LevelSequenceManageComponent = PlayerInterface->GetLevelSequenceManageComponent())
		{
			FVector NewLocation = GetActorLocation();
			NewLocation.Z = 0.f;
			LevelSequenceManageComponent->SetLevelSequenceActorLocation(TEXT("SpawnBeacon"), NewLocation);
			LevelSequenceManageComponent->OnLevelSequenceStopDelegate.AddUObject(this, &ThisClass::OnLevelSequenceStop);
			LevelSequenceManageComponent->PlayLevelSequence(TEXT("SpawnBeacon"));
		}
	}
}

void ABeacon_StartStage::OnLevelSequenceStop(const FName& LevelSequenceTag)
{
	SetActorHiddenInGame(false);

	// Enable Input
	APlayerController* PC = GetWorld() ? GetWorld()->GetFirstPlayerController<APlayerController>() : nullptr;
	if (PC)
	{
		PC->EnableInput(PC);
	}
	
	// Show HUD
	if (const AAuraHUD* AuraHUD = PC ? PC->GetHUD<AAuraHUD>() : nullptr)
	{
		AuraHUD->ShowGameOverlay(true);
	}
}

void ABeacon_StartStage::ServerInteract_Implementation()
{
	if (AStageGameMode* StageGameMode = GetWorld() ? GetWorld()->GetAuthGameMode<AStageGameMode>() : nullptr)
	{
		StageGameMode->OnStageBeaconInteracted();
	}
}
