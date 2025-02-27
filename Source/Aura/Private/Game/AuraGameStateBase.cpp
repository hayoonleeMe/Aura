// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/AuraGameStateBase.h"

#include "AuraBlueprintLibrary.h"
#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "GameFramework/Character.h"
#include "Player/AuraPlayerController.h"

AAuraGameStateBase::AAuraGameStateBase()
{
	/* Respawn */
	TotalLifeCount = 3;
	RespawnTime = 7.f;
}

void AAuraGameStateBase::AttachPauseMenuLevelSequenceActorToPawn(APawn* Pawn) const
{
	if (PauseMenuLevelSequenceActor && Pawn)
	{
		// Pawn에 부착한다.
		if (PauseMenuLevelSequenceActor->AttachToActor(Pawn, FAttachmentTransformRules::SnapToTargetNotIncludingScale))
		{
			ULevelSequencePlayer* LevelSequencePlayer = PauseMenuLevelSequenceActor->SequencePlayer;
			AAuraPlayerController* AuraPC = Pawn->GetController<AAuraPlayerController>();
			
			if (LevelSequencePlayer && AuraPC)
			{
				// 중복 Bind 방지
				if (!LevelSequencePlayer->OnStop.IsAlreadyBound(AuraPC, &AAuraPlayerController::OnLevelSequencePlayerStop))
				{
					// LevelSequencePlayer Stop 시 OnLevelSequencePlayerStop를 호출해 ViewTarget을 플레이어 캐릭터로 되돌린다.
					LevelSequencePlayer->OnStop.AddDynamic(AuraPC, &AAuraPlayerController::OnLevelSequencePlayerStop);
				}

				// PauseMenu가 열린 채로 죽었다 리스폰되는 경우, PauseMenu Level Sequence의 끝 프레임으로 점프
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

void AAuraGameStateBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// Caching Level Sequence Actor
	PauseMenuLevelSequenceActor = UAuraBlueprintLibrary::GetLevelSequenceActorMatchingTag(GetWorld(), TEXT("PauseMenu"));	// TODO : Parameterize?
}
