// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/TutorialMenu.h"

#include "Components/Button.h"
#include "UI/Widget/SquareButton.h"

void UTutorialMenu::CloseMenu()
{
	Super::CloseMenu();
	
	RemoveFromParent();
}

void UTutorialMenu::NativeConstruct()
{
	Super::NativeConstruct();
	
	Button_Close->InternalButton->OnClicked.AddDynamic(this, &ThisClass::CloseMenu);
}
