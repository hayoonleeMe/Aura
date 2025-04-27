// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "Interface/CombatInterface.h"
#include "Interface/PlayerInterface.h"
#include "AuraCharacter.generated.h"

class UBoxComponent;
class UWidgetComponent;
class UObjectPoolComponent;
class UNiagaraComponent;
class UCameraComponent;
class USpringArmComponent;
class UGameplayEffect;

/**
 * 
 */
UCLASS()
class AURA_API AAuraCharacter : public ABaseCharacter, public IPlayerInterface
{
	GENERATED_BODY()

public:
	AAuraCharacter();
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	/* Begin CombatInterface */
	virtual void Die() override;
	/* End CombatInterface */

	/* Begin PlayerInterface */
	virtual int32 GetAttributePoints() const override;
	virtual void AddToAttributePoints(int32 Points) override;
	virtual void IncrementAttributePoints() override;
	virtual void DecrementAttributePoints() override;
	virtual int32 GetSpellPoints() const override;
	virtual void AddToSpellPoints(int32 Points) override;
	virtual void IncrementSpellPoints() override;
	virtual void DecrementSpellPoints() override;
	virtual void IndicateDamage(float Damage, bool bIsBlockedHit, bool bIsCriticalHit, const FVector_NetQuantize& TargetLocation) const override;
	virtual int32 GetLevelUpXpRequirement(int32 Level) const override;
	virtual int32 GetLevelUpAttributePointsAward(int32 Level) const override;
	virtual int32 GetLevelUpSpellPointsAward(int32 Level) const override;
	/* End PlayerInterface */

	// ============================================================================
	// Test
	// ============================================================================

	// XPAmount만큼의 XP를 획득
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ServerAddXPForTest(int32 XPAmount);

	// 레벨업
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ServerLevelUpForTest();

	// Attribute Point 획득
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ServerAddAttributePointForTest();

	// Spell Point 획득
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ServerAddSpellPointForTest();
	
protected:
	virtual void BeginPlay() override;
	
	virtual void InitAbilityActorInfo() override;
	virtual void InitializeAttributes() override;
	virtual void HandleDeathLocally() override;

private:
	// ============================================================================
	// Effects
	// ============================================================================

	UPROPERTY(EditDefaultsOnly, Category="Aura|Effects")
	TArray<TSubclassOf<UGameplayEffect>> StartupEffects;

	// ============================================================================
	// Camera
	// ============================================================================

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArmComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UCameraComponent> CameraComponent;

	// ============================================================================
	// Passive Spell Niagara Component
	// ============================================================================

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UNiagaraComponent> HaloOfProtectionComponent;

	void OnHaloOfProtectionSiphonTagChanged(const FGameplayTag Tag, int32 NewCount) const;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UNiagaraComponent> HealthSiphonComponent;

	void OnHealthSiphonTagChanged(const FGameplayTag Tag, int32 NewCount) const;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UNiagaraComponent> ManaSiphonComponent;

	void OnManaSiphonTagChanged(const FGameplayTag Tag, int32 NewCount) const;

	// ============================================================================
	// Invincibility
	// ============================================================================

	// 무적 기능을 부여하는 GameplayEffect Class
	UPROPERTY(EditDefaultsOnly, Category="Aura|Invincibility")
	TSubclassOf<UGameplayEffect> InvincibilityEffectClass;

	// 무적 효과를 표시하기 위한 Material
	UPROPERTY(EditDefaultsOnly, Category="Aura|Invincibility")
	TObjectPtr<UMaterialInterface> InvincibilityEffectMaterial;

	// Invincibility Tag가 변경되면 호출되는 콜백 함수
	void OnInvincibilityTagChanged(const FGameplayTag Tag, int32 NewCount) const;

	// 리스폰 시 무적 시간
	UPROPERTY(EditDefaultsOnly, Category="Aura|Invincibility")
	float RespawnInvincibilityTime;

	// ============================================================================
	// Player Nameplate
	// ============================================================================

	// 플레이어 아이디를 표시하는 위젯 컴포넌트
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UWidgetComponent> PlayerNameplateWidgetComponent;

	// PlayerNameplate 위젯에 플레이어 아이디 설정
	void InitializePlayerNameplateWidget() const;

	// ============================================================================
	// Obstacle Check
	// ============================================================================

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> ObstacleCheckBox;
};
