// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "BaseCharacter.generated.h"

class UMotionWarpingComponent;
class UGameplayEffect;
class UGameplayAbility;
class UAttributeSet;
class UAbilitySystemComponent;

UCLASS(Abstract)
class AURA_API ABaseCharacter : public ACharacter, public IAbilitySystemInterface, public ICombatInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();

	/* Begin IAbilitySystemInterface */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; }
	/* End IAbilitySystemInterface */
	FORCEINLINE UAttributeSet* GetAttributeSet() const { return AttributeSet; }

	/* Begin CombatInterface */
	virtual FVector GetCombatSocketLocation_Implementation() const override;
	virtual FGameplayTag GetRoleTag_Implementation() const override { return RoleTag; }
	virtual void SetFacingTarget_Implementation(const FVector& TargetLocation) override;
	/* End CombatInterface */
	
protected:
	virtual void BeginPlay() override;

	/*
	 *	Weapon Mesh
	 */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> WeaponMeshComponent;

	UPROPERTY(EditDefaultsOnly, Category="Aura|Weapon Mesh")
	FName WeaponSocketName;

	UPROPERTY(EditDefaultsOnly, Category="Aura|Weapon Mesh")
	FName CombatSocketName;

	/*
	 *	GAS
	 */
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	// ASC, AS를 초기화하는 함수
	virtual void InitAbilityActorInfo() {}

	// Attributes를 초기화하는 함수
	virtual void InitializeAttributes() {}

	void ApplyEffectSpecToSelf(const TSubclassOf<UGameplayEffect>& EffectClass, float InLevel = 1.f) const;

	void AddStartupAbilities(const TArray<TSubclassOf<UGameplayAbility>>& AbilityClasses) const;

	/*
	 *	Role
	 */
	UPROPERTY(EditAnywhere, Category="Aura|Role")
	FGameplayTag RoleTag;

	/*
	 *	Motion Warping
	 */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UMotionWarpingComponent> MotionWarpingComponent;

	UPROPERTY(EditDefaultsOnly, Category="Aura|Motion Warping")
	FName WarpTargetName;
};
