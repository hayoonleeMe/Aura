// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LevelSequenceManageComponent.generated.h"

class ULevelSequence;
class ALevelSequenceActor;

// Level Sequence가 멈추면 Broadcast하고 해당 Level Sequence Tag를 전달한다.
DECLARE_MULTICAST_DELEGATE_OneParam(FOnLevelSequenceStopSignature, const FName& /*LevelSequenceTag*/);

/**
 * Level Sequence를 관리하는 Actor Component
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class AURA_API ULevelSequenceManageComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULevelSequenceManageComponent();
	
	// 관리할 Level Sequence Tag 설정 
	void SetLevelSequenceTags(TArray<FName>&& Tags);

	FOnLevelSequenceStopSignature OnLevelSequenceStopDelegate;

	void AttachLevelSequenceActorToPawn(const FName& LevelSequenceTag, APawn* Pawn, bool bJumpToEndTime);

	void PlayLevelSequence(const FName& LevelSequenceTag);
	void StopLevelSequence(const FName& LevelSequenceTag);

	void SetLevelSequenceActorLocation(const FName& LevelSequenceTag, const FVector& NewLocation);

protected:
	virtual void InitializeComponent() override;

private:
	TArray<FName> LevelSequenceActorTags;
	
	TMap<FName, TObjectPtr<ALevelSequenceActor>> LevelSequenceActorMap;

	FName CurrentPlayingLevelSequenceTag;
	
	UFUNCTION()
	void OnLevelSequencePlayerStop();
};
