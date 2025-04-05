// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Beacon_StartStage.h"

#include "Aura/Aura.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Game/StageGameMode.h"
#include "Kismet/GameplayStatics.h"

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
		if (InteractSound)
		{
			UGameplayStatics::PlaySound2D(this, InteractSound);
		}
		ServerInteract();
		bHasInteractedWithPlayer = true;

		TooltipWidgetComponent->SetVisibility(false);
	}
}

void ABeacon_StartStage::ServerInteract_Implementation()
{
	if (AStageGameMode* StageGameMode = GetWorld() ? GetWorld()->GetAuthGameMode<AStageGameMode>() : nullptr)
	{
		StageGameMode->OnStageBeaconInteracted();
	}
}
