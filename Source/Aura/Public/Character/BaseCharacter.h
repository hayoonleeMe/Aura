// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Character.h"
#include "Interface/CombatInterface.h"
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
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/* Begin IAbilitySystemInterface */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; }
	/* End IAbilitySystemInterface */
	FORCEINLINE UAttributeSet* GetAttributeSet() const { return AttributeSet; }

	/* Begin CombatInterface */
	virtual FGameplayTag GetRoleTag() const override { return RoleTag; }
	virtual void SetFacingTarget(const FVector& TargetLocation) override;
	virtual FTaggedCombatInfo GetTaggedCombatInfo(const FGameplayTag& InTag) const override;
	virtual FVector GetCombatSocketLocation(const FName& CombatSocketName) const override;
	virtual FTransform GetCombatSocketTransform(const FName& CombatSocketName) const override;
	virtual void Die() override;
	virtual bool IsDead() const override { return bDead; }
	virtual FOnCharacterDeadSignature* GetOnCharacterDeadDelegate() override { return &OnCharacterDeadDelegate; }
	/* End CombatInterface */

	// ============================================================================
	// Dead
	// ============================================================================

	UPROPERTY(BlueprintAssignable)
	FOnCharacterDeadSignature OnCharacterDeadDelegate;
	
protected:
	virtual void BeginPlay() override;

	// ============================================================================
	// Weapon Mesh
	// ============================================================================

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> WeaponMeshComponent;

	UPROPERTY(EditDefaultsOnly, Category="Aura|Weapon Mesh")
	FName WeaponSocketName;

	// ============================================================================
	// GAS
	// ============================================================================

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	// ASC, AS를 초기화하는 함수
	virtual void InitAbilityActorInfo() {}

	// Attributes를 초기화하는 함수
	virtual void InitializeAttributes() {}


	void AddStartupAbilities(const TArray<TSubclassOf<UGameplayAbility>>& AbilityClasses, int32 InLevel = 1) const;

	// ============================================================================
	// Role
	// ============================================================================

	UPROPERTY(EditAnywhere, Category="Aura|Role")
	FGameplayTag RoleTag;

	// ============================================================================
	// Motion Warping
	// ============================================================================

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UMotionWarpingComponent> MotionWarpingComponent;

	UPROPERTY(EditDefaultsOnly, Category="Aura|Motion Warping")
	FName WarpTargetName;

	// ============================================================================
	// Combat
	// ============================================================================

	UPROPERTY(EditDefaultsOnly, Category="Aura|Combat")
	TArray<FTaggedCombatInfo> TaggedCombatInfos;

	// ============================================================================
	// Abilities
	// ============================================================================

	UPROPERTY(EditDefaultsOnly, Category="Aura|Ability")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

	// ============================================================================
	// Dead
	// ============================================================================

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Dead, Category="Aura|Dead", meta=(AllowPrivateAccess=true))
	uint8 bDead : 1;

	UFUNCTION()
	virtual void OnRep_Dead();

	// Local에서만 적용되는 Dead 관련 작업을 수행
	virtual void HandleDeathLocally() {}
};
