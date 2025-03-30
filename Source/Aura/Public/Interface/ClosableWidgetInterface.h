// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ClosableWidgetInterface.generated.h"

// 위젯이 제거될 때 호출되는 델레게이트
DECLARE_MULTICAST_DELEGATE(FOnRemovedSignature);

// This class does not need to be modified.
UINTERFACE()
class UClosableWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 닫을 수 있는 위젯의 기능을 구성하는 인터페이스
 */
class AURA_API IClosableWidgetInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION()
	virtual void CloseMenu() = 0;

	virtual FOnRemovedSignature& GetOnRemovedDelegate() = 0;
};
