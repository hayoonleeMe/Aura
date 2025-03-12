// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/OptionsMenu.h"

#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "UI/Widget/OptionTab.h"
#include "UI/Widget/SquareButton.h"
#include "UI/Widget/GraphicsOptionMenu.h"
#include "UI/Widget/SoundOptionMenu.h"

void UOptionsMenu::NativeConstruct()
{
	Super::NativeConstruct();

	OptionTab_Graphics->InternalButton->OnClicked.AddDynamic(this, &ThisClass::OnGraphicsTabClicked);
	OptionTab_Sound->InternalButton->OnClicked.AddDynamic(this, &ThisClass::OnSoundTabClicked);
	OptionTabs.Add(OptionTab_Graphics);
	OptionTabs.Add(OptionTab_Sound);
	
	WidgetSwitcher->SetActiveWidget(GraphicsOptionMenu);
	OptionTab_Graphics->Select();
	
	Button_Save->InternalButton->OnClicked.AddDynamic(this, &ThisClass::OnSaveButtonClicked);
	Button_Reset->InternalButton->OnClicked.AddDynamic(this, &ThisClass::OnResetButtonClicked);
	Button_Close->InternalButton->OnClicked.AddDynamic(this, &ThisClass::OnCloseButtonClicked);
}

void UOptionsMenu::OnGraphicsTabClicked()
{
	WidgetSwitcher->SetActiveWidget(GraphicsOptionMenu);
	for (UOptionTab* Tab : OptionTabs)
	{
		if (Tab != OptionTab_Graphics)
		{
			Tab->Deselect();
		}
	}
}

void UOptionsMenu::OnSoundTabClicked()
{
	WidgetSwitcher->SetActiveWidget(SoundOptionMenu);
	for (UOptionTab* Tab : OptionTabs)
	{
		if (Tab != OptionTab_Sound)
		{
			Tab->Deselect();
		}
	}
}

void UOptionsMenu::OnSaveButtonClicked()
{
}

void UOptionsMenu::OnResetButtonClicked()
{
}

void UOptionsMenu::OnCloseButtonClicked()
{
	RemoveFromParent();
}
