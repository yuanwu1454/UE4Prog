// Nothing ...

#pragma once

#include "CoreMinimal.h"
#include "Anim/KingBaseAnimInstance.h"
#include "KingCharacterAnimInstance.generated.h"

class ABaseCharacter;
class UCharacterMovementComponent;
/**
 * 
 */
UCLASS()
class MYGAME_API UKingCharacterAnimInstance : public UKingBaseAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
protected:
	UPROPERTY()
	ABaseCharacter* OwningCharacter;

	UPROPERTY()
	UCharacterMovementComponent* OwningMovementComponent;


	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly, Category="AnimData|LocomotionData")
	float GroundSpeed;
	
	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly, Category="AnimData|LocomotionData")
	bool bHasAcceleration;
};
