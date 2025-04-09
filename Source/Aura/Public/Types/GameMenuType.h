#pragma once

#include "GameMenuType.generated.h"

/**
 * 게임 내 메뉴의 타입을 나타낸다.
 */
UENUM()
enum class EGameMenuType : uint8
{
	AttributeMenu,
	SpellMenu,
	PauseMenu,
	TutorialMenu
};

inline const TCHAR* LexToString(EGameMenuType GameMenuType)
{
	switch (GameMenuType)
	{
	case EGameMenuType::AttributeMenu:
		return TEXT("Attribute Menu");
	case EGameMenuType::SpellMenu:
		return TEXT("Spell Menu");
	case EGameMenuType::PauseMenu:
		return TEXT("Pause Menu");
	case EGameMenuType::TutorialMenu:
		return TEXT("Tutorial Menu");
	}
	return TEXT("");
}
