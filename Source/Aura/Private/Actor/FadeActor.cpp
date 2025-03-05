// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/FadeActor.h"

#include "Aura/Aura.h"

AFadeActor::AFadeActor()
{
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	SetRootComponent(MeshComponent);
	MeshComponent->SetGenerateOverlapEvents(true);
	MeshComponent->SetCollisionObjectType(ECC_WorldStatic);
	MeshComponent->SetCollisionResponseToChannel(ECC_ClickToMove, ECR_Ignore);
	MeshComponent->SetCollisionResponseToChannel(ECC_Target, ECR_Ignore);
	MeshComponent->SetCollisionResponseToChannel(ECC_OnlyWall, ECR_Block);

	// AuraCharacter
	MeshComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::BeginOverlap);
	MeshComponent->OnComponentEndOverlap.AddDynamic(this, &ThisClass::EndOverlap);
}

void AFadeActor::EnableFade()
{
	if (FadeMaterial)
	{
		OriginalMaterial = MeshComponent->GetMaterial(0);
		MeshComponent->SetMaterial(0, FadeMaterial);
	}
}

void AFadeActor::DisableFade()
{
	MeshComponent->SetMaterial(0, OriginalMaterial);
}

void AFadeActor::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	EnableFade();
}

void AFadeActor::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	DisableFade();
}
