// Nothing ...

#pragma once

#include "CoreMinimal.h"
#include "KingCharacterAnimInstance.h"
#include "KingHeroAnimInstance.generated.h"

class AKingCharacter;
/**
 * 
 */
UCLASS()
class MYGAME_API UKingHeroAnimInstance : public UKingCharacterAnimInstance
{
	GENERATED_BODY()
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="AnimData|Reference")
	AKingCharacter* OwningKingCharacter;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="AnimData|LocomotionData")
	bool bShouldEnterRelaxState;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="AnimData|LocomotionData")
	float EnterRelaxStateThreshold =5.f;

	float IdleElpasedTime;
	
};


