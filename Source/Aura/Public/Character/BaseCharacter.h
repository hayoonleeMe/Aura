// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

class UAttributeSet;
class UAbilitySystemComponent;

UCLASS(Abstract)
class AURA_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseCharacter();

protected:
	virtual void BeginPlay() override;

	/*
	 *	Weapon Mesh
	 */
	UPROPERTY(VisibleAnywhere, Category="Weapon Mesh")
	TObjectPtr<USkeletalMeshComponent> WeaponMeshComponent;

	UPROPERTY(EditDefaultsOnly, Category="Weapon Mesh")
	FName WeaponSocketName;

	/*
	 *	GAS
	 */
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	// ASC, AS를 초기화하는 함수
	virtual void InitAbilityActorInfo() {}
};
