// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FadeActor.generated.h"

/**
 * 캐릭터를 가리면 Static Mesh를 숨기는 액터
 */
UCLASS()
class AURA_API AFadeActor : public AActor
{
	GENERATED_BODY()

public:
	AFadeActor();
	
	void EnableFade();
	void DisableFade();

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UMaterialInterface> FadeMaterial;

	UPROPERTY()
	TObjectPtr<UMaterialInterface> OriginalMaterial;

	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
