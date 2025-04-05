// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/BaseHUD.h"

#include "Interface/PlayerInterface.h"
#include "Interface/ClosableWidgetInterface.h"
#include "UI/Widget/OptionsMenu.h"

void ABaseHUD::RegisterWidget(UObject* Object)
{
	if (IClosableWidgetInterface* WidgetCloseInterface = Cast<IClosableWidgetInterface>(Object))
	{
		// 처음 UI 위젯이 열릴 때만 UI Input
		if (CurrentWidgetStack.IsEmpty())
		{
			if (IPlayerInterface* PlayerInterface = Cast<IPlayerInterface>(GetOwningPlayerController()))
			{
				PlayerInterface->EnableUIInput();
			}
		}
	
		WidgetCloseInterface->GetOnRemovedDelegate().AddUObject(this, &ThisClass::UnregisterCurrentWidget);
		CurrentWidgetStack.Push(Object);
	}
}

void ABaseHUD::UnregisterCurrentWidget()
{
	if (CurrentWidgetStack.Num())
	{
		CurrentWidgetStack.Pop();
	}

	// 열린 UI 위젯이 없으면 Input 되돌리기
	if (CurrentWidgetStack.IsEmpty())
	{
		if (IPlayerInterface* PlayerInterface = Cast<IPlayerInterface>(GetOwningPlayerController()))
		{
			PlayerInterface->DisableUIInput();
		}
	}
}

void ABaseHUD::CloseCurrentWidget()
{
	if (CurrentWidgetStack.Num() && CurrentWidgetStack.Top().IsValid())
	{
		if (IClosableWidgetInterface* WidgetCloseInterface = Cast<IClosableWidgetInterface>(CurrentWidgetStack.Top()))
		{
			WidgetCloseInterface->CloseMenu();
		}
	}
}

void ABaseHUD::AddOptionsMenu() const
{
	check(OptionsMenuWidgetClass);
	if (UOptionsMenu* OptionsMenu = CreateWidget<UOptionsMenu>(GetOwningPlayerController(), OptionsMenuWidgetClass, TEXT("OptionsMenu")))
	{
		OptionsMenu->SetAnchorsInViewport(FAnchors(0.f, 0.f, 1.f, 1.f));
		OptionsMenu->AddToViewport(2);	// 최상단
	}
}
