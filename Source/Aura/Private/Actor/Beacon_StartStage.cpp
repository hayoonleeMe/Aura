// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Beacon_StartStage.h"

#include "Aura/Aura.h"
#include "Components/BoxComponent.h"
#include "Game/StageGameMode.h"

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
}

void ABeacon_StartStage::HighlightActor()
{
	MeshComponent->SetRenderCustomDepth(true);
}

void ABeacon_StartStage::UnHighlightActor()
{
	MeshComponent->SetRenderCustomDepth(false);
}

void ABeacon_StartStage::Interact()
{
	ServerInteract();
}

void ABeacon_StartStage::ServerInteract_Implementation()
{
	if (AStageGameMode* StageGameMode = GetWorld() ? GetWorld()->GetAuthGameMode<AStageGameMode>() : nullptr)
	{
		StageGameMode->StartStage();
	}
	Destroy();
}
