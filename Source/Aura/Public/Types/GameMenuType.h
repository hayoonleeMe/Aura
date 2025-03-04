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
	PauseMenu
};
