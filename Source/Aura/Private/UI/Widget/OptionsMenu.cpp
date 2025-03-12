// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/OptionsMenu.h"

#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "GameUserSettings/AuraGameUserSettings.h"
#include "UI/Widget/OptionTab.h"
#include "UI/Widget/SquareButton.h"
#include "UI/Widget/GraphicsOptionMenu.h"
#include "UI/Widget/SoundOptionMenu.h"

void UOptionsMenu::NativeConstruct()
{
	Super::NativeConstruct();

	AuraGameUserSettings = CastChecked<UAuraGameUserSettings>(GEngine ? GEngine->GetGameUserSettings() : nullptr);

	OptionTab_Graphics->InternalButton->OnClicked.AddDynamic(this, &ThisClass::OnGraphicsTabClicked);
	OptionTab_Sound->InternalButton->OnClicked.AddDynamic(this, &ThisClass::OnSoundTabClicked);
	OptionTabs.Add(OptionTab_Graphics);
	OptionTabs.Add(OptionTab_Sound);
	
	WidgetSwitcher->SetActiveWidget(GraphicsOptionMenu);
	OptionTab_Graphics->Select();
	
	Button_Save->InternalButton->OnClicked.AddDynamic(this, &ThisClass::OnSaveButtonClicked);
	Button_Revert->InternalButton->OnClicked.AddDynamic(this, &ThisClass::OnRevertButtonClicked);
	Button_Close->InternalButton->OnClicked.AddDynamic(this, &ThisClass::OnCloseButtonClicked);

	Button_Save->InternalButton->SetIsEnabled(false);
	GraphicsOptionMenu->OnOptionChangedDelegate.AddUObject(this, &ThisClass::OnOptionChanged);
	SoundOptionMenu->OnOptionChangedDelegate.AddUObject(this, &ThisClass::OnOptionChanged);
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
	// 옵션 적용
	AuraGameUserSettings->ApplyCustomSettings(false, GetWorld());
	GraphicsOptionMenu->OnOptionSaved();
	SoundOptionMenu->OnOptionSaved();
	Button_Save->InternalButton->SetIsEnabled(false);
}

void UOptionsMenu::OnRevertButtonClicked()
{
	GraphicsOptionMenu->RevertChanges();
	SoundOptionMenu->RevertChanges();
}

void UOptionsMenu::OnCloseButtonClicked()
{
	RemoveFromParent();
}

void UOptionsMenu::OnOptionChanged()
{
	// 옵션이 변경되면 Save Button 활성화/비활성화
	Button_Save->InternalButton->SetIsEnabled(GraphicsOptionMenu->HasOptionChanged() | SoundOptionMenu->HasOptionChanged());
}
