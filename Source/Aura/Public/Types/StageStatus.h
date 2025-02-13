#pragma once

#include "StageStatus.generated.h"

/**
 * Stage 상태를 나타내는 enum
 * Waiting : Stage가 종료된 후 Start 까지 기다리는 상태
 * Started : Waiting 중인 Stage를 시작한 상태
 */
UENUM(BlueprintType)
enum class EStageStatus : uint8
{
	Waiting,
	Started
};

inline const TCHAR* LexToString(EStageStatus StageStatus)
{
	switch (StageStatus)
	{
	case EStageStatus::Waiting:
		return TEXT("EStageStatus::Waiting");
	case EStageStatus::Started:
		return TEXT("EStageStatus::Started");
	}
	return TEXT("");
}
