// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/LevelSequenceManageComponent.h"

#include "AuraBlueprintLibrary.h"
#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"

ULevelSequenceManageComponent::ULevelSequenceManageComponent()
{
	bAutoActivate = true;
	bWantsInitializeComponent = true;
	SetIsReplicatedByDefault(false);
}

void ULevelSequenceManageComponent::SetLevelSequenceTags(TArray<FName>&& Tags)
{
	LevelSequenceActorTags = MoveTemp(Tags);
}

void ULevelSequenceManageComponent::AttachLevelSequenceActorToPawn(const FName& LevelSequenceTag, APawn* Pawn, bool bJumpToEndTime)
{
	if (!IsValid(Pawn) || !LevelSequenceActorMap.Contains(LevelSequenceTag))
	{
		return;
	}

	if (ALevelSequenceActor* LevelSequenceActor = LevelSequenceActorMap[LevelSequenceTag])
	{
		// Pawn에 부착한다.
		if (LevelSequenceActor->AttachToActor(Pawn, FAttachmentTransformRules::SnapToTargetNotIncludingScale))
		{
			if (bJumpToEndTime)
			{
				if (ULevelSequencePlayer* LevelSequencePlayer = LevelSequenceActor->SequencePlayer)
				{
					// 중단됐다가 다시 부착된다면 EndTime에서 재생
					if (LevelSequencePlayer->IsPaused())
					{
						const FMovieSceneSequencePlaybackParams Params(LevelSequencePlayer->GetEndTime().Time, EUpdatePositionMethod::Jump);
						LevelSequencePlayer->SetPlaybackPosition(Params);
						LevelSequencePlayer->Play();
					}
				}
			}
		}
	}
}

void ULevelSequenceManageComponent::PlayLevelSequence(const FName& LevelSequenceTag)
{
	if (LevelSequenceActorMap.Contains(LevelSequenceTag))
	{
		if (const ALevelSequenceActor* LevelSequenceActor = LevelSequenceActorMap[LevelSequenceTag])
		{
			if (ULevelSequencePlayer* LevelSequencePlayer = LevelSequenceActor->SequencePlayer)
			{
				LevelSequencePlayer->Play();
				CurrentPlayingLevelSequenceTag = LevelSequenceTag;
			}
		}
	}
}

void ULevelSequenceManageComponent::StopLevelSequence(const FName& LevelSequenceTag)
{
	if (LevelSequenceActorMap.Contains(LevelSequenceTag))
	{
		if (const ALevelSequenceActor* LevelSequenceActor = LevelSequenceActorMap[LevelSequenceTag])
		{
			if (ULevelSequencePlayer* LevelSequencePlayer = LevelSequenceActor->SequencePlayer)
			{
				LevelSequencePlayer->Stop();
			}
		}
	}
}

void ULevelSequenceManageComponent::StopCurrentPlayingLevelSequence()
{
	StopLevelSequence(CurrentPlayingLevelSequenceTag);
}

bool ULevelSequenceManageComponent::IsPlayingLevelSequence() const
{
	if (LevelSequenceActorMap.Contains(CurrentPlayingLevelSequenceTag))
	{
		if (const ALevelSequenceActor* LevelSequenceActor = LevelSequenceActorMap[CurrentPlayingLevelSequenceTag])
		{
			return LevelSequenceActor->SequencePlayer && LevelSequenceActor->SequencePlayer->IsPlaying();
		}
	}
	return false;
}

void ULevelSequenceManageComponent::SetLevelSequenceActorLocation(const FName& LevelSequenceTag, const FVector& NewLocation)
{
	if (LevelSequenceActorMap.Contains(LevelSequenceTag))
	{
		if (ALevelSequenceActor* LevelSequenceActor = LevelSequenceActorMap[LevelSequenceTag])
		{
			LevelSequenceActor->SetActorLocation(NewLocation);
		}
	}
}

void ULevelSequenceManageComponent::InitializeComponent()
{
	Super::InitializeComponent();

	// Level에 배치된 Level Sequence Actor 캐싱
	for (const FName& Tag : LevelSequenceActorTags)
	{
		if (ALevelSequenceActor* LevelSequenceActor = UAuraBlueprintLibrary::GetLevelSequenceActorMatchingTag(GetWorld(), Tag))
		{
			if (ULevelSequencePlayer* LevelSequencePlayer = LevelSequenceActor->SequencePlayer)
			{
				LevelSequencePlayer->OnStop.AddDynamic(this, &ThisClass::OnLevelSequencePlayerStop);
			}
			LevelSequenceActorMap.Add(Tag, LevelSequenceActor);
		}
	}
}

void ULevelSequenceManageComponent::OnLevelSequencePlayerStop()
{
	OnLevelSequenceStopDelegate.Broadcast(CurrentPlayingLevelSequenceTag);
}
